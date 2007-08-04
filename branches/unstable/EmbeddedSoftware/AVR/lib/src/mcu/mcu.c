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
#include <mcu.h>


/*-----------------------------------------------------------------------------
 * Prerequisites
 *---------------------------------------------------------------------------*/
#ifndef MCU
	#error MCU Not defined in mcu_cfg.h
#endif


/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

/**
 * Initialize MCU. If the MCU is mega88, the internal clock frequency will be
 * set to 8MHz. Interrupts must not be enabled when calling this function!
 */
void Mcu_Init() {
	#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) 
	// set the clock speed to 8MHz
	// set the clock prescaler. First write CLKPCE to enable setting of clock the
	// next four instructions.
	CLKPR=(1<<CLKPCE);
	CLKPR=0; // 8 MHZ
	#endif
}

/**
 * Enable interrupts.
 */
void Mcu_EnableIRQ() {
	#if MCU == ATMEGA8
		sei();
	#endif
}

/**
 * Disable interrupts.
 */
void Mcu_DisableIRQ() {
	#if MCU == ATMEGA8
		cli();
	#endif
}
