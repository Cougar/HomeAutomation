#ifndef FLASH_H_
#define FLASH_H_

#define BOOTLOADER __attribute__ ((section (".bootloader")))

//---------------------------------------------------------------------------
// Function prototypes for flash modifying functions

void flash_init(void);
void flash_write_word(uint16_t addr, uint16_t word) BOOTLOADER;
void flash_flush_buffer(void) BOOTLOADER;

#endif /*FLASH_H_*/
