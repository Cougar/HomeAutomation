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

#include <stackTasks.h>

typedef unsigned long TICK;

#if !defined(TICKS_PER_SECOND)
    #error TICKS_PER_SECOND must be defined.
#endif

#if !defined(TICK_PRESCALE_VALUE)
    #error TICK_PRESCALE_VALUE must be defined.
#endif

#define TICK_PERIOD_MS          10
#define TICK_SECOND             ((TICK)TICKS_PER_SECOND)


/*
 * Only Tick.c defines TICK_INCLUDE and thus defines Seconds
 * and TickValue storage.
 */
#ifndef TICK_INCLUDE
extern TICK TickCount;
#endif

void tickInit(void);
TICK tickGet(void);
void tickUpdate(void);

#endif
