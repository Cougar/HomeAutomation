#include "bios_funs.h"
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdio.h>

//---------------------------------------------------------------------------
// Function definitions for the exported interface

int bios_putchar(char c, FILE *stream) {
	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = c;
	return 0;
};

int bios_getchar(FILE *stream) {
	loop_until_bit_is_set(UCSRA, RXC);
	return UDR;
};

void bios_reset(void) {
	bios_putchar('\n', 0);
};

unsigned bios_version(void) {
	return BIOS_VERSION;
};

//---------------------------------------------------------------------------
// Function definitions for the private functions
    
void program_page (uint32_t page, uint8_t *buf)
{
	uint16_t i;
	uint8_t sreg;

	// Disable interrupts.

	sreg = SREG;
	cli();
    
	eeprom_busy_wait ();

	boot_page_erase (page);
	boot_spm_busy_wait ();      // Wait until the memory is erased.

	for (i=0; i<SPM_PAGESIZE; i+=2)
	{
		// Set up little-endian word.
		
		uint16_t w = *buf++;
		w += (*buf++) << 8;
		
		boot_page_fill (page + i, w);
	}

	boot_page_write (page);     // Store buffer in flash page.
	boot_spm_busy_wait();       // Wait until the memory is written.

	// Reenable RWW-section again. We need this if we want to jump back
	// to the application after bootloading.

	boot_rww_enable ();

	// Re-enable interrupts (if they were ever enabled).

	SREG = sreg;
}

static FILE bios_stdio = FDEV_SETUP_STREAM(bios_putchar, bios_getchar,
                                         _FDEV_SETUP_RW);
    
int main() {
	void (*app_reset)(void) = 0;
	// Setup USART for debug channel
	UCSRB = _BV(RXEN)|_BV(TXEN);
	UBRRL = (F_CPU / 16 / F_BAUD) - 1;
	stdout = &bios_stdio;
	stdin = &bios_stdio;
	
	bios->reset();
	printf("AVR BIOS\n");
	app_reset();
	
	return 0;
}
