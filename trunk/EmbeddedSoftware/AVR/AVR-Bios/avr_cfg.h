#ifndef AVR_CFG_H_
#define AVR_CFG_H_

#if defined(__AVR_ATmega8__)

#define TIMEBASE_COUNTER TCNT0
#define TIMEBASE_RELOAD (F_CPU / 1000 / 64)
#define TIMEBASE_INIT()            \
	TCNT0 = TIMEBASE_RELOAD;       \
	TIFR  |= (1<<TOV0);            \
	TIMSK |= (1<<TOIE0);           \
	TCCR0 = (1<<CS01) | (1<<CS00);
#define TIMEBASE_VECTOR TIMER0_OVF_vect

#elif defined(__AVR_ATmega88__)

#define TIMEBASE_COUNTER TCNT0
#define TIMEBASE_RELOAD (F_CPU / 1000 / 64)
#if (TIMEBASE_RELOAD > 255)
# error System frequency too high for timebase prescaler 64.
#endif
#define TIMEBASE_INIT()            \
	TCNT0 = TIMEBASE_RELOAD;       \
	TIFR0  |= (1<<TOV0);           \
	TIMSK0 |= (1<<TOIE0);          \
	TCCR0B = (1<<CS01) | (1<<CS00);
#define TIMEBASE_VECTOR TIMER0_OVF_vect

#else
#error AVR specifics not defined!
#endif

#endif /*AVR_CFG_H_*/
