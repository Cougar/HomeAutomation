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


typedef struct _CAN_PROTO_MESSAGE
{
	BYTE funct; //4
	WORD funcc; //10
	BYTE nid; //6
	WORD sid; //9
	BYTE data_length;//5
	BYTE data[8];
} CAN_PROTO_MESSAGE;


#define MY_ID 0x78 
#define MY_NID 0x00

#define ADDRL_INDEX 0
#define ADDRH_INDEX 1
#define ADDRU_INDEX 2
#define RID_LOW_INDEX 4
#define RID_HIGH_INDEX 5
#define ERR_INDEX 4

//<ERR> 8 bitar feltyp
#define ERR_NO_ERROR 0x00 //= inget fel
#define ERR_ERROR 0x01 //= fel


typedef enum {PRIO_LOWEST=0,PRIO_LOW=1,PRIO_HIGH=2,PRIO_HIGEST=3} CAN_PRIORITY;


void canInit(void);
BOOL canSendMessage(CAN_PROTO_MESSAGE cm, CAN_PRIORITY prio);

#endif //CAN.h
