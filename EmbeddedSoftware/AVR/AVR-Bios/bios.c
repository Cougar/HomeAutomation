#include "bios_funs.h"
#include "avr_cfg.h"
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "flash.h"
#include "vectors.h" // Must be included after sfr_defs.h but before any ISR()
#include "can.h"

#define FUNCTmask		0x1E000000
#define FUNCTshift		25
#define FUNCCmask		0x01FF8000
#define FUNCCshift		15
#define NIDmask			0x00007E00
#define NIDshift		9
#define SIDmask			0x000001FF
#define SIDshift		0

#define FUNCTbios		0x00

#define FUNCCstartprg	0x81
#define FUNCCprgdata	0x82
#define FUNCCstopprg	0x83

#define PRG_STATE_IDLE 			0
#define PRG_STATE_PROGRAMMING	1


//---------------------------------------------------------------------------
// Globals

volatile long gMilliSecTick;

//---------------------------------------------------------------------------
// Function definitions for the exported interface

void reset(void) {
	//Just loop and let the watchdog reset
	cli(); 
	while(1);
};

void bios_putchar(char c) {
	UART_PUTCHAR(c);
};

char bios_getchar(void) {
	char c;
	UART_GETCHAR(c)
	return c;
};

long timebase_get(void) {
	uint8_t sreg;
	long res;
	
	//Disable interrupts while reading tick count
	sreg=SREG;
	cli();
	
	res = gMilliSecTick;
	
	SREG=sreg;
	
	return res;
}

//---------------------------------------------------------------------------
// Function definitions for the private functions

static int console_getchar(FILE *stream) {
	return bios_getchar();
}

static int console_putchar(char c, FILE *stream) {
	bios_putchar(c);
	return 0;
}

static FILE bios_stdio = FDEV_SETUP_STREAM(console_putchar, console_getchar,
                                         _FDEV_SETUP_RW);
    

//---------------------------------------------------------------------------
// Interrupt Service Routines
// Read app_vectors.h!

ISR(TIMEBASE_VECTOR) {
	TIMEBASE_COUNTER -= TIMEBASE_RELOAD;
	gMilliSecTick++;
	wdt_reset();
}

int main() {
	void (*app_reset)(void) = 0;
	
	// Setup USART for debug channel
	UART_INIT();
	stdout = &bios_stdio;
	stdin = &bios_stdio;
	
	// Initialize a suitable hardware timer to create a 1KHz interrupt.
	TIMEBASE_INIT();
	
	// Enable watchdog timer to protect against an application locking the
	// node by leaving interrupts disabled. 
	wdt_enable(WDTO_500MS);
	
	// Move interrupt vectors to start of bootloader section and enable interrupts
	IVSEL_REG = _BV(IVCE);
	IVSEL_REG = _BV(IVSEL);
	sei();
	
	printf("AVR BIOS\n");
	
	
	printf("CanInit...");
	if (Can_Init() != CAN_OK) {
		printf("FAILED!\n");
	}
	else {
		printf("OK!\n");
	}
	
	uint32_t timeStamp = 0;
	
	Can_Message_t txMsg;
	Can_Message_t rxMsg;
	//The databytes are just what happens to be in the memory. They are never set.
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1; 

	txMsg.Id = 15;
	txMsg.DataLength = 2;
	
	uint16_t address;
	uint16_t lastoffset=0x0;
	uint8_t prg_state=PRG_STATE_IDLE;
	
	uint8_t i;
	/* main loop */
	while (1) {
		/* service the CAN routines */
		Can_Service();
		
		/* send CAN message and check for CAN errors once every second */
		if ((timebase_get() - timeStamp) >= 10000) {
			timeStamp = timebase_get();

			/* send txMsg */
			//txMsg.Id++;
			Can_Send(&txMsg);
			
			if (prg_state==PRG_STATE_IDLE) {
				if (pgm_read_word(0) != 0xffff) {
					printf("Starting application.\n");
					app_reset();
				}
				printf("No application found.\n");
			}
		}
		
		/* check if any messages have been received */
		while (Can_Receive(&rxMsg) == CAN_OK) {
			printf("BIOS Received: ID=%lx, DLC=%u, EXT=%u, RTR=%u, ", rxMsg.Id, (uint16_t)(rxMsg.DataLength), (uint16_t)(rxMsg.ExtendedFlag), (uint16_t)(rxMsg.RemoteFlag));
    		printf("data={ ");
    		for (i=0; i<rxMsg.DataLength; i++) {
    			printf("%x ", rxMsg.Data.bytes[i]);
    		}
    		printf("}\n");
    		
    		if ((rxMsg.Id & FUNCTmask) == ((uint32_t)FUNCTbios<<FUNCTshift)) {
    			//printf("Got 'bios package'\n");
    			//om mottagen data är bois-data (detta borde redan filtrerats i can_receive?)
    			if ((prg_state==PRG_STATE_IDLE) && ((rxMsg.Id & FUNCCmask) == ((uint32_t)FUNCCstartprg<<FUNCCshift)) && (rxMsg.DataLength == 6)) {
    				//om mottagen data har funktionskoden starta programmering

	    			//ska kolla om nod-id i IDENT stämmer med nodens id
	    			//om ok:
	    			prg_state=PRG_STATE_PROGRAMMING;
	    			address = rxMsg.Data.bytes[2];
	    			address |= rxMsg.Data.bytes[3];
	    			printf("Got 'start programming' at address %x\n", address);
	    		} else if (prg_state==PRG_STATE_PROGRAMMING) {
		    		if ((rxMsg.Id & FUNCCmask) == ((uint32_t)FUNCCprgdata<<FUNCCshift)) {
		    			//om mottagen data har funktionskoden programdata
		    			uint16_t tmpaddress = rxMsg.Data.bytes[0] + (rxMsg.Data.bytes[1] << 8);
		    			if (tmpaddress >= lastoffset) {
		    				tmpaddress += address;
			    			printf("Got 'programdatapacket' at offset %x\n", tmpaddress);
			    			for (i=2; i<rxMsg.DataLength; i+=2) {
			    				flash_write_word(tmpaddress, rxMsg.Data.bytes[i] + (rxMsg.Data.bytes[i+1] << 8));
			    				tmpaddress+=2;
			    			} 
			    			printf("Setting lastoffset to %x\n", tmpaddress);
		    				lastoffset = tmpaddress;
		    			}
		    		} else if ((rxMsg.Id & FUNCCmask) == ((uint32_t)FUNCCstopprg<<FUNCCshift)) {
		    			flash_flush_buffer();
		    			prg_state=PRG_STATE_IDLE;
		    			lastoffset=0x0;
		    			//beräkna crc på all programdata mellan address och address+lastoffset
		    			printf("Got 'end programming'\n");
		    		}
	    		}
    		}
    		
    		//if (rxMsg.Id % 20 == 19) reset();
		}
	}

	return 0;
}
