/**
 * MCU driver.
 * 
 * @date	2006-11-19
 * 
 * @author	Jimmy Myhrman
 *   
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <avr/interrupt.h>
#include <config.h>
#include <drivers/mcu/mcu.h>


/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

/**
 * Initialize MCU. If the MCU is mega88, the internal clock frequency will be
 * set to 8MHz. Interrupts must not be enabled when calling this function!
 */
void Mcu_Init() {
#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__)
	// set the clock speed to 8MHz
	// set the clock prescaler. First write CLKPCE to enable setting of clock the
	// next four instructions.
	CLKPR=(1<<CLKPCE);
	#ifndef AVR_CLOCK_PRESC
		CLKPR=0; // 8 MHZ
	#endif
	
	#if AVR_CLOCK_PRESC == 1
		CLKPR=0; // 8 MHZ
	#elif AVR_CLOCK_PRESC == 2
		CLKPR=1; // 4 MHZ
	#elif AVR_CLOCK_PRESC == 4
		CLKPR=2; // 2 MHZ
	#elif AVR_CLOCK_PRESC == 8
		CLKPR=3; // 1 MHZ
	#elif AVR_CLOCK_PRESC == 16
		CLKPR=4; // 500 kHZ
	#elif AVR_CLOCK_PRESC == 32
		CLKPR=5; // 250 kHZ
	#elif AVR_CLOCK_PRESC == 64
		CLKPR=6; // 125 kHZ
	#elif AVR_CLOCK_PRESC == 128
		CLKPR=7; // 62.5 kHZ
	#elif AVR_CLOCK_PRESC == 256
		CLKPR=8; // 31.25 kHZ
	#endif
#endif
}

/**
 * Enable interrupts.
 */
void Mcu_EnableIRQ() {
	#if MCU == atmega8
		sei();
	#endif
}

/**
 * Disable interrupts.
 */
void Mcu_DisableIRQ() {
	#if MCU == atmega8
		cli();
	#endif
}
