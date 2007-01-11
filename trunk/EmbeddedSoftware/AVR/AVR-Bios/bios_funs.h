#ifndef BIOS_FUNS_H_
#define BIOS_FUNS_H_

#define BIOS_BUILDTYPE_BIOS
#define BIOS_VERSION 0x1002

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

#endif /*BIOS_FUNS_H_*/
