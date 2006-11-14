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
	pgsWATING_START=0,
	pgsSEND_ACK,
	pgsWAIT_FIRST_PGM_PACKET,
	pgsWAIT_PGM_PACKET,
	pgsWRITE_PROGRAM,
	pgsDONE,
} PROGRAM_STATE;



#endif //TYPEDEFS_H
