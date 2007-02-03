#ifndef FLASH_H_
#define FLASH_H_

#define BOOTLOADER __attribute__ ((section (".bootloader")))

//#define FLASH_LOAD_BUFFER
#define FLASH_COPY_DATA

//---------------------------------------------------------------------------
// Function prototypes for flash modifying functions

void flash_init(void) BOOTLOADER;
void flash_write_word(uint16_t addr, uint16_t word) BOOTLOADER;
void flash_flush_buffer(void) BOOTLOADER;
#ifdef FLASH_COPY_DATA
void flash_copy_data(uint16_t src, uint16_t dst, uint16_t len) BOOTLOADER;
#endif

#endif /*FLASH_H_*/
