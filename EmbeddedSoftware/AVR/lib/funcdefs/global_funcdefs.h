/****************************************************************
 *
 *   Defines functs and funccs and other global definitions.
 *
 *********************************************************************
 * 
 * 
 * By: johboh, edited by eqlazer 
 * 
 ********************************************************************/



//<FUNCT>
#define FUNCT_BOOTLOADER 0x00
#define FUNCT_SENSORS 0x01

//<FUNCC> configuration packet
#define FUNCC_BOOT_INIT   0b0000000001 //Start programmring and/or resseting user program to go into bootloader mode.
#define FUNCC_BOOT_ADDR   0b0000000010  //Change programming address.
#define FUNCC_BOOT_SET_ID 0b0000000110  //Change ID and NID.
#define FUNCC_BOOT_DONE   0b0000000101  //End programming, start user program.

#define FUNCC_BOOT_HEARTBEAT 0b0000001000 // Heartbeat

//<FUNCC> program packet
#define FUNCC_BOOT_PGM 0b0000000011 //To indicate programming data packet.

//<FUNCC> ack packet 10 bits, 0x3FF
#define FUNCC_BOOT_ACK 0b0000000100 //Ack packet


//<FUNCC> 10 bits, 0x3FF

/*#define FUNCC_SENSORS_IR 						0x001

#define FUNCC_SENSORS_TEMPERATURE_INSIDE 		0x010
#define FUNCC_SENSORS_TEMPERATURE_OUTSIDE 		0x011
#define FUNCC_SENSORS_TEMPERATURE_FREEZER 		0x012
#define FUNCC_SENSORS_TEMPERATURE_REFRIGERATOR 	0x013

#define FUNCC_SENSORS_DOORS_FREEZER 			0x020
#define FUNCC_SENSORS_DOORS_REFRIGERATOR 		0x021
*/

#define DEFAULT_ID 0x78 	// Default ID for bootloader/node when no id is set.
#define DEFAULT_NID 0x00	// Default ID for bootloader/node when no nid is set.


#define NODE_ID_EE  0x00   	// EEPROM first index where to look for ID and NID. 
				// NODE_ID_EE = NODE_HAS_ID if has correct ID, otherwise use DEFAULT_ID and DEFAULT_NID
				// NODE_ID_EE + 1 = ID[1]
				// NODE_ID_EE + 2 = ID[0]
				// NODE_ID_EE + 3 = NID
 
#define NODE_HAS_ID 0x2A   	// Value which indicate valid ID and NID.


// Definitions for BOOT packets. Which index in data where things are.
#define BOOT_DATA_ADDRL_INDEX 0
#define BOOT_DATA_ADDRH_INDEX 1
#define BOOT_DATA_ADDRU_INDEX 2
#define BOOT_DATA_RID_LOW_INDEX 4
#define BOOT_DATA_RID_HIGH_INDEX 5
#define BOOT_DATA_ERR_INDEX 4
#define BOOT_DATA_NEW_ID_LOW_INDEX 0 
#define BOOT_DATA_NEW_ID_HIGH_INDEX 1 
#define BOOT_DATA_NEW_NID_INDEX 2
#define BOOT_DATA_HEARTBEAT_INDEX 0

//<ERR>
// Value in BOOT_DATA_ERR_INDEX
#define ACK_ERR_NO_ERROR 0x00 //= no error
#define ACK_ERR_ERROR 0x01 //= error

//<HEART>
#define HEARTBEAT_BOOT_STARTUP 0x00
#define HEARTBEAT_USER_STARTUP 0x01
#define HEARTBEAT_ALIVE	0x02

 
