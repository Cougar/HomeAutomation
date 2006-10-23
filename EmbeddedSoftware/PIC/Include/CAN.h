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
	BYTE data_length;
	BYTE data[8];
} CAN_MESSAGE;




void canInit(void);
void canISR(void);
void canParse(CAN_MESSAGE cm);
BOOL canSendMessage(CAN_MESSAGE cm);

#endif //CAN.h
