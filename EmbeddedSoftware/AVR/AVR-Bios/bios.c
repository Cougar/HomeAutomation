#include "bios_funs.h"
#include "avr_cfg.h"
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stdio.h>
#include "app_vectors.h"

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
	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = c;
};

char bios_getchar(void) {
	loop_until_bit_is_set(UCSRA, RXC);
	return UDR;
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

static uint16_t flash_prev_addr;

void flash_flush_buffer() {
	uint8_t sreg;
	
	// Disable interrupts.
	
	sreg = SREG;
	cli();
	
	eeprom_busy_wait();
	
	boot_page_erase(flash_prev_addr);
	boot_spm_busy_wait();      // Wait until the memory is erased.
	
	boot_page_write(flash_prev_addr);     // Store buffer in flash page.
	boot_spm_busy_wait();       // Wait until the memory is written.

	// Reenable RWW-section again. We need this if we want to jump back
	// to the application after bootloading.

	boot_rww_enable ();

	// Re-enable interrupts (if they were ever enabled).

	SREG = sreg;
}

void flash_load_buffer(uint16_t addr) {
	//TODO: Load current flash page contents into temporary buffer to implement
	// flash read-modify-writes with word granularity. (If possible, the data
	// sheet is a little fuzzy about this.)
} 

void flash_write_word(uint16_t addr, uint16_t word) {
	
	if ((addr / SPM_PAGESIZE) != (flash_prev_addr / SPM_PAGESIZE)) {
		flash_flush_buffer();
		flash_load_buffer(addr);
	}
	
	boot_page_fill(addr, word);
	flash_prev_addr = addr;
}     

void flash_init() {
	flash_prev_addr = 0xffff;
}

static int console_getchar(FILE *stream) {
	return bios->can_receive();
}

static int console_putchar(char c, FILE *stream) {
	bios->can_send(c);
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
	UCSRB = _BV(RXEN)|_BV(TXEN);
	UBRRL = (F_CPU / 16 / F_BAUD) - 1;
	stdout = &bios_stdio;
	stdin = &bios_stdio;
	
	// Initialize a suitable hardware timer to create a 1KHz interrupt.
	TIMEBASE_INIT();
	
	// Enable watchdog timer to protect against an application locking the
	// node by leaving interrupts disabled. 
	wdt_enable(WDTO_500MS);
	
	// Move interrupt vectors to start of bootloader section and enable interrupts
	GICR = _BV(IVCE);
	GICR = _BV(IVSEL);
	sei();
	
	printf("AVR BIOS\n");
	
	if (pgm_read_word(0) != 0xffff) {
		printf("Starting application.\n");
		app_reset();
	}
	
	printf("No application found.\n");
	return 0;
}
