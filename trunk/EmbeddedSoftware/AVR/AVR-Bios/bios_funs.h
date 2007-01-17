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
static Can_Return_t can_send(Can_Message_t* msg);
//static Can_Return_t can_receive(Can_Message_t *msg);

static bios_t bios_functions = {
	BIOS_VERSION,
	reset,
	can_send,
	0,
	bios_putchar,
	bios_getchar,
	timebase_get
};

//---------------------------------------------------------------------------
// Special CAN masks

// completely bogus
#define CAN_FUNCT_MASK	0x00000F
#define CAN_MSG_TYPE_BIOS	0x0
#define CAN_MSG_TYPE_APP	0x1
#define CAN_MSG_TYPE_BAJS	0x4
#define CAN_MSG_TYPE_LOPRIO	0xf

#endif /*BIOS_FUNS_H_*/
