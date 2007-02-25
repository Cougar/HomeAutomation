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
#include <funcdefs.h>

typedef enum {PRIO_LOWEST=0,PRIO_LOW=1,PRIO_HIGH=2,PRIO_HIGEST=3} CAN_PRIORITY;


void canInit(void);
void canISR(void);
void canParse(CAN_PACKET cp);
BOOL canSendMessage(CAN_PACKET cp, CAN_PRIORITY prio);

#endif //CAN.h
