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
#include <config.h>
#include <drivers/ir/receiver/irreceiver.h>
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

//TODO: ändra prescaler till /8
//TODO: skriv doxygen-header som för de andra funktionerna
void initTimer(void) {
	//sätt upp timer
	#if defined(__AVR_ATmega8__)
	TCCR1B = (0<<CS12) | (1<<CS11) | (0<<CS10); // prescaler: 1
	TIFR  |= (1<<TOV1);  			// clear overflow flag.
	TCNT1 = 0;
	#endif
	#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__)
	TCCR1B = (0<<CS12) | (1<<CS11) | (0<<CS10); // prescaler: 1
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

/**
 * Receive RC5 data, http://www.sbprojects.com/knowledge/ir/rc5.htm
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 * @return
 * 		IR_OK if data received successfully, one of several errormessages if not
 */
uint8_t receiveRC5(uint8_t *address, uint8_t *command) {
	//printf("startbit of RC5!\n");
	uint8_t i;
	uint16_t rawbits=0;
	//read second startbit, togglebit, the 5 addressbits and the 6 commandbits
	for (i=0; i<13; i++) {
		//set up a quarter of a bit period time
		setTimerVal(0xFFFF-(IR_RC5_BIT/4));
		//and wait for time to elapse
		while(!isTimerOvfl());
		//reset timer ovfl flag
		setTimerVal(TIM_OVFL_RELOAD_VAL);
		if (IRPIN & (1<<IRBIT)) {
			//save a one
			rawbits |= (1<<(12-i));
			//wait for negative slope, while checking timer overflow
			while ((IRPIN & (1<<IRBIT))) {
				//if timeout (if timer-ovflow-flaggan is set)
				if (isTimerOvfl() == 1) return IR_TIME_OVFL;
			}
		} else {
			//save a zero
			
			//wait for positiv slope
			while (!(IRPIN & (1<<IRBIT))) {
				//if timeout (if timer-ovflow-flaggan is set)
				if (isTimerOvfl() == 1) return IR_TIME_OVFL;
			}
		}
		//set up a half a bit period time
		setTimerVal(0xFFFF-(IR_RC5_BIT/2));
		//and wait for time to elapse
		while(!isTimerOvfl());
	}
	
	*command = ((uint8_t)rawbits&0x3f);
	*address = ((uint8_t)(rawbits>>6)&0x7f);	
	
	//wait for positiv slope to ensure that ir sequence is over
	setTimerVal(TIM_OVFL_RELOAD_VAL);
	while (!(IRPIN & (1<<IRBIT))) {
		//if timeout (if timer-ovflow-flaggan is set)
		if (isTimerOvfl() == 1) return IR_TIME_OVFL;
	}
	
	//support RC5-extended by putting the inversion of startbit 2 as the 7th commandbit
	//hmm, not really supported because of startbit length in function IrReceive_CheckIR
	*command |= (~(*address) & 0x40);
	*address &= 0x1f;		//remove togglebit and second startbit from address
	//printf("address %i\n", *address);
	//printf("command %i\n", *command);
	
	return IR_OK;
}

/**
 * Receive SIRC data, http://www.sbprojects.com/knowledge/ir/sirc.htm
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 * @return
 * 		IR_OK if data received successfully, one of several errormessages if not
 */
uint8_t receiveSIRC(uint8_t *address, uint8_t *command) {
	//printf("startbit of SIRC!\n");
	
	uint8_t i;
	uint16_t rawbits=0;
	//read the 12 bits
	for (i=0; i<12; i++) {
		setTimerVal(TIM_OVFL_RELOAD_VAL);
		//wait for negative slope, while checking timer overflow
		while ((IRPIN & (1<<IRBIT))) {
			//om timeout (om timer-ovflow-flaggan sätt)
			if (isTimerOvfl() == 1) return IR_TIME_OVFL;
		}
		
		setTimerVal(TIM_OVFL_RELOAD_VAL);
		//wait for positiv slope, while checking timer overflow
		while (!(IRPIN & (1<<IRBIT))) {
			//if timeout (if timer-ovflow-flaggan is set)
			if (isTimerOvfl() == 1) return IR_TIME_OVFL;
		}
		
		uint16_t timerVal = getTimerVal();
		
		if (timerVal > IR_SIRC_ZERO_LOW+TIM_OVFL_RELOAD_VAL && timerVal < IR_SIRC_ZERO_HIGH+TIM_OVFL_RELOAD_VAL) {
			//write a zero
		} else if (timerVal > IR_SIRC_ONE_LOW+TIM_OVFL_RELOAD_VAL && timerVal < IR_SIRC_ONE_HIGH+TIM_OVFL_RELOAD_VAL) {
			//write a one
			rawbits |= (1<<i);
		} else {
			return IR_NOT_CORRECT_DATA;
		}
	}
	
	*command = ((uint8_t)rawbits&0x7f);
	*address = ((uint8_t)(rawbits>>7)&0x1f);
	
	// check/wait for positiv to ensure that ir sequence is over
	setTimerVal(TIM_OVFL_RELOAD_VAL);
	while (!(IRPIN & (1<<IRBIT))) {
		//if timeout (if timer-ovflow-flaggan is set)
		if (isTimerOvfl() == 1) return IR_TIME_OVFL;
	}
	
	return IR_OK;
}

//TODO: skriv doxygen-header som för de andra funktionerna
/**
 * 
 * 
 * 
 * 
 */
void IrReceive_Init(void) {
	IRDDR &= ~(1<<IRBIT);
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
 * @param timeout
 * 		Pointer to store the timout, in ms, between two ir-pulsetrains
 * @return
 * 		IR_OK if data received successfully, one of several errormessages if not
 */
uint8_t IrReceive_CheckIR(uint8_t *proto, uint8_t *address, uint8_t *command, uint16_t *timeout) {
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
		*timeout = IR_RC5_REPETITION;
		return receiveRC5(&*address, &*command);
	} else if ((timerVal < IR_SIRC_ST_BIT+500) && (timerVal > IR_SIRC_ST_BIT-500)) {
		*proto = IR_PROTO_SIRC;
		*timeout = IR_SIRC_REPETITION;
		return receiveSIRC(&*address, &*command);
	}
	
	
	return IR_NO_PROTOCOL;
}

//varför är denna funktion så komplex?
//TODO: skriv doxygen-header som för de andra funktionerna
uint8_t IrReceive_CheckIdle(void) {
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
