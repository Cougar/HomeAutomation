#include <avr/interrupt.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <flash.h>

static uint16_t flash_prev_addr;
static uint8_t flash_buffer_dirty = 0;
static uint8_t* flash_pagebuf;

void flash_flush_buffer() {
	uint8_t sreg;
	uint16_t word;
	uint8_t i;
	uint16_t* buf = (uint16_t*)flash_pagebuf; // Access buffer on a word basis.
	
	if (!flash_buffer_dirty) return; // Nothing to flush.
	
	// Copy the sram buffer to the temporary page buffer.
	for (i = 0; i < SPM_PAGESIZE/2; i++) {
		word = buf[i];
		boot_page_fill(i*2, word); // Absolute addressing is not neccessary.
	}
	
	// Disable interrupts.
	sreg = SREG;
	cli();
	
	eeprom_busy_wait(); // Make sure any eeprom access has finished.
	
	boot_page_erase(flash_prev_addr);
	boot_spm_busy_wait();  // Wait until the memory is erased.
	
	boot_page_write(flash_prev_addr); // Store buffer in flash page.
	boot_spm_busy_wait();  // Wait until the memory is written.

	flash_buffer_dirty = 0;

	// Reenable RWW-section again so we can return to BIOS after flashing.
	boot_rww_enable ();
	
	// Re-enable interrupts (if they were ever enabled).
	SREG = sreg;
}

void flash_load_buffer(uint16_t addr) {
	uint8_t i;
	uint8_t byte;
	uint16_t page = (addr / SPM_PAGESIZE) * SPM_PAGESIZE;
	
	// Load current flash page contents into sram page buffer to implement
	// flash read-modify-writes with word granularity.
	for (i = 0; i < SPM_PAGESIZE; i++) {
		byte = pgm_read_byte(page + i);
		flash_pagebuf[i] = byte;
	}
}

void flash_write_word(uint16_t addr, uint16_t word) {
	uint16_t* buf = (uint16_t*)flash_pagebuf; // Access buffer on a word basis.
	
	// Check if page address has changed since last write
	if ((addr / SPM_PAGESIZE) != (flash_prev_addr / SPM_PAGESIZE)) {
		flash_flush_buffer(); // Flash buffer contents to previous page
		flash_load_buffer(addr); // Read new page contents into buffer
	}
	
	buf[(addr % SPM_PAGESIZE)/2] = word;
	flash_prev_addr = addr;
	flash_buffer_dirty = 1;
}     

void flash_init(uint8_t* buf) {
	// 0xffff is never in any page that can be written on devices with <= 64K flash,
	// so the first flash_write_word will always trigger a buffer load.
	flash_prev_addr = 0xffff;
	flash_pagebuf = buf;
	flash_buffer_dirty = 0;
}

extern char __flash_code_start; // Start of .flash_code from ld-script

void flash_copy_data(uint16_t src, uint16_t dst, uint16_t len) {
	uint16_t data;

	cli(); // From this point we're on our own
	
	len = (len | (SPM_PAGESIZE - 1)) + 3; // Round len up to a whole page + 2
	
	while (len -= 2) {
		if (dst >= (uint16_t)&__flash_code_start) break;

		wdt_reset();

		data = pgm_read_word(src); // Read source word
		boot_page_fill(dst, data); // Write word to destination
		
		if ((dst + 2) % SPM_PAGESIZE == 0) { // If we just wrote the last word of a page
			boot_page_erase(dst);
			boot_spm_busy_wait(); // Wait until the memory is erased.
			boot_page_write(dst); // Store buffer in flash page.
			boot_spm_busy_wait(); // Wait until the memory is written.
			boot_rww_enable();
		}
		 
		src += 2;
		dst += 2;		
	}

	while (1); // Nothing more we can do, hopefully there is a watchdog active
}
