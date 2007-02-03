#ifndef BIOS_FUNS_H_
#define BIOS_FUNS_H_

#define BIOS_BUILDTYPE_BIOS
#define BIOS_VERSION 0x1005
//#define BIOS_UART

#include "bios.h"

extern uint16_t __bios_start;

//---------------------------------------------------------------------------
// Function prototypes for the exported interface

static void reset(void);
#ifdef BIOS_UART
static void bios_putchar(char c);
static char bios_getchar(void);
#endif
static unsigned long timebase_get(void);
static Can_Return_t can_send(Can_Message_t* msg);

static bios_t bios_functions = {
	BIOS_VERSION,
	reset,
	can_send,
	0,
#ifdef BIOS_UART
	bios_putchar,
	bios_getchar,
#else
	0,
	0,
#endif
	timebase_get
};

#define CAN_ID_NMT_BIOS_START  ((CAN_NMT << CAN_SHIFT_CLASS) \
                              | (CAN_NMT_BIOS_START << CAN_SHIFT_NMT_TYPE) \
                              | (NODE_ID << CAN_SHIFT_NMT_SID))
#define CAN_ID_NMT_PGM_ACK     ((CAN_NMT << CAN_SHIFT_CLASS) \
                              | (CAN_NMT_PGM_ACK << CAN_SHIFT_NMT_TYPE) \
                              | (NODE_ID << CAN_SHIFT_NMT_SID))
#define CAN_ID_NMT_PGM_NACK    ((CAN_NMT << CAN_SHIFT_CLASS) \
                              | (CAN_NMT_PGM_NACK << CAN_SHIFT_NMT_TYPE) \
                              | (NODE_ID << CAN_SHIFT_NMT_SID))

#endif /*BIOS_FUNS_H_*/
