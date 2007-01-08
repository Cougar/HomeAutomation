#ifndef BIOS_FUNS_H_
#define BIOS_FUNS_H_

#define BIOS_BUILDTYPE_BIOS
#define BIOS_VERSION 0x1002

#define UART_RX_BUFFER_SIZE 16
#define UART_TX_BUFFER_SIZE 16

#include "bios.h"

//---------------------------------------------------------------------------
// Function prototypes for the exported interface

static void reset(void);
static void bios_putchar(char c);
static char bios_getchar(void);
static long timebase_get(void);

static bios_t bios_functions = {
	BIOS_VERSION,
	reset,
	bios_putchar,
	bios_getchar,
	timebase_get
};

//---------------------------------------------------------------------------
// Function prototypes for flash modifying functions
// TODO: These must be located above boot loader section limit even if bios
// size grows larger than that.

void flash_flush_buffer(void);
void flash_load_buffer(uint16_t addr);
void flash_write_word(uint16_t addr, uint16_t word);
void flash_init(void);

#endif /*BIOS_FUNS_H_*/
