#ifndef AVR_CFG_H_
#define AVR_CFG_H_

//#define TIMER2 /* For using Timer2 istead of Timer0 */

#if defined(__AVR_ATmega8__)

#if defined(TIMER2)
	#define TIMEBASE_COUNTER TCNT2
	#define TIMEBASE_RELOAD (F_CPU / 1000 / 64)
	#define TIMEBASE_INIT()            \
		TCNT2 = TIMEBASE_RELOAD;       \
		TIFR  |= (1<<TOV2);            \
		TIMSK |= (1<<TOIE2);           \
		TCCR0 = (1<<CS22);
	#define TIMEBASE_VECTOR TIMER2_OVF_vect
#else
	#define TIMEBASE_COUNTER TCNT0
	#define TIMEBASE_RELOAD (F_CPU / 1000 / 64)
	#define TIMEBASE_INIT()            \
		TCNT0 = TIMEBASE_RELOAD;       \
		TIFR  |= (1<<TOV0);            \
		TIMSK |= (1<<TOIE0);           \
		TCCR0 = (1<<CS01) | (1<<CS00);
	#define TIMEBASE_VECTOR TIMER0_OVF_vect
#endif

#define UART_INIT()                   \
	UCSRB = _BV(RXEN)|_BV(TXEN);      \
	UBRRL = (F_CPU / 16 / F_BAUD) - 1;
#define UART_PUTCHAR(_c_)               \
	loop_until_bit_is_set(UCSRA, UDRE); \
	UDR = (_c_);
#define UART_GETCHAR(_c_)              \
	loop_until_bit_is_set(UCSRA, RXC); \
	(_c_) = UDR;

#define IVSEL_REG GICR
#define INT1_REG GICR

#elif defined(__AVR_ATmega88__)
#if defined(TIMER2)
	/* Use timer2 */
	#define TIMEBASE_COUNTER TCNT2
	#define TIMEBASE_RELOAD (F_CPU / 1000 / 64)
	#if (TIMEBASE_RELOAD > 255)
	# error System frequency too high for timebase prescaler 64.
	#endif
	#define TIMEBASE_INIT()            \
		TCNT2 = TIMEBASE_RELOAD;       \
		TIFR2  |= (1<<TOV2);           \
		TIMSK2 |= (1<<TOIE2);          \
		TCCR2B = (1<<CS22);
	#define TIMEBASE_VECTOR TIMER2_OVF_vect
#else /* Use timer0*/
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
#endif

#define UART_INIT()                    \
	UCSR0B = _BV(RXEN0)|_BV(TXEN0);    \
	UBRR0L = (F_CPU / 16 / F_BAUD) - 1;
#define UART_PUTCHAR(_c_)                 \
	loop_until_bit_is_set(UCSR0A, UDRE0); \
	UDR0 = (_c_);
#define UART_GETCHAR(_c_)              \
	loop_until_bit_is_set(UCSR0A, RXC0); \
	(_c_) = UDR0;

#define IVSEL_REG MCUCR
#define INT1_REG EIMSK

#else
#error AVR specifics not defined!
#endif

#endif /*AVR_CFG_H_*/
