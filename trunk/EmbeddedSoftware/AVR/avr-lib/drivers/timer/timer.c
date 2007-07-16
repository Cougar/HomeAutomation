/**
 * Timer software module. Utilizes a hardware timer to provide a number of
 * software timers to the application. Each timer can be set to either one-shot
 * or free-running mode.
 * 
 * @author	Andreas Fritiofson
 * 
 * @date	2007-07-13
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <drivers/timer/timer.h>
#include "timer_cfg.h"


/*-----------------------------------------------------------------------------
 * Globals
 *---------------------------------------------------------------------------*/
volatile uint32_t ticks;

struct {
	uint16_t timeout;
	uint16_t reload;
	unsigned char expired;
	timerCallback_t callback;
} timers[TIMER_NUM_TIMERS];

/*-----------------------------------------------------------------------------
 * Interrupt Service Routines
 *---------------------------------------------------------------------------*/
ISR(TIMER_VECTOR)
{
	uint8_t t;
#if defined(TIMER_RELOAD)
	/* The hw timer does not support auto-reload so set the count manually. */
	TIMER_RELOAD(TIMER_COUNTS_PER_TICK);
#endif

	/* Increase the global tick count. */
	ticks++;
	
	/* Decrease the timeout of all active timers and check if any have
	 * expired. */
	for (t = 0; t < TIMER_NUM_TIMERS; t++)
	{
		if ((timers[t].timeout > 0) && (--timers[t].timeout == 0))
		{
			timers[t].expired = 1;
			timers[t].timeout = timers[t].reload;
			/* Call the callback, if any. */
			if (timers[t].callback) timers[t].callback(t);
		}
	}
}


/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

void Timer_Init()
{
	/* Do basic setup of the timer hw, including interrupts. */
	TIMER_INIT();
#if defined(TIMER_RELOAD)
	/* The hw timer does not support auto-reload so set the count manually. */
	TIMER_RELOAD(TIMER_COUNTS_PER_TICK);
#endif
#if defined(TIMER_COMPARE_REG)
	/* The hw timer has a compare match module so use it for auto-reload. */
	TIMER_COMPARE_REG = TIMER_COUNTS_PER_TICK;
#endif
	/* Finalize hw setup and start the timer by setting its prescaler. */
	TIMER_SET_PRESCALER();
}

/*---------------------------------------------------------------------------*/

uint32_t Timer_GetTicks()
{
	uint32_t res;
	uint8_t sreg = SREG;
	cli();
	res = ticks;
	SREG = sreg;
	return res;
}

/*---------------------------------------------------------------------------*/

void Timer_SetTimeout(uint8_t timer, uint16_t timeout, uint8_t type, timerCallback_t callback)
{
	if (timer<TIMER_NUM_TIMERS)
	{
		uint8_t sreg = SREG;
		cli();
		timers[timer].expired = 0;
		timers[timer].timeout = timeout;
		if (type == TimerTypeFreeRunning)
			timers[timer].reload = timeout;
		else
			timers[timer].reload = 0;
		timers[timer].callback = callback;
		SREG = sreg;
	}
}

/*---------------------------------------------------------------------------*/

uint8_t Timer_Expired(uint8_t timer)
{
	if ((timer<TIMER_NUM_TIMERS) && (timers[timer].expired))
	{
		timers[timer].expired = 0;
		return 1;
	}
	return 0;
}
