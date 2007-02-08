#ifndef FLASH_H_
#define FLASH_H_

#define BOOTLOADER __attribute__ ((section (".bootloader")))

//#define FLASH_LOAD_BUFFER
//#define FLASH_COPY_DATA
#define FLASH_COPY_DATA_NEW

//---------------------------------------------------------------------------
// Function prototypes for flash modifying functions

void flash_init(void) BOOTLOADER;
void flash_write_word(uint16_t addr, uint16_t word) BOOTLOADER;
void flash_flush_buffer(void) BOOTLOADER;
#ifdef FLASH_COPY_DATA
void flash_copy_data(uint16_t src, uint16_t dst, uint16_t len) BOOTLOADER;
#endif
#ifdef FLASH_COPY_DATA_NEW
void flash_copy_data(uint16_t src, uint16_t dst, uint16_t len) __attribute__ ((noreturn)) __attribute__ ((section (".flash_code")));
#endif

#endif /*FLASH_H_*/
