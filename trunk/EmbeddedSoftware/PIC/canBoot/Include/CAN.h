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


typedef struct _CAN_PROTO_MESSAGE
{
	BYTE funct; //4
	WORD funcc; //10
	BYTE nid; //6
	WORD sid; //9
	BYTE data_length;//5
	BOOL remote_request; //1
	BYTE data[8];
} CAN_PROTO_MESSAGE;


//<FUNCT>
#define FUNCT_BOOTLOADER 0x0

//<FUNCC> configuration packet
#define FUNCC_BOOT_INIT 0b0000000001 //för att initiera en programmering.
#define FUNCC_BOOT_ADDR 0b0000000010  //för att ändra adress.
#define FUNCC_BOOT_DONE 0b0000000101  //för att avsluta en programmering.

//<FUNCC> program packet
#define FUNCC_BOOT_PGM 0b0000000011 //för att indikera att detta är programdata som ska skrivas.

//<FUNCC> ack packet
#define FUNCC_BOOT_ACK 0b0000000100 //för att indikera att detta är en ACK.

#define MY_ID 0x78 
#define MY_NID 0x00

#define ADDRL_INDEX 0
#define ADDRH_INDEX 1
#define ADDRU_INDEX 2
#define RID_INDEX 4
#define ERR_INDEX 4

//<ERR> 8 bitar feltyp
#define ERR_NO_ERROR 0x00 //= inget fel
#define ERR_ERROR 0x01 //= fel


typedef enum {PRIO_LOWEST=0,PRIO_LOW=1,PRIO_HIGH=2,PRIO_HIGEST=3} CAN_PRIORITY;


void canInit(void);
BOOL canSendMessage(CAN_PROTO_MESSAGE cm, CAN_PRIORITY prio);

#endif //CAN.h
