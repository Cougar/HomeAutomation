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



/**
 * This is a test-bootloader with ihex parser, since it takes a while to program
 * a page in flash the serial port must to be slow, change to 2400baud in bios.h
 * Also, rename this file to bios.c for simplicity
 */



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

uint8_t asciitoint(uint8_t data) {
	data = data - 48;
	if (data > 9) 
		data = data - 7;	//asume A-F (uppercase)
	return data;
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
	flash_init();

	uint16_t addr;
	uint16_t prgword;
	uint8_t state = 0;
	uint8_t i = 0;
	uint8_t nibblecnt = 0;
	uint8_t hexdatacnt;
	
/*		uint8_t tmp = 48;
		tmp = 48+ hexdatacnt;
		UART_PUTCHAR(tmp);*/
	for (i=1; i<100; i++) {
		uint8_t data = bios_getchar();
		if (i == 1) {
			//Wait for Start code ':'
			if (data != ':') {
				i = 0;
			}
		}
		//Read two digits for Byte count
		if (i == 2) {
			data = asciitoint(data);
			hexdatacnt = data<<4;
		}
		if (i == 3) {
			data = asciitoint(data);
			hexdatacnt |= data;
		}
		//Read four digits for Address
		if (i == 4) {
			data = asciitoint(data);
			addr = data<<12;
		}
		if (i == 5) {
			data = asciitoint(data);
			addr |= data<<8;
		}
		if (i == 6) {
			data = asciitoint(data);
			addr |= data<<4;
		}
		if (i == 7) {
			data = asciitoint(data);
			addr |= data;
		}
		//Record type = 01, End Of File record
		if (i == 9) {
			if (data == '1') {
				i = 100;
				state = 1; 
			}
		}
		//If data but uneven number of bytes (half a word)
		if ((i>9) && (state==0) && (hexdatacnt == 1)) {
			prgword |= data;
			prgword |= 0xFF00;
			flash_write_word(addr, prgword);
			i = 0;
		}
		//If data, until byte counter = 0
		if ((i>9) && (state == 0)) {
			data = asciitoint(data);
			nibblecnt++;
			if (nibblecnt == 1) {
				prgword = data<<4;
			} else if (nibblecnt == 2) {
				prgword |= data;
			} else if (nibblecnt == 3) {
				prgword |= data<<12;
			} else if (nibblecnt == 4) {
				prgword |= data<<8;
				nibblecnt = 0;
				
				flash_write_word(addr, prgword);
				addr = addr + 2;
				hexdatacnt = hexdatacnt-2;
			}
			if (hexdatacnt == 0) {
				i = 0;
			}
		}
	}
	//flash last page
	flash_flush_buffer();

	
	if (pgm_read_word(0) != 0xffff) {
		printf("Starting application.\n");
		app_reset();
	}
	
	printf("No application found.\n");
	return 0;
}
