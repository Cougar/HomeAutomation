/*********************************************************************
 *
 *   Defines functs and funccs.
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