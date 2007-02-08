#include <avr/interrupt.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
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

	flash_buffer_dirty = 0;

	// Reenable RWW-section again. We need this if we want to jump back
	// to the application after bootloading.

	boot_rww_enable ();
	// Re-enable interrupts (if they were ever enabled).

	SREG = sreg;
}

#ifdef FLASH_LOAD_BUFFER
void flash_load_buffer(uint16_t addr) BOOTLOADER;
void flash_load_buffer(uint16_t addr) {
	//TODO: Load current flash page contents into temporary buffer to implement
	// flash read-modify-writes with word granularity. (If possible, the data
	// sheet is a little fuzzy about this.)
	// Update: It seems it isn't possible to write any location in the temporary
	// buffer more than once without destroying the buffer. To implement this
	// functionality, a sram based buffer is probably neccessary. Problem is, 
	// how should it be allocated? A global buffer uses valuable .bss space.
	// All functions should probably take a pointer to a buffer allocated by
	// main code.
	uint8_t i;
	uint16_t data;
	uint16_t page = (addr / SPM_PAGESIZE) * SPM_PAGESIZE;
	
	for (i = 0; i < SPM_PAGESIZE; i += 2) {
		data = pgm_read_word(page + i);
		boot_page_fill(page + i, data);
	}
}
#endif

void flash_write_word(uint16_t addr, uint16_t word) {
	
	if ((addr / SPM_PAGESIZE) != (flash_prev_addr / SPM_PAGESIZE)) {
		flash_flush_buffer();
#ifdef FLASH_LOAD_BUFFER
		flash_load_buffer(addr);
#endif
	}
	
	boot_page_fill(addr, word);
	flash_prev_addr = addr;
	flash_buffer_dirty = 1;
}     

void flash_init() {
	flash_prev_addr = 0xffff;
	flash_buffer_dirty = 0;
}

#ifdef FLASH_COPY_DATA
void flash_copy_data(uint16_t src, uint16_t dst, uint16_t len) {
	uint16_t i;
	flash_init();
	cli(); // From this point we're on our own
	for (i = 0; i < len; i+=2) {
		flash_write_word(dst + i, pgm_read_word(src + i));
		wdt_reset();
	}
	flash_flush_buffer();
	while (1); // Nothing more we can do, hopefully there is a watchdog active
}
#endif

#ifdef FLASH_COPY_DATA_NEW
extern uint16_t __flash_code_start;

void flash_copy_data(uint16_t src, uint16_t dst, uint16_t len) {
	uint16_t offset;
	uint16_t data;

	cli(); // From this point we're on our own
	
	for (offset = 0; offset < len; offset += 2) {
		if (dst + offset >= (uint16_t)&__flash_code_start) break;
		data = pgm_read_word(src + offset);
		boot_page_fill(dst + offset, data);
		if ((dst + offset + 2) % SPM_PAGESIZE == 0) {
			boot_page_erase(dst + offset);
			//boot_spm_busy_wait();      // Wait until the memory is erased.
			
			boot_page_write(dst + offset);     // Store buffer in flash page.
			//boot_spm_busy_wait();       // Wait until the memory is written.
		}
			
		wdt_reset();
	}
	while (1); // Nothing more we can do, hopefully there is a watchdog active
}
#endif
