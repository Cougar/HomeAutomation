/******************************************************************************
 * Copyright (C) 2005 Martin THOMAS, Kaiserslautern, Germany
 * <eversmith@heizung-thomas.de>
 * http://www.siwawi.arubi.uni-kl.de/avr_projects
 *****************************************************************************
 *
 * File    : timebase.c
 * Version : 0.9
 * 
 * Summary : AVR Timebase using Hardware-Timer 0
 *
 *****************************************************************************/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>

#include "timebase.h"

SIGNAL(SIG_OVERFLOW0)
{
	gMilliSecTick++;
	TCNT0 = TIMEBASE_RELOAD;
}

void timebase_init(void)
{
	TCCR0 = (1<<CS01) | (1<<CS00); // prescaler: 64
	TCNT0 = TIMEBASE_RELOAD; // set initial reload-value
	TIFR  |= (1<<TOV0);  // clear overflow int.
	TIMSK |= (1<<TOIE0); // enable overflow-interrupt
}

uint16_t timebase_actTime(void)
{
	uint8_t sreg;
	uint16_t res;
	
	sreg=SREG;
	cli();

	res = gMilliSecTick;
	
	SREG=sreg;
	
	return res;
}
	
uint16_t timebase_passedTimeMS(uint16_t t0)
{
	uint8_t sreg;
	uint16_t res;
	
	sreg=SREG;
	cli();

	res = (uint16_t)(gMilliSecTick-t0);
	
	SREG=sreg;
	
	return res;
}


