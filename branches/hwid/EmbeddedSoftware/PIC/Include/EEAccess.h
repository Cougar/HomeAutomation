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
#ifndef __EEACCESS_H
#define __EEACCESS_H

#include <typedefs.h>

BYTE EERead(WORD addr);
void EEWrite(WORD addr,BYTE data);

#endif //EEACCESS_H
