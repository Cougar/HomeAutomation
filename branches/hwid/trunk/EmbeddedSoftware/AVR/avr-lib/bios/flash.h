#ifndef FLASH_H_
#define FLASH_H_

#define BOOTLOADER __attribute__ ((section (".bootloader")))
#define FLASH_CODE __attribute__ ((section (".flash_code")))

//---------------------------------------------------------------------------
// Function prototypes for flash modifying functions

// Only flash_flush_buffer uses the spm instruction and has to be in the
// .bootloader section, but there is room for the other functions which
// would otherwise use space in .text.
void flash_init(uint8_t* buf) BOOTLOADER;
void flash_write_word(uint16_t addr, uint16_t word) BOOTLOADER;
void flash_flush_buffer(void) BOOTLOADER;
// Tell gcc that flash_copy_data never returns, so it can be further optimized.
void flash_copy_data(uint16_t src, uint16_t dst, uint16_t len) __attribute__ ((noreturn)) FLASH_CODE;

#endif /*FLASH_H_*/
