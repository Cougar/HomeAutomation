#include "bios_funs.h"
#include "avr_cfg.h"
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#ifdef BIOS_UART
#include <stdio.h>
#endif
#include <string.h>
#include <util/crc16.h>
#include "flash.h"
#include "vectors.h" // Must be included after sfr_defs.h but before any ISR()
#include "mcp2515/mcp2515.h"

//---------------------------------------------------------------------------
// Globals

volatile uint32_t gMilliSecTick;
volatile uint8_t bios_msg_full;
Can_Message_t* bios_msg_ptr; // only a pointer to main-local structure to save .bss space

//---------------------------------------------------------------------------
// Function definitions for the exported interface

void reset(void) {
	// Just loop and let the watchdog reset
	cli(); 
	while(1);
}

#ifdef BIOS_UART
void bios_putchar(char c) {
	UART_PUTCHAR(c);
}

char bios_getchar(void) {
	char c;
	UART_GETCHAR(c)
	return c;
}
#endif

unsigned long timebase_get(void) {
	uint8_t sreg;
	unsigned long res;
	
	// Disable interrupts while reading tick count
	sreg=SREG;
	cli();
	
	res = gMilliSecTick;
	
	SREG=sreg;
	
	return res;
}

// TODO: Insert can driver's Can_Send directly in bios interface.
Can_Return_t can_send(Can_Message_t* msg) {
	return Can_Send(msg);
}

//---------------------------------------------------------------------------
// Function definitions for the private functions

#ifdef BIOS_UART
static int console_getchar(FILE *stream) {
	return bios_getchar();
}

static int console_putchar(char c, FILE *stream) {
	bios_putchar(c);
	return 0;
}

static FILE bios_stdio = FDEV_SETUP_STREAM(console_putchar, console_getchar,
                                         _FDEV_SETUP_RW);
#endif

//---------------------------------------------------------------------------
// Interrupt Service Routines
// Read app_vectors.h!

ISR(TIMEBASE_VECTOR) {
	TIMEBASE_COUNTER -= TIMEBASE_RELOAD;
	gMilliSecTick++;
	wdt_reset();
}

void Can_Process(Can_Message_t* msg) {
	if (!(msg->ExtendedFlag)) return; // We don't care about standard CAN frames.
	
	if ((msg->Id & CAN_MASK_CLASS)>>CAN_SHIFT_CLASS == CAN_NMT) {
		
		if ((msg->Id & CAN_MASK_NMT_TYPE)>>CAN_SHIFT_NMT_TYPE == CAN_NMT_TIME) {
			// Reset watchdog as long as time messages from the master node
			// are received periodically. If anything hangs the CAN communication
			// or the master node is not up, the node will be reset.
			wdt_reset();
			//TODO: Don't care about time right now... Figure out what to do with it. 
		}
		
		if ((msg->Id & CAN_MASK_NMT_RID)>>CAN_SHIFT_NMT_RID == NODE_ID) {

			if ((msg->Id & CAN_MASK_NMT_TYPE)>>CAN_SHIFT_NMT_TYPE == CAN_NMT_RESET) {
				reset();
			}
			// Copy message to bios buffer if bios is done with the previous message.
			// This is also a check to see if bios is still running. When app is
			// started, bios_msg_full is never reset to 0 so the check fails and all
			// NMT communication except reset is ignored. 
			if (!(bios_msg_full)) {
				memcpy(bios_msg_ptr, msg, sizeof(Can_Message_t));
				bios_msg_full = 1;
			}
		}		
	} else if (bios->can_callback) {
		//printf("Calling app callback.\n");
		bios->can_callback(msg);
	}
}

#define BIOS_APP	1
#define BIOS_NOAPP	2
#define BIOS_PGM	3

