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



typedef enum {OPEN=2,MIDDLE=3,CLOSE=4} BLINDS_DIR;

#define TMR1_VAL 60536
// 0.5ms
// Tosc/4 = 10Mhz = 0,0000001
// .05ms = 5000
// 65536-5000 = 

void blindsInit(void);

void blindsTurn(BLINDS_DIR dir,WORD steps);
void blindsStop(void);

void blindsISR(void);

#endif
