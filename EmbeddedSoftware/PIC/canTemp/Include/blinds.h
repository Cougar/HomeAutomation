/*********************************************************************
 *
 *   Header file for BLINDS routines.
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/
#ifndef BLINDS_H
#define BLINDS_H

#include <stackTasks.h>
#include <typedefs.h>
#include <delays.h>
#include <compiler.h>


#define ANGLE_MIN = -50
#define ANGLE_MAX = 60


#define IO_TIMEOUT 700
#define TMR1_VAL 60536
// 0.05ms
// Fosc/4 = 10Mhz = 0,0000001
// 0.1ms = 500

void blindsInit(void);

void blindsTurn(BYTE precent);

void blindsISR(void);

BYTE blindsGetPrecent(void);

#endif
