/*********************************************************************
 *
 *                  Stack specific
 *
 *********************************************************************
 * FileName:        stackTasks.h
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Johan Böhlin     21-10-06 	Original        (Rev 1.0)
 ********************************************************************/

#ifndef stackTasks_H
#define stackTasks_H

#define USE_CAN
#define USE_UART

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
