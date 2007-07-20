#ifndef BIOS_H_
#define BIOS_H_

#define F_CPU 8000000UL
#define F_BAUD  31250UL
#define NODE_ID 1

#include "can.h"

//---------------------------------------------------------------------------
// Public function structure definition and declaration

typedef struct bios{
	unsigned version;
	void (*reset)(void);
	Can_Return_t (*can_send)(Can_Message_t* msg);
	void (*can_callback)(Can_Message_t *msg);
	void (*debug_putchar)(char c);
	char (*debug_getchar)(void);
	unsigned long (*timebase_get)(void);
} bios_t;
 
static bios_t bios_functions __attribute__ ((section (".bios_interface")));

static bios_t *bios __attribute__ ((used)) = (bios_t*)&bios_functions;

#endif /*BIOS_H_*/
