#ifndef FLASH_H_
#define FLASH_H_

#define BOOTLOADER __attribute__ ((section (".bootloader")))

//---------------------------------------------------------------------------
// Function prototypes for flash modifying functions
// TODO: These must be located above boot loader section limit even if bios
// size grows larger than that.

void flash_init(void);
void flash_write_word(uint16_t addr, uint16_t word) BOOTLOADER;
void flash_flush_buffer(void) __attribute__ ((section (".bootloader")));

#endif /*FLASH_H_*/
