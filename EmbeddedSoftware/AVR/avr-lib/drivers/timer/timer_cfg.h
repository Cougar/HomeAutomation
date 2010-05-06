#ifndef TIMER_CFG_H_
#define TIMER_CFG_H_

#include <config.h>

//#define TIMER_DEBUG_CONFIG

/*-----------------------------------------------------------------------------
 * Configuration
 *---------------------------------------------------------------------------*/
#ifndef TIMER_NUM_TIMERS
#	error TIMER_NUM_TIMERS is not defined! Edit config.inc.
#endif

#ifndef TIMER_USE_TIMER
#	error TIMER_USE_TIMER is not defined! Edit config.inc.
#endif

#ifndef TIMER_TICKFREQ
#	warning Using default TIMER_TICKFREQ value of 1000.
#	define TIMER_TICKFREQ 1000
#endif

#if defined(__AVR_ATmega8__)
#define TIMER0_MAX					255
#define TIMER0_VECTOR				TIMER0_OVF_vect
#define TIMER0_RELOAD(_t_)			TCNT0 = ( 256 - _t_ );
#define TIMER0_INIT() 				TIFR=(1<<TOV0);TIMSK|=(1<<TOIE0);
#define TIMER0_SET_PRESCALER_1()	TCCR0=1;
#define TIMER0_SET_PRESCALER_8()	TCCR0=2;
#define TIMER0_SET_PRESCALER_64()	TCCR0=3;
#define TIMER0_SET_PRESCALER_256()	TCCR0=4;
#define TIMER0_SET_PRESCALER_1024()	TCCR0=5;
#define TIMER1_MAX					65535
#define TIMER1_VECTOR				TIMER1_COMPA_vect
#define TIMER1_COMPARE_REG			OCR1A
#define TIMER1_INIT() 				TCCR1A=0;TIFR=(1<<OCF1A);TIMSK|=(1<<OCIE1A);
#define TIMER1_SET_PRESCALER_1()	TCCR1B=(1<<WGM12)|1;
#define TIMER1_SET_PRESCALER_8()	TCCR1B=(1<<WGM12)|2;
#define TIMER1_SET_PRESCALER_64()	TCCR1B=(1<<WGM12)|3;
#define TIMER1_SET_PRESCALER_256()	TCCR1B=(1<<WGM12)|4;
#define TIMER1_SET_PRESCALER_1024()	TCCR1B=(1<<WGM12)|5;
#define TIMER2_MAX					255
#define TIMER2_VECTOR				TIMER2_COMP_vect
#define TIMER2_COMPARE_REG			OCR2
#define TIMER2_INIT() 				TIFR=(1<<OCF2);TIMSK|=(1<<OCIE2);
#define TIMER2_SET_PRESCALER_1()	TCCR2=(1<<WGM21)|1;
#define TIMER2_SET_PRESCALER_8()	TCCR2=(1<<WGM21)|2;
#define TIMER2_SET_PRESCALER_64()	TCCR2=(1<<WGM21)|4;
#define TIMER2_SET_PRESCALER_256()	TCCR2=(1<<WGM21)|6;
#define TIMER2_SET_PRESCALER_1024()	TCCR2=(1<<WGM21)|7;

