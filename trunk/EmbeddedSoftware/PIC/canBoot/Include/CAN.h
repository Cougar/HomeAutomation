/*********************************************************************
 *
 *                  CAN module header file
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

void canInit(void);
BOOL canSendMessage(CAN_PACKET cp);

#endif //CAN.h
