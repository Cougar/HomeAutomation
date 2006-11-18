/*********************************************************************
 *
 *                  Stack specific like which modules to use.
 *
 *********************************************************************
 * FileName:        $HeadURL: http://svn.arune.se/svn/HomeAutomation/trunk/EmbeddedSoftware/PIC/canBase/Include/stackTasks.h $
 * Last changed:	$LastChangedDate: 2006-11-14 10:55:38 +0100 (ti, 14 nov 2006) $
 * By:				$LastChangedBy: johboh $
 * Revision:		$Revision: 86 $
 ********************************************************************/

#ifndef stackTasks_H
#define stackTasks_H

#define USE_CAN

#define TICKS_PER_SECOND               (100)        // 10ms

#if (TICKS_PER_SECOND < 10 || TICKS_PER_SECOND > 255)
#error Invalid TICKS_PER_SECONDS specified.
#endif

/*
 * Manually select prescale value to achieve necessary tick period
 * for a given clock frequency.
 */
#define TICK_PRESCALE_VALUE             (256)



#endif
