#ifndef BIOS_FUNS_H_
#define BIOS_FUNS_H_

#define BIOS_BUILDTYPE_BIOS
#define BIOS_VERSION 0x1001

#include <avr/boot.h>
#include "bios.h"

//---------------------------------------------------------------------------
// Function prototypes for the exported interface

static int bios_putchar(char c, FILE *stream);
static int bios_getchar(FILE *stream);
static void bios_reset(void);
static unsigned bios_version(void);

static bios_t bios_functions = {
	bios_putchar,
	bios_getchar,
	bios_reset,
	bios_version
};

//---------------------------------------------------------------------------
// Function prototypes for flash modifying functions

//void program_page (uint32_t page, uint8_t *buf) BOOTLOADER_SECTION;
void program_page (uint32_t page, uint8_t *buf);

#endif /*BIOS_FUNS_H_*/