int main() {
	void (*app_reset)(void) = 0; // Function pointer to jump to application reset vector.
	uint8_t bios_state;
	uint8_t nmt_type;
	uint8_t pagebuf[SPM_PAGESIZE];
	// These unneccessary initializations uses 12 bytes flash, just to make the compiler happy!
	uint16_t base_addr = 0;
	uint16_t offset = 0;
	uint16_t addr = 0;
	uint8_t len;
	uint16_t data;
	uint8_t send_msg = 0;
	Can_Message_t tx_msg;
	Can_Message_t bios_msg;
	bios_msg_ptr = &bios_msg;
	
#ifdef BIOS_UART
	// Setup USART for debug channel
	UART_INIT();
	stdout = &bios_stdio;
	stdin = &bios_stdio;
#endif
	
	// Initialize a suitable hardware timer to create a 1KHz interrupt.
	//TODO: Any point in having timebase in bios after wdt_reset() is moved to Can_Process?
	TIMEBASE_INIT();
	
	// Enable watchdog timer to protect against an application locking the
	// node by leaving interrupts disabled. 
	wdt_enable(WDTO_2S);
	
	// Move interrupt vectors to start of bootloader section and enable interrupts
	IVSEL_REG = _BV(IVCE);
	IVSEL_REG = _BV(IVSEL);
	sei();
	
	if (Can_Init() != CAN_OK) reset();
	
	tx_msg.RemoteFlag = 0;
	tx_msg.ExtendedFlag = 1;
	tx_msg.Id = CAN_ID_NMT_BIOS_START;
	tx_msg.DataLength = 3;
	tx_msg.Data.words[0] = BIOS_VERSION;
#ifdef BIOS_UART
	printf("BIOS start\n");
#endif
	if (pgm_read_word(0) == 0xffff) {
		// No application in flash
		// Send CAN_NMT_BIOS_START(BIOS_VERSION, 0)
		tx_msg.Data.bytes[2] = 0;
		bios_state = BIOS_NOAPP;
	} else {
		// Application exists
		// Send CAN_NMT_BIOS_START(BIOS_VERSION, 1)
		tx_msg.Data.bytes[2] = 1;
		bios_state = BIOS_APP;
	}
	
	while (Can_Send(&tx_msg) != CAN_OK);

	tx_msg.DataLength = 2; // All msg sent after BIOS_START are length 2 so set it once.
	
	// Main loop
	while (1) {
		// Wait for message
		while (!bios_msg_full);
		
		nmt_type = (bios_msg.Id & CAN_MASK_NMT_TYPE)>>CAN_SHIFT_NMT_TYPE;
		
		switch (bios_state) {
		case BIOS_APP:
			if (nmt_type == CAN_NMT_START_APP) {
				app_reset(); // Will never return
			}
			// Fall through
		case BIOS_NOAPP:
			if (nmt_type == CAN_NMT_PGM_START) {
				// Set base address
				base_addr = bios_msg.Data.words[0];
				flash_init(pagebuf);
				//send CAN_NMT_PGM_ACK(offset)
				tx_msg.Id = CAN_ID_NMT_PGM_ACK;
				tx_msg.Data.words[0] = base_addr;
				send_msg = 1;
				bios_state = BIOS_PGM;
			}
			if (nmt_type == CAN_NMT_PGM_COPY) {
				// For BIOS updating over CAN. Upload bios to application area,
				// send this message with correct parameters to copy data from
				// application to bios area and pray it will come alive again.
				flash_copy_data(bios_msg.Data.words[0], 
				                bios_msg.Data.words[1], 
				                bios_msg.Data.words[2]);
				// flash_copy_data will never return.
			}
			break;
		case BIOS_PGM:
			// Default to send CAN_NMT_PGM_ACK(offset)
			tx_msg.Id = CAN_ID_NMT_PGM_ACK;
			
			if (nmt_type == CAN_NMT_PGM_DATA) {
				// Set address = base address + offset.
				offset = bios_msg.Data.words[0];
				addr = base_addr + offset;
				
				// One of ACK and NACK will be sent, both have offset as data.
				tx_msg.Data.words[0] = offset;
				send_msg = 1;

				// Flash all data sent, beginning at addr.
				len = (bios_msg.DataLength+1)/2; // Number of words in message, rounded up.
				uint8_t i;
				for (i=1; i<len; i++) { // Skip first word (offset).
					
					// Abort if trying to write in BIOS area.
					if (addr >= (uint16_t)&__bios_start) {
						// Send CAN_NMT_PGM_NACK(offset).
						tx_msg.Id = CAN_ID_NMT_PGM_NACK;
						break; //for loop
					}

					data = bios_msg.Data.words[i];
					flash_write_word(addr, data);
					addr += 2;
				}
				// Adjust addr if an odd number of bytes were sent, for use in crc calc.
				addr -= bios_msg.DataLength & 1; 
			}
			if (nmt_type == CAN_NMT_PGM_END) {
				// Make sure a partly filled page will be flashed.
				flash_flush_buffer();
				
				// Calculate crc on written data.
				//TODO: This will not work if data is not sent sequentially without
				// holes, starting with offset=0. We might want to send a NACK if the
				// offset in a CAN_NMT_PGM_DATA msg differs from the expected offset.
				uint16_t loc, crc = 0;
				for (loc = base_addr; loc < addr - 2; loc++) { // addr will be last location written + 2.
					crc = _crc16_update(crc, pgm_read_byte(loc));
				}
				
				tx_msg.Data.words[0] = crc;
				send_msg = 1;
				bios_state = BIOS_NOAPP;
			}
		}
		
		if (send_msg) {
			while (Can_Send(&tx_msg) != CAN_OK);
			send_msg = 0;
		}

		bios_msg_full = 0; // We're done with this message, ready for the next.
	}

	return 0;
}
