/*********************************************************************
 *
 *                  Ticker module header file
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/

#ifndef TICK_H
#define TICK_H

#include <Compiler.h>
#include <stackTasks.h>
#include <typedefs.h>

typedef unsigned long TICK;

#define TICK_1S    (TICK)100
#define TICK_100MS  (TICK)10

#define TICK_COUNTER 62411
#define TICK_PRESCALE 0b100 //32

void tickInit(void);
TICK tickGet(void);
void tickUpdate(void);

#endif
