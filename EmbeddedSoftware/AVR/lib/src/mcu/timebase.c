/**
 * Timebase software module. Utilizes timer0 to provide a 32bit timebase with
 * millisecond-granularity to the application.
 * 
 * @target	ATmega8
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

#include <timebase.h>


/*-----------------------------------------------------------------------------
 * Globals
 *---------------------------------------------------------------------------*/
volatile uint32_t gMilliSecTick;


/*-----------------------------------------------------------------------------
 * Interrupt Service Routines
 *---------------------------------------------------------------------------*/
ISR(SIG_OVERFLOW0) {
	TCNT0 = TIMEBASE_RELOAD;
	gMilliSecTick++;
}


/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/
/**
 * Initializes the timebase. The hardware timer interrupt source will be
 * enabled, but global interrupts need to be enabled by the application.
 */
void Timebase_Init() {
	TCCR0 = (1<<CS01) | (1<<CS00); // prescaler: 64
	TCNT0 = TIMEBASE_RELOAD; // set initial reload-value
	TIFR  |= (1<<TOV0);  // clear overflow int.
	TIMSK |= (1<<TOIE0); // enable overflow-interrupt
}

/**
 * Reads the current time.
 * 
 * @return
 * 		The current time in milliseconds (32bit value).
 */
uint32_t Timebase_CurrentTime(void) {
	uint8_t sreg;
	uint16_t res;
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
	uint16_t res;
	sreg=SREG;
	cli();
	res = (uint16_t)(gMilliSecTick-t0);
	SREG=sreg;
	return res;
}
