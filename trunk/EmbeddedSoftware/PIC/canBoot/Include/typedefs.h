/*********************************************************************
 *
 *                  Type definitions
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/

#ifndef TYPEDEFS_H
#define TYPEDEFS_H

typedef enum _BOOL { FALSE = 0, TRUE } BOOL;
typedef unsigned char		BYTE;				// 8-bit
typedef unsigned short int	WORD;				// 16-bit
typedef unsigned long		DWORD;				// 32-bit


typedef enum _PROGRAM_STATE
{
	pgsWAITING_START=0,
	pgsSEND_ACK,
	pgsWAIT_FIRST_DATA,
	pgsWAIT_DATA_OR_CRC,
	pgsWRITE_DATA,
	pgsDONE,
} PROGRAM_STATE;

#endif //TYPEDEFS_H
