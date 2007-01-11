#include "bios_funs.h"
#include "avr_cfg.h"
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "flash.h"
#include "vectors.h" // Must be included after sfr_defs.h

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
	
	if (pgm_read_word(0) != 0xffff) {
		printf("Starting application.\n");
		app_reset();
	}
	
	printf("No application found.\n");
	return 0;
}
