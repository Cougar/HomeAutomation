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



//<FUNCT>
#define FUNCT_BOOTLOADER 0x0

//<FUNCC> configuration packet
#define FUNCC_BOOT_INIT 0b0000000001 //Start programmring and/or resseting user program to go into bootloader mode.
#define FUNCC_BOOT_ADDR 0b0000000010  //Change programming address.
#define FUNCC_BOOT_DONE 0b0000000101  //End programming, start user program.

//<FUNCC> program packet
#define FUNCC_BOOT_PGM 0b0000000011 //To indicate programming data packet.

//<FUNCC> ack packet
#define FUNCC_BOOT_ACK 0b0000000100 //Ack packet


#define DEFAULT_ID 0x78 	// Default ID for bootloader/node when no id is set.


#define NODE_ID_EE  0x00   	// EEPROM first index where to look for ID and NID. 
				// NODE_HAS_ID_EE = NODE_HAS_ID if has correct ID, otherwise use DEFAULT_ID
				// NODE_HAS_ID_EE + 1 = ID[1]
				// NODE_HAS_ID_EE + 2 = ID[0]
				// NODE_HAS_ID_EE + 3 = NID
 
#define NODE_HAS_ID 0x2A   	// Value which indicate valid ID and NID.
