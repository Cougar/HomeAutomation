/**
 * IR receiver driver.
 * 
 * @date	2006-12-10
 * 
 * @author	Anders Runeson
 *   
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include "irreceiver.h"
#include <avr/io.h>

/*-----------------------------------------------------------------------------
 * Globals
 *---------------------------------------------------------------------------*/
volatile uint16_t lastProtoTimeout = 0;


/*-----------------------------------------------------------------------------
 * Prerequisites
 *---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

void initTimer(void) {
	//sätt upp timer
	#if defined(__AVR_ATmega8__)
	TCCR1B = (0<<CS11) | (1<<CS10); // prescaler: 1
	TIFR  |= (1<<TOV1);  			// clear overflow flag.
	TCNT1 = 0;
	#endif
	#if defined(__AVR_ATmega88__)
	TCCR1B = (0<<CS11) | (1<<CS10); // prescaler: 1
	TIFR1  |= (1<<TOV1);  			// clear overflow flag.
	TCNT1 = 0;
	#endif
}

int isTimerOvfl(void) {
	#if defined(__AVR_ATmega8__)
	if (TIFR  & (1<<TOV1)) return 1;
	#endif
	#if defined(__AVR_ATmega88__)
	if (TIFR1  & (1<<TOV1)) return 1;
	#endif

	return 0;
}

uint16_t getTimerVal(void) {
	#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__)
	return TCNT1;
	#endif
}

void setTimerVal(uint16_t value) {
	#if defined(__AVR_ATmega8__)
	TCNT1 = value;
	TIFR |= (1<<TOV1);  // clear overflow flag.
	#endif
	#if defined(__AVR_ATmega88__)
	TCNT1 = value;
	TIFR1 |= (1<<TOV1);  // clear overflow flag.
	#endif
}

/**
 * Receive RC5 data
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 */
int receiveRC5(uint8_t *address, uint8_t *command) {
	//printf("startbit of RC5!\n");
	uint8_t i;
	//read second startbit, togglebit and the 5 addressbits
	for (i=0; i<7; i++) {
		//set up a quarter of a bit period time
		setTimerVal(0xFFFF-(IR_RC5_BIT/4));
		//and wait for time to elapse
		while(!isTimerOvfl());
		//reset timer ovfl flag
		setTimerVal(0);
		if (IRPIN & (1<<IRBIT)) {
			//save a one
			*address |= (1<<(6-i));
			//wait for negative slope, while checking timer overflow
			while ((IRPIN & (1<<IRBIT))) {
				//om timeout (om timer-ovflow-flaggan sätt)
				if (isTimerOvfl() == 1) return IR_TIME_OVFL;
			}
		} else {
			//save a zero
			
			//wait for positiv slope
			while (!(IRPIN & (1<<IRBIT))) {
				//om timeout (om timer-ovflow-flaggan sätt)
				if (isTimerOvfl() == 1) return IR_TIME_OVFL;
			}
		}
		//set up a half a bit period time
		setTimerVal(0xFFFF-(IR_RC5_BIT/2));
		//and wait for time to elapse
		while(!isTimerOvfl());
	}
	
	//read 5 command bits
	for (i=0; i<6; i++) {
		//set up a quarter of a bit period time
		setTimerVal(0xFFFF-(IR_RC5_BIT/4));
		//and wait for time to elapse
		while(!isTimerOvfl());
		//reset timer ovfl flag
		setTimerVal(0);
		if (IRPIN & (1<<IRBIT)) {
			//save a one
			*command |= (1<<(5-i));
			//wait for negative slope, while checking timer overflow
			while ((IRPIN & (1<<IRBIT))) {
				//om timeout (om timer-ovflow-flaggan sätt).
				if (isTimerOvfl() == 1) return IR_TIME_OVFL;
			}
		} else {
			//save a zero
			
			//wait for positiv slope
			while (!(IRPIN & (1<<IRBIT))) {
				//om timeout (om timer-ovflow-flaggan sätt)
				if (isTimerOvfl() == 1) return IR_TIME_OVFL;
			}
		}
		//set up a half a bit period time
		setTimerVal(0xFFFF-(IR_RC5_BIT/2));
		//and wait for time to elapse
		while(!isTimerOvfl());
	}
	
	//wait for positiv slope to ensure that ir sequence is over
	setTimerVal(0);
	while (!(IRPIN & (1<<IRBIT))) {
		//om timeout (om timer-ovflow-flaggan sätt)
		if (isTimerOvfl() == 1) return IR_TIME_OVFL;
	}
	
	//support RC5-extended by putting the inversion of startbit 2 as the 7th commandbit
	*command |= (~(*address) & 0x40);
	*address &= 0x1f;
	//printf("address %i\n", *address);
	//printf("command %i\n", *command);
	
	return IR_OK;
}

uint16_t getLastProtoTimeout(void) {
	return lastProtoTimeout;
}

/**
 * Checks if ir-receiver is outputting data, if so then receive it 
 * 
 * @param proto
 * 		Pointer to store the protocol of the received data
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 * @return
 * 		IR_OK if data received successfully, one of several errormessages if not 
 */
int checkIr(uint8_t *proto, uint8_t *address, uint8_t *command) {
	if (IRPIN & (1<<IRBIT)) return IR_NO_DATA;		//om irmodulen ger en etta så återgå
	
	//nu lägger irmodulen ut en nolla, "startbiten" alltså
	
	uint16_t timerVal;
	
	*address = 0;
	*command = 0;
	
	//Läs in längden på startbiten
	initTimer();
	while (!(IRPIN & (1<<IRBIT))) {		//vänta på att irmodulen lägger ut en etta
		//om timeout (om timer-ovflow-flaggan sätt)
		if (isTimerOvfl() == 1) return IR_TIME_OVFL;
	}
	timerVal = getTimerVal();
	
	/*
	 * Anropa olika funktioner beroende på längden
	 * för vissa protokoll behöver längden på första biten skickas med till funktionen?
	 * t.ex. sharp?
	 * */
	
	if ((timerVal < IR_RC5_ST_BIT+500) && (timerVal > IR_RC5_ST_BIT-500)) {
		*proto = IR_PROTO_RC5;
		return receiveRC5(&*address, &*command);
	} //else if ...
	
	
	return IR_NO_PROTOCOL;
}


int checkIrIdle(void) {
	if (IRPIN & (1<<IRBIT)) return IR_NO_DATA;		//om irmodulen ger en etta så återgå
	
	//nu lägger irmodulen ut en nolla, "startbiten" alltså
	
	uint16_t timerVal;
	
	//Läs in längden på startbiten
	initTimer();
	while (!(IRPIN & (1<<IRBIT))) {		//vänta på att irmodulen lägger ut en etta
		//om timeout (om timer-ovflow-flaggan sätt)
		if (isTimerOvfl() == 1) return IR_TIME_OVFL;
	}
	timerVal = getTimerVal();
	
	if ((timerVal < IR_MAX_REPEATE_PULSE_WIDTH) && (timerVal > IR_MIN_REPEATE_PULSE_WIDTH)) {
		return IR_OK;
	} //else if ...
	
	
	return IR_NO_PROTOCOL;
}