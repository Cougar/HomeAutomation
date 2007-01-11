#include <avr/interrupt.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include "flash.h"

static uint16_t flash_prev_addr;
static uint8_t flash_buffer_dirty = 0;

void flash_flush_buffer() {
	uint8_t sreg;
	
	if (!flash_buffer_dirty) return; // Nothing to flush
	
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
	uint8_t i;
	uint16_t data;
	uint16_t page = (addr / SPM_PAGESIZE) * SPM_PAGESIZE;
	
	for (i = 0; i < SPM_PAGESIZE; i += 2) {
		data = pgm_read_word(page + i);
		boot_page_fill(page + i, data);
	}
} 

void flash_write_word(uint16_t addr, uint16_t word) {
	
	if ((addr / SPM_PAGESIZE) != (flash_prev_addr / SPM_PAGESIZE)) {
		flash_flush_buffer();
		flash_load_buffer(addr);
	}
	
	boot_page_fill(addr, word);
	flash_prev_addr = addr;
	flash_buffer_dirty = 1;
}     

void flash_init() {
	flash_prev_addr = 0xffff;
	flash_buffer_dirty = 0;
}
