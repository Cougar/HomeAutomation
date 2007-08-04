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
#include <pgmtypes.h>

#define DEFAULT_ID 0x78 	// Default ID for bootloader/node when no id is set.
#define NODE_ID_EE  0x00   	// EEPROM first index where to look for ID and NID. 
				// NODE_ID_EE = NODE_HAS_ID if has correct ID, otherwise use DEFAULT_ID and DEFAULT_NID
				// NODE_ID_EE + 1 = ID
#define NODE_HAS_ID 0x2A   	// Value which indicate valid ID and NID.


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

