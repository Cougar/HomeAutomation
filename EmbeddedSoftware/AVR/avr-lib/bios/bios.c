/*
 * $Id$
 */
 
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <util/crc16.h>
#include <config.h>
#include <vectors.h> // Must be included after sfr_defs.h but before any ISR()
#include <drivers/can/can.h>
#include <flash.h>
#include <bios_export.h>
#include CAN_DRIVER_H

#if !defined( NODE_HW_ID )
#error No NODE_HW_ID in bios.inc, set NODE_HW_ID=<GENERATE_HW> and Ill generate it for you
#endif

#if defined(__AVR_ATmega8__)
#define IVSEL_REG GICR
#elif defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__)
#define IVSEL_REG MCUCR
#else
#error AVR device not supported!
#endif

#if defined(AUTOSTART)
#if AUTOSTART == 0 
#error Please choose a higher AUTOSTART value in bios.inc, the nod will cease to work, and you can only reflash node with ISP
#endif
#if AUTOSTART == 1
#error Please choose a higher AUTOSTART value in bios.inc, there might be problems with updating software on node
#endif
#endif

//---------------------------------------------------------------------------
// Private declarations

volatile uint8_t bios_msg_full;
Can_Message_t* bios_msg_ptr; // only a pointer to main-local structure to save .bss space
extern void __bios_start; // Start of BIOS area in flash, from ld-script.

prog_uint32_t hwid = NODE_HW_ID;

#if defined(AUTOSTART)
uint8_t autostart_cnt;
#endif

#define BIOS_APP		1
#define BIOS_NOAPP		2
#define BIOS_PGM		3
#define BIOS_END_PGM	4

//---------------------------------------------------------------------------
// Private functions

void Can_Process(Can_Message_t* msg) {
	if (!(msg->ExtendedFlag)) return; // We don't care about standard CAN frames.
	
	if ((msg->Id & CAN_MASK_CLASS)>>CAN_SHIFT_CLASS == CAN_NMT) {
		
		if ((msg->Id & CAN_MASK_NMT_TYPE)>>CAN_SHIFT_NMT_TYPE == CAN_NMT_TIME) {
			// Reset watchdog as long as time messages from the master node
			// are received periodically. If anything hangs the CAN communication
			// or the master node is not up, the node will be reset.
			wdt_reset();
			//TODO: Don't care about time right now... Figure out what to do with it. 
#if defined(AUTOSTART)
			autostart_cnt++;
#endif
		}
		
		if (((msg->Id & CAN_MASK_NMT_TYPE)>>CAN_SHIFT_NMT_TYPE) == CAN_NMT_RESET && msg->DataLength == 4 &&
				msg->Data.bytes[0] == (hwid&0xff) && msg->Data.bytes[1] == ((hwid>>8)&0xff) &&
				msg->Data.bytes[2] == ((hwid>>16)&0xff) && msg->Data.bytes[3] == ((hwid>>24)&0xff) ) {
			BIOS_Reset();
		}
		// Copy message to bios buffer if bios is done with the previous message.
		// This is also a check to see if bios is still running. When app is
		// started, bios_msg_full is never reset to 0 so the check fails and all
		// NMT communication except reset is ignored. 
		if (!(bios_msg_full)) {
			memcpy(bios_msg_ptr, msg, sizeof(Can_Message_t));
			bios_msg_full = 1;
		}
	} else if (BIOS_CanCallback) {
		//printf("Calling app callback.\n");
		BIOS_CanCallback(msg);
	}
}

int main(void) {
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

#if defined(AUTOSTART)
	autostart_cnt = 0;
#endif

	// Enable watchdog timer to protect against an application locking the
	// node by leaving interrupts disabled. 
	wdt_enable(WDTO_2S);
	
	// Move interrupt vectors to start of bootloader section and enable interrupts
	IVSEL_REG = _BV(IVCE);
	IVSEL_REG = _BV(IVSEL);
	sei();
	
	if (Can_Init() != CAN_OK) BIOS_Reset();
	
	tx_msg.RemoteFlag = 0;
	tx_msg.ExtendedFlag = 1;
	tx_msg.Id = CAN_ID_NMT_BIOS_START;
	tx_msg.DataLength = 8;
	tx_msg.Data.bytes[0] = BIOS_VERSION&0xff;
	tx_msg.Data.bytes[1] = (BIOS_VERSION>>8)&0xff;
	tx_msg.Data.bytes[4] = hwid&0xff;
	tx_msg.Data.bytes[5] = (hwid>>8)&0xff;
	tx_msg.Data.bytes[6] = (hwid>>16)&0xff;
	tx_msg.Data.bytes[7] = (hwid>>24)&0xff;

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
		while (!bios_msg_full) {
#if defined(AUTOSTART)
			if (bios_state == BIOS_APP && autostart_cnt == AUTOSTART) {
				app_reset();
			}
#endif
		}
		
		nmt_type = (bios_msg.Id & CAN_MASK_NMT_TYPE)>>CAN_SHIFT_NMT_TYPE;
		
		switch (bios_state) {
		case BIOS_APP:
			//if (nmt_type == CAN_NMT_START_APP) {
			if (nmt_type == CAN_NMT_START_APP && bios_msg.DataLength == 4 &&
					bios_msg.Data.bytes[0] == (hwid&0xff) && bios_msg.Data.bytes[1] == ((hwid>>8)&0xff) &&
					bios_msg.Data.bytes[2] == ((hwid>>16)&0xff) && bios_msg.Data.bytes[3] == ((hwid>>24)&0xff) ) {
				app_reset(); // Will never return
			}
			// Fall through
		case BIOS_NOAPP:
			//if (nmt_type == CAN_NMT_PGM_START) {
			if (nmt_type == CAN_NMT_PGM_START && bios_msg.DataLength == 8 &&
					bios_msg.Data.bytes[4] == (hwid&0xff) && bios_msg.Data.bytes[5] == ((hwid>>8)&0xff) &&
					bios_msg.Data.bytes[6] == ((hwid>>16)&0xff) && bios_msg.Data.bytes[7] == ((hwid>>24)&0xff) ) {
				// Set base address
				base_addr = bios_msg.Data.words[0];
				flash_init(pagebuf);
				//send CAN_NMT_PGM_ACK(offset)
				tx_msg.Id = CAN_ID_NMT_PGM_ACK;
				tx_msg.Data.words[0] = base_addr;
				send_msg = 1;
				bios_state = BIOS_PGM;
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
				for (loc = base_addr; loc < addr; loc++) { // addr will be last location written + 2.
					crc = _crc16_update(crc, pgm_read_byte(loc));
				}
				
				tx_msg.Data.words[0] = crc;
				send_msg = 1;
				bios_state = BIOS_END_PGM;
			}
			break;
		case BIOS_END_PGM:
			if (nmt_type == CAN_NMT_PGM_COPY) {
				// For BIOS updating over CAN. Upload bios to application area,
				// send this message with correct parameters to copy data from
				// application to bios area and pray it will come alive again.
				flash_copy_data(bios_msg.Data.words[0], 
				                bios_msg.Data.words[1], 
				                bios_msg.Data.words[2]);
				// flash_copy_data will never return.
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