#elif defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__)
#define TIMER0_MAX					255
#define TIMER0_VECTOR				TIMER0_COMPA_vect
#define TIMER0_COMPARE_REG			OCR0A
#define TIMER0_INIT() 				TCCR0A=(1<<WGM01);TIFR0=(1<<OCF0A);TIMSK0|=(1<<OCIE0A);
#define TIMER0_SET_PRESCALER_1()	TCCR0B=1;
#define TIMER0_SET_PRESCALER_8()	TCCR0B=2;
#define TIMER0_SET_PRESCALER_64()	TCCR0B=3;
#define TIMER0_SET_PRESCALER_256()	TCCR0B=4;
#define TIMER0_SET_PRESCALER_1024()	TCCR0B=5;
#define TIMER1_MAX					65535
#define TIMER1_VECTOR				TIMER1_COMPA_vect
#define TIMER1_COMPARE_REG			OCR1A
#define TIMER1_INIT() 				TCCR1A=0;TIFR1=(1<<OCF1A);TIMSK1|=(1<<OCIE1A);
#define TIMER1_SET_PRESCALER_1()	TCCR1B=(1<<WGM12)|1;
#define TIMER1_SET_PRESCALER_8()	TCCR1B=(1<<WGM12)|2;
#define TIMER1_SET_PRESCALER_64()	TCCR1B=(1<<WGM12)|3;
#define TIMER1_SET_PRESCALER_256()	TCCR1B=(1<<WGM12)|4;
#define TIMER1_SET_PRESCALER_1024()	TCCR1B=(1<<WGM12)|5;
#define TIMER2_MAX					255
#define TIMER2_VECTOR				TIMER2_COMPA_vect
#define TIMER2_COMPARE_REG			OCR2A
#define TIMER2_INIT() 				TCCR2A=(1<<WGM21);TIFR2=(1<<OCF2A);TIMSK2|=(1<<OCIE2A);
#define TIMER2_SET_PRESCALER_1()	TCCR2B=1;
#define TIMER2_SET_PRESCALER_8()	TCCR2B=2;
#define TIMER2_SET_PRESCALER_64()	TCCR2B=4;
#define TIMER2_SET_PRESCALER_256()	TCCR2B=6;
#define TIMER2_SET_PRESCALER_1024()	TCCR2B=7;

#else
#	error AVR not supported!
#endif

#if TIMER_USE_TIMER == 0
#define TIMER_MAX					TIMER0_MAX
#define TIMER_VECTOR				TIMER0_VECTOR
#if defined(TIMER0_RELOAD)
#	define TIMER_RELOAD				TIMER0_RELOAD
#endif
#if defined(TIMER0_COMPARE_REG)
#	define TIMER_COMPARE_REG			TIMER0_COMPARE_REG
#endif
#define TIMER_INIT					TIMER0_INIT
#define TIMER_SET_PRESCALER_1		TIMER0_SET_PRESCALER_1
#define TIMER_SET_PRESCALER_8		TIMER0_SET_PRESCALER_8
#define TIMER_SET_PRESCALER_64		TIMER0_SET_PRESCALER_64
#define TIMER_SET_PRESCALER_256		TIMER0_SET_PRESCALER_256
#define TIMER_SET_PRESCALER_1024	TIMER0_SET_PRESCALER_1024
#if defined(TIMER_DEBUG_CONFIG)
#	warning Using Timer 0. 
#endif

#elif TIMER_USE_TIMER == 1
#define TIMER_MAX					TIMER1_MAX
#define TIMER_VECTOR				TIMER1_VECTOR
#if defined(TIMER1_RELOAD)
#	define TIMER_RELOAD				TIMER1_RELOAD
#endif
#if defined(TIMER1_COMPARE_REG)
#	define TIMER_COMPARE_REG			TIMER1_COMPARE_REG
#endif
#define TIMER_INIT					TIMER1_INIT
#define TIMER_SET_PRESCALER_1		TIMER1_SET_PRESCALER_1
#define TIMER_SET_PRESCALER_8		TIMER1_SET_PRESCALER_8
#define TIMER_SET_PRESCALER_64		TIMER1_SET_PRESCALER_64
#define TIMER_SET_PRESCALER_256		TIMER1_SET_PRESCALER_256
#define TIMER_SET_PRESCALER_1024	TIMER1_SET_PRESCALER_1024
#if defined(TIMER_DEBUG_CONFIG)
#	warning Using Timer 1. 
#endif

#elif TIMER_USE_TIMER == 2
#define TIMER_MAX					TIMER2_MAX
#define TIMER_VECTOR				TIMER2_VECTOR
#if defined(TIMER2_RELOAD)
#	define TIMER_RELOAD				TIMER2_RELOAD
#endif
#if defined(TIMER2_COMPARE_REG)
#	define TIMER_COMPARE_REG			TIMER2_COMPARE_REG
#endif
#define TIMER_INIT					TIMER2_INIT
#define TIMER_SET_PRESCALER_1		TIMER2_SET_PRESCALER_1
#define TIMER_SET_PRESCALER_8		TIMER2_SET_PRESCALER_8
#define TIMER_SET_PRESCALER_64		TIMER2_SET_PRESCALER_64
#define TIMER_SET_PRESCALER_256		TIMER2_SET_PRESCALER_256
#define TIMER_SET_PRESCALER_1024	TIMER2_SET_PRESCALER_1024
#if defined(TIMER_DEBUG_CONFIG)
#	warning Using Timer 2. 
#endif

