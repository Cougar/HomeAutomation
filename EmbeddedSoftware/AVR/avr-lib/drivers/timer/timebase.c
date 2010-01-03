/**
 * Timebase software module. Utilizes timer0 to provide a 32bit timebase with
 * millisecond-granularity to the application.
 * 
 * @author	Martin Thomas
 * @author	Jimmy Myhrman
 * 
 * @date	2006-11-19
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <config.h>
#include <drivers/timer/timebase.h>


/*-----------------------------------------------------------------------------
 * Globals
 *---------------------------------------------------------------------------*/
volatile uint32_t gMilliSecTick;


/*-----------------------------------------------------------------------------
 * Interrupt Service Routines
 *---------------------------------------------------------------------------*/
#if defined(TIMEBASE_NEW_IMPLEMENTATION)

#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
#if defined(TIMER2)
ISR(TIMER2_COMPA_vect) {
#else //!defined(TIMER2)
ISR(TIMER0_COMPA_vect) {
#endif //defined(TIMER2)
#else //!defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
ISR(TIMER2_COMP_vect) {
#endif //defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
	gMilliSecTick++;
}

#else //!defined(TIMEBASE_NEW_IMPLEMENTATION)

#if defined(TIMER2)
ISR(SIG_OVERFLOW2) {
	TCNT2 = TIMEBASE_RELOAD;
	gMilliSecTick++;
}
#else
ISR(SIG_OVERFLOW0) {
	TCNT0 = TIMEBASE_RELOAD;
	gMilliSecTick++;
}
#endif

#endif //defined(TIMEBASE_NEW_IMPLEMENTATION)


/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

/**
 * Initializes the timebase. The hardware timer interrupt source will be
 * enabled, but global interrupts need to be enabled by the application.
 */
void Timebase_Init() {
#if defined(TIMEBASE_NEW_IMPLEMENTATION)

#if defined(TIMER2)
	#if defined(__AVR_ATmega8__)
	TCCR2 = (1<<WGM21); // CTC mode
	#if TIMEBASE_PRESCALE == 64
	TCCR2 = (1<<CS22);	// prescaler: 64
	#elif TIMEBASE_PRESCALE == 256
	TCCR2 = (1<<CS22) | (1<<CS21); // prescaler: 256
	#endif
	OCR2 = TIMEBASE_HITS_PER_1MS; // set timer period
	TIFR  |= (1<<OCF2);  // clear ouput compare match flag
	TIMSK |= (1<<OCIE2); // enable output compare match interrupt
	#endif
	
	#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
	TCCR2A = (1<<WGM21); // CTC mode
	#if TIMEBASE_PRESCALE == 64
	TCCR2B = (1<<CS22);	// prescaler: 64
	#elif TIMEBASE_PRESCALE == 256
	TCCR2B = (1<<CS22)|(1<<CS21);				// prescaler: 256
	#endif
	OCR2A = TIMEBASE_HITS_PER_1MS; // set timer period
	TIFR2  |= (1<<OCF2A);  // clear ouput compare match flag
	TIMSK2 |= (1<<OCIE2A); // enable output compare match interrupt
	#endif
#else /* Use timer0 */
	#if defined(__AVR_ATmega8__)
	#error TIMER0 not supported in ATmega8 with TIMEBASE_NEW_IMPLEMENTATION set!
	#endif
	
	#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
	TCCR0A = (1<<WGM01); // CTC mode
	#if TIMEBASE_PRESCALE == 64
	TCCR0B = (1<<CS01) | (1<<CS00);	// prescaler: 64
	#elif TIMEBASE_PRESCALE == 256
	TCCR0B = (1<<CS02);				// prescaler: 256
	#endif
	OCR0A = TIMEBASE_HITS_PER_1MS; // set timer period
	TIFR0  |= (1<<OCF0A);  // clear ouput compare match flag
	TIMSK0 |= (1<<OCIE0A); // enable output compare match interrupt
    #endif
#endif


#else //!defined(TIMEBASE_NEW_IMPLEMENTATION)

#if defined(TIMER2)
	#if defined(__AVR_ATmega8__)
	#if TIMEBASE_PRESCALE == 64
	TCCR2 = (1<<CS22);	// prescaler: 64
	#elif TIMEBASE_PRESCALE == 256
	TCCR2 = (1<<CS22) | (1<<CS21); // prescaler: 256
	#endif
	TCNT2 = TIMEBASE_RELOAD; // set initial reload-value
	TIFR  |= (1<<TOV2);  // clear overflow int.
	TIMSK |= (1<<TOIE2); // enable overflow-interrupt
	#endif
	
	#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
	#if TIMEBASE_PRESCALE == 64
	TCCR2B = (1<<CS22);	// prescaler: 64
	#elif TIMEBASE_PRESCALE == 256
	TCCR2B = (1<<CS22)|(1<<CS21);				// prescaler: 256
	#endif
	TCNT2 = TIMEBASE_RELOAD; // set initial reload-value
	TIFR2  |= (1<<TOV2);  // clear overflow int.
	TIMSK2 |= (1<<TOIE2); // enable overflow-interrupt
	#endif
#else /* Use timer0 */
	#if defined(__AVR_ATmega8__)
	#if TIMEBASE_PRESCALE == 64
	TCCR0 = (1<<CS01) | (1<<CS00);	// prescaler: 64
	#elif TIMEBASE_PRESCALE == 256
	TCCR0 = (1<<CS02); 				// prescaler: 256
	#endif
	TCNT0 = TIMEBASE_RELOAD; // set initial reload-value
	TIFR  |= (1<<TOV0);  // clear overflow int.
	TIMSK |= (1<<TOIE0); // enable overflow-interrupt
	#endif
	
	#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
	#if TIMEBASE_PRESCALE == 64
	TCCR0B = (1<<CS01) | (1<<CS00);	// prescaler: 64
	#elif TIMEBASE_PRESCALE == 256
	TCCR0B = (1<<CS02);				// prescaler: 256
	#endif
	TCNT0 = TIMEBASE_RELOAD; // set initial reload-value
	TIFR0  |= (1<<TOV0);  // clear overflow int.
	TIMSK0 |= (1<<TOIE0); // enable overflow-interrupt
    #endif
#endif

#endif //defined(TIMEBASE_NEW_IMPLEMENTATION)
}


/**
 * Reads the current time.
 * 
 * @return
 * 		The current time in milliseconds (32bit value).
 */
uint32_t Timebase_CurrentTime(void) {
	uint8_t sreg;
	uint32_t res;
	sreg=SREG;
	cli();
	res = gMilliSecTick;
	SREG=sreg;
	return res;
}


/**
 * Checks how much time has passed since a given timestamp.
 * 
 * @param t0
 * 		The timestamp.
 * @return
 * 		Number of milliseconds that have passed since t0.
 */ 
uint32_t Timebase_PassedTimeMillis(uint32_t t0) {
	uint8_t sreg;
	uint32_t res;
	sreg=SREG;
	cli();
	res = (uint32_t)(gMilliSecTick-t0);
	SREG=sreg;
	return res;
}
