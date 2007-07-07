/**
 * IR common driver, functions to be used in receiver and transmitter
 * 
 * @date	2007-07-07
 * 
 * @author	Anders Runeson
 *   
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <drivers/ir/ircommon.h>
#include <avr/io.h>

/*-----------------------------------------------------------------------------
 * Globals
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Prerequisites
 *---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

 
//TODO: skriv doxygen-header som för de andra funktionerna
void initTimer(void) {
	//sätt upp timer
	#if defined(__AVR_ATmega8__)
	TCCR1B = (0<<CS12) | (1<<CS11) | (0<<CS10); // prescaler: 8
	TIFR  |= (1<<TOV1);  			// clear overflow flag.
	TCNT1 = 0;
	#endif
	#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__)
	TCCR1B = (0<<CS12) | (1<<CS11) | (0<<CS10); // prescaler: 8
	TIFR1  |= (1<<TOV1);  			// clear overflow flag.
	TCNT1 = 0;
	#endif
}

//TODO: skriv doxygen-header som för de andra funktionerna
uint8_t isTimerOvfl(void) {
	#if defined(__AVR_ATmega8__)
	if (TIFR  & (1<<TOV1)) return 1;
	#endif
	#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__)
	if (TIFR1  & (1<<TOV1)) return 1;
	#endif

	return 0;
}

//TODO: skriv doxygen-header som för de andra funktionerna
uint16_t getTimerVal(void) {
	#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__)
	return TCNT1;
	#endif
}

//TODO: skriv doxygen-header som för de andra funktionerna
void setTimerVal(uint16_t value) {
	#if defined(__AVR_ATmega8__)
	TCNT1 = value;
	TIFR |= (1<<TOV1);  // clear overflow flag.
	#endif
	#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__)
	TCNT1 = value;
	TIFR1 |= (1<<TOV1);  // clear overflow flag.
	#endif
}
 