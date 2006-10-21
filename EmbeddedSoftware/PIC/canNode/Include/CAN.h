#ifndef CAN_H
#define CAN_H

#include "../Include/CANdefs.h"
#include "../Include/compiler.h"
#include "../Include/typedefs.h"


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
