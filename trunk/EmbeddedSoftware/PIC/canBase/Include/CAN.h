/*********************************************************************
 *
 *   CAN module header file with prototypes and message typedef
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/

#ifndef CAN_H
#define CAN_H

#include <CANdefs.h>
#include <compiler.h>
#include <typedefs.h>


typedef struct _CAN_MESSAGE
{
	BYTE funct; //4
	WORD funcc; //10
	BYTE nid; //6
	WORD sid; //9
	BYTE data_length;//5
	BYTE data[8];
} CAN_MESSAGE;


typedef enum {PRIO_LOWEST=0,PRIO_LOW=1,PRIO_HIGH=2,PRIO_HIGEST=3} CAN_PRIORITY;


void canInit(void);
void canISR(void);
void canParse(CAN_MESSAGE cm);
BOOL canSendMessage(CAN_MESSAGE cm, CAN_PRIORITY prio);

#endif //CAN.h
