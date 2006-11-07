/*********************************************************************
 *
 *                  CAN.c header file
 *
 *********************************************************************
 * FileName:        CAN.h
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Johan Böhlin     21-10-06 	Original        (Rev 1.0)
 ********************************************************************/

#ifndef CAN_H
#define CAN_H

#include <CANdefs.h>
#include <compiler.h>
#include <typedefs.h>


typedef struct _CAN_MESSAGE
{
	
	DWORD ident;
	BOOL extended;
	BOOL remote_request;
	BYTE data_length;
	BYTE data[8];
} CAN_MESSAGE;


typedef enum {PRIO_LOWEST=0,PRIO_LOW=1,PRIO_HIGH=2,PRIO_HIGEST=3} CAN_PRIORITY;


void canInit(void);
void canISR(void);
void canParse(CAN_MESSAGE cm);
BOOL canSendMessage(CAN_MESSAGE cm, CAN_PRIORITY prio);

#endif //CAN.h
