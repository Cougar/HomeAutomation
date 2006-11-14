/*********************************************************************
 *
 *   Functions for reading and writing to EEPROM.
 *
 *********************************************************************
 * FileName:        	$HeadURL$
 * Last changed:	$LastChangedDate$
 * By:			$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/
#include <p18cxxx.h>
#include <EEaccess.h>

BYTE EERead(WORD addr)
{
	EEADRH=(BYTE)(addr>>8);
	EEADR=(BYTE)(addr & 0xFF);
	EECON1bits.EEPGD=0;
	EECON1bits.CFGS=0;
	EECON1bits.RD=1;
	return EEDATA;	
}


void EEWrite(WORD addr,BYTE data)
{
	PIR2bits.EEIF=0;
	EEADRH=(BYTE)(addr>>8);
	EEADR=(BYTE)(addr & 0xFF);
	EEDATA=data;
	EECON1bits.EEPGD=0;
	EECON1bits.CFGS=0;
	EECON1bits.WREN=1;
	INTCONbits.GIE=0;
	EECON2=0x55;
	EECON2=0xAA;
	EECON1bits.WR=1;
	INTCONbits.GIE=1;
	while(!PIR2bits.EEIF);
	EECON1bits.WREN=0;
}
