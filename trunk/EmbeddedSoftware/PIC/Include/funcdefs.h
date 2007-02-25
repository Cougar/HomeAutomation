/*********************************************************************
 *
 *   Defines functs and funccs and other global definitions.
 *
 *********************************************************************
 * FileName:        	$HeadURL$
 * Last changed:	$LastChangedDate$
 * By:			$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/

#include <typedefs.h>

#define DEFAULT_ID 0x78 	// Default ID for bootloader/node when no id is set.
#define NODE_ID_EE  0x00   	// EEPROM first index where to look for ID and NID. 
				// NODE_ID_EE = NODE_HAS_ID if has correct ID, otherwise use DEFAULT_ID and DEFAULT_NID
				// NODE_ID_EE + 1 = ID
#define NODE_HAS_ID 0x2A   	// Value which indicate valid ID and NID.



typedef enum _PACKET_TYPE {ptBOOT=0x0,ptPGM=0x1} PACKET_TYPE;
typedef enum _BOOT_TYPE {btADDR=0x0,btPGM=0x1,btDONE=0x2,btACK=0x4,btNACK=0x5,btCRC=0x6,btCHANGEID=0x7} BOOT_TYPE;
typedef enum _PGM_CLASS {pcCTRL=0x0,pcSENSOR=0x1,pcACTUATOR=0x2} PGM_CLASS;

typedef struct _CAN_BOOT_PACKET
{
	BOOT_TYPE type; //3
	BYTE    offset; //8
	BYTE       rid; //8
} CAN_BOOT_PACKET;


typedef struct _CAN_PGM_PACKET
{
	PGM_CLASS class; //4
	WORD         id; //15
} CAN_PGM_PACKET;

typedef struct _CAN_PACKET
{
	PACKET_TYPE     type;
	CAN_PGM_PACKET   pgm;
	CAN_BOOT_PACKET boot;
	BYTE             sid; //8
	BYTE          length;
	BYTE         data[8];
} CAN_PACKET;



// When CAN_BOOT_PACKET.type==btADDR
// data[2] = ADDRU
// data[1] = ADDRH
// data[0] = ADDRL

// When CAN_BOOT_PACKET.type==btCRC
// data[4] = CRCH
// data[3] = CRCL
// data[2] = ADDRU
// data[1] = ADDRH
// data[0] = ADDRL

// When CAN_BOOT_PACKET.type==btCHANGEID
// data[0] = new id

typedef enum _PGM_CTRL_TYPE {pctBOOTBOOT=0x0,pctAPPBOOT=0x1,pctHEARTBEAT=0x2} PGM_CTRL_TYPE;
// When CAN_PGM_PACKET.type==pcCTRL
// 	When CAN_PGM_PACKET.id==pctBOOTBOOT
//		data[0] = startup reason

typedef enum _PGM_SENSOR_TYPE 	{	
					pstIR=0x0,
					pstTEMP_INSIDE=0x1, pstTEMP_OUTSIDE=0x2, pstTEMP_FREEZER=0x3, pstTEMP_REFRIGERATOR=0x4,
					pstDOOR_FREEZER=0x5, pstDOOR_REFRIGERATOR=0x6,
					pstBLIND_TR=0x7
				} PGM_SENSOR_TYPE;
// When CAN_PGM_PACKET.type==pcSENSOR
//	When CAN_PGM_PACKET.id==pstIR
//		data[0]...
//	When CAN_PGM_PACKET.id==pstTEMP_*
//		data[1] = integer value -127 to 127
//		data[0] = decimal value 0-9
//	When CAN_PGM_PACKET.id==pstBLIND_*
//		data[0] = Precent value 0-100

typedef enum _PGM_ACTUATOR_TYPE {	
					patBLIND_TR=0x7
				} PGM_ACTUATOR_TYPE;
// When CAN_PGM_PACKET.type==pcACTUATOR
//	When CAN_PGM_PACKET.id==pstBLIND_*
//		data[0] = Precent value 0-100






 
