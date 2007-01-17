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
#include "mcp2515/mcp2515.h"

//---------------------------------------------------------------------------
// Globals

volatile long gMilliSecTick;

//---------------------------------------------------------------------------
// Function definitions for the exported interface

void reset(void) {
	//Just loop and let the watchdog reset
	cli(); 
	while(1);
}

void bios_putchar(char c) {
	UART_PUTCHAR(c);
}

char bios_getchar(void) {
	char c;
	UART_GETCHAR(c)
	return c;
}

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

// TODO: Insert can driver's Can_Send directly in bios interface.
Can_Return_t can_send(Can_Message_t* msg) {
	return CAN_FAIL;
}

//Can_Return_t can_receive(Can_Message_t *msg) {
//	return CAN_FAIL;
//}

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

void Can_Process(Can_Message_t* msg) {
	if (msg->ExtendedFlag && ((msg->Id & CAN_FUNCT_MASK) == CAN_MSG_TYPE_BIOS)) {
		printf("BIOS received msg!\n");
	} else if (bios->can_callback) {
		printf("Calling app callback.\n");
		bios->can_callback(msg);
	} else {
		printf("BIOS received application msg but no handler installed!\n");
	}
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
	
	if (pgm_read_word(0) != 0xffff) {
		printf("Starting application.\n");
		app_reset();
	}
	
	printf("No application found.\n");
	
	printf("\n------------------------------------------------------------\n");
	printf(  "   CAN Test: Periodic Transmission\n");
	printf(  "------------------------------------------------------------\n");
	
	printf("CanInit...\n");
	if (Can_Init() != CAN_OK) {
		printf("FAILED!\n");
	}
	else {
		printf("OK!\n");
	}
	
	uint32_t timeStamp = 0;
	
	Can_Message_t txMsg;
	//The databytes are just what happens to be in the memory. They are never set.
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1; 
#define NODENUMBER 2
#if NODENUMBER == 1
	txMsg.Id = 16;
	txMsg.DataLength = 2;
#elif NODENUMBER == 2
	txMsg.Id = 32;
	txMsg.DataLength = 5;
#else
# error NODENUMBER not set!
#endif
	
	/* main loop */
	while (1) {
		
		/* send CAN message and check for CAN errors once every second */
		if ((timebase_get() - timeStamp) >= 1000) {
			timeStamp = timebase_get();
			/* send txMsg */
			txMsg.Id++;
			Can_Send(&txMsg);
		}
	}

	return 0;
}