#else
#	error Invalid timer selected!
#endif


/* Find a suitable prescaler setting. Prescalers 1, 8, 64, 256 and 1024 are
 * available in all timers, so consider only these for simplicity. */
#define TIMER_PRESCALE 1
#define TIMER_COUNTS_PER_TICK (F_CPU/TIMER_PRESCALE/TIMER_TICKFREQ-1)
#if defined(TIMER_DEBUG_CONFIG)
#	warning Trying prescaler 1... 
#endif

#if TIMER_COUNTS_PER_TICK > TIMER_MAX
#undef TIMER_PRESCALE
#undef TIMER_COUNTS_PER_TICK
#define TIMER_PRESCALE 8
#define TIMER_COUNTS_PER_TICK (F_CPU/TIMER_PRESCALE/TIMER_TICKFREQ-1)
#if defined(TIMER_DEBUG_CONFIG)
#	warning Trying prescaler 8... 
#endif
#endif

#if TIMER_COUNTS_PER_TICK > TIMER_MAX
#undef TIMER_PRESCALE
#undef TIMER_COUNTS_PER_TICK
#define TIMER_PRESCALE 64
#define TIMER_COUNTS_PER_TICK (F_CPU/TIMER_PRESCALE/TIMER_TICKFREQ-1)
#if defined(TIMER_DEBUG_CONFIG)
#	warning Trying prescaler 64... 
#endif
#endif

#if TIMER_COUNTS_PER_TICK > TIMER_MAX
#undef TIMER_PRESCALE
#undef TIMER_COUNTS_PER_TICK
#define TIMER_PRESCALE 256
#define TIMER_COUNTS_PER_TICK (F_CPU/TIMER_PRESCALE/TIMER_TICKFREQ-1)
#if defined(TIMER_DEBUG_CONFIG)
#	warning Trying prescaler 256... 
#endif
#endif

#if TIMER_COUNTS_PER_TICK > TIMER_MAX
#undef TIMER_PRESCALE
#undef TIMER_COUNTS_PER_TICK
#define TIMER_PRESCALE 1024
#define TIMER_COUNTS_PER_TICK (F_CPU/TIMER_PRESCALE/TIMER_TICKFREQ-1)
#if defined(TIMER_DEBUG_CONFIG)
#	warning Trying prescaler 1024... 
#endif
#endif

#if TIMER_COUNTS_PER_TICK > TIMER_MAX
#	error Invalid frequency selection!
#endif

#if defined(TIMER_DEBUG_CONFIG)
#	warning Prescaler ok! 
#endif

#if TIMER_PRESCALE == 1
#	define TIMER_SET_PRESCALER	TIMER_SET_PRESCALER_1
#	if defined(TIMER_DEBUG_CONFIG)
#		warning Using prescaler 1. 
#	endif
#elif TIMER_PRESCALE == 8
#	define TIMER_SET_PRESCALER	TIMER_SET_PRESCALER_8
#	if defined(TIMER_DEBUG_CONFIG)
#		warning Using prescaler 8. 
#	endif
#elif TIMER_PRESCALE == 64
#	define TIMER_SET_PRESCALER	TIMER_SET_PRESCALER_64
#	if defined(TIMER_DEBUG_CONFIG)
#		warning Using prescaler 64. 
#	endif
#elif TIMER_PRESCALE == 256
#	define TIMER_SET_PRESCALER	TIMER_SET_PRESCALER_256
#	if defined(TIMER_DEBUG_CONFIG)
#		warning Using prescaler 256. 
#	endif
#elif TIMER_PRESCALE == 1024
#	define TIMER_SET_PRESCALER	TIMER_SET_PRESCALER_1024
#	if defined(TIMER_DEBUG_CONFIG)
#		warning Using prescaler 1024. 
#	endif
#endif

#endif /*TIMER_CFG_H_*/
