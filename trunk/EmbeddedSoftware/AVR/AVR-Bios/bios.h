#ifndef BIOS_H_
#define BIOS_H_

#define F_CPU 16000000UL
#define F_BAUD  31250UL

#include <stdio.h>

//---------------------------------------------------------------------------
// Public function structure definition and declaration

typedef struct bios{
	int (*put_char)(char c, FILE *stream);
	int (*get_char)(FILE *stream);
	void (*reset)(void);
	unsigned (*version)(void);
} bios_t;
 
static bios_t bios_functions __attribute__ ((section (".bios_interface")));

//#ifndef BIOS_BUILDTYPE_BIOS
static bios_t *bios = (bios_t*)&bios_functions;
//#endif

#endif /*BIOS_H_*/
