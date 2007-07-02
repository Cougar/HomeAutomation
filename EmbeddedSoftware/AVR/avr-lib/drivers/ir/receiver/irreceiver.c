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
static uint16_t times[MAX_NR_TIMES];			//stores 
static uint8_t timesCounter=0;					//counts the items in Times, always less then MAX_NR_TIMES

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

/**
 * Get an ir-time from the stored array, this can be used
 * when a proper protocol is not found.
 * 
 * @param index
 * 		Which time to get
 * @return
 * 		The 16bit value at index index
 */
uint16_t getRawData(uint8_t index) {
	return times[index];
}

/**
 * Get the number of stored ir-times in the array
 * 
 * @return
 * 		The number of stored ir-times in the array
 */
uint8_t getRawDataCnt(void) {
	return timesCounter;
}

#if 1

/**
 * Test data on SIRC protocol, 12-bit version
 * http://www.sbprojects.com/knowledge/ir/sirc.htm
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 * @return
 * 		IR_OK if data parsed successfully, one of several errormessages if not
 */
uint8_t testSIRC(uint8_t *address, uint8_t *command) {
	/* parse times[], max is timesCounter */

	/* check if we have correct amount of data */ 
	if (timesCounter != 25) {
		return IR_NOT_CORRECT_DATA;
	}
	
	/* check startbit */
	if (times[0] > IR_SIRC_ST_BIT + IR_SIRC_ST_BIT/5 || times[0] < IR_SIRC_ST_BIT - IR_SIRC_ST_BIT/5) {
		return IR_NOT_CORRECT_DATA;
	}
	
	uint16_t rawbits=0;
	
	for (uint8_t i = 1; i < timesCounter; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			/* check length of pause between bits */
			if (times[i] > IR_SIRC_LOW + IR_SIRC_LOW/5 || times[i] < IR_SIRC_LOW - IR_SIRC_LOW/5) {
				return IR_NOT_CORRECT_DATA;
			}
		} else {			/* if even, ir-bit */
			if (times[i] > IR_SIRC_HIGH_ONE - IR_SIRC_HIGH_ONE/5 && times[i] < IR_SIRC_HIGH_ONE + IR_SIRC_HIGH_ONE/5) {
				/* write a one */
				rawbits |= 1<<((i-2)>>1);
			} else if (times[i] > IR_SIRC_HIGH_ZERO - IR_SIRC_HIGH_ZERO/5 && times[i] < IR_SIRC_HIGH_ZERO + IR_SIRC_HIGH_ZERO/5) {
				/* do nothing, a zero is already in rawbits */
			} else {
				return IR_NOT_CORRECT_DATA;
			}
		}
	}
	
	*command = ((uint8_t)rawbits&0x7f);
	*address = ((uint8_t)(rawbits>>7)&0x1f);
	
	return IR_OK;
}

/**
 * Test data on RC5 protocol 
 * http://www.sbprojects.com/knowledge/ir/rc5.htm
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 * @return
 * 		IR_OK if data parsed successfully, one of several errormessages if not
 */
uint8_t testRC5(uint8_t *address, uint8_t *command) {
	uint8_t halfbitscnt = 1;
	uint16_t rawbits = 0;
	
	for (uint8_t i = 0; i<timesCounter; i++) {
		//halfbitscnt&1==1 in the middle of bits
		//i&1==0 positive flank

		if ((halfbitscnt&1)==1 && (i&1)==0) {		/* in the middle of bit AND a positve flank */
			rawbits |= (1<<(13-(halfbitscnt>>1)));
		}
		
		if (times[i] > IR_RC5_HALF_BIT - IR_RC5_HALF_BIT/5 && times[i] < IR_RC5_HALF_BIT + IR_RC5_HALF_BIT/5) {
			halfbitscnt += 1;
		} else if (times[i] > IR_RC5_BIT - IR_RC5_BIT/5 && times[i] < IR_RC5_BIT + IR_RC5_BIT/5) {
			halfbitscnt += 2;
		} else {
			return IR_NOT_CORRECT_DATA;
		}
		
	}

	*command = ((uint8_t)rawbits&0x3f);
	*address = ((uint8_t)(rawbits>>6)&0x7f);	

	//support RC5-extended by putting the inversion of startbit 2 as the 7th commandbit
	*command |= (~(*address) & 0x40);
	*address &= 0x1f;		//remove togglebit and both startbits from address
	
	return IR_OK;
}
	
uint8_t IrReceive_CheckIR(uint8_t *proto, uint8_t *address, uint8_t *command, uint16_t *timeout) {
	if (IRPIN & (1<<IRBIT)) return IR_NO_DATA;		//om irmodulen ger en etta så återgå
	
	initTimer();
	
	//this routine should sample the timings for received irdata
	timesCounter = 0;
	
	uint8_t gotTimout = 0;	// a timeout is a very long break in irdata (such as occurs between ir pulsetrains)
	while (!gotTimout && timesCounter < MAX_NR_TIMES-1) { //loop as long as no timeout occured and not to many pulses received
		setTimerVal(TIM_OVFL_RELOAD_VAL);
		//wait for positiv slope, while checking timer overflow
		while (!(IRPIN & (1<<IRBIT))) {
			//if timeout (if timer-ovflow-flaggan is set)
			if (isTimerOvfl() == 1) return IR_TIME_OVFL;	//this timeout is not good (irsignal low which means active)
		}
		
		times[timesCounter] = getTimerVal()-TIM_OVFL_RELOAD_VAL;
		//TODO: should we check times[timesCounter] so its a realistic time-value? 
		timesCounter++;
		
		setTimerVal(TIM_OVFL_RELOAD_VAL);
		//wait for negative slope, while checking timer overflow
		while ((IRPIN & (1<<IRBIT)) && !gotTimout) {
			//om timeout (om timer-ovflow-flaggan sätt)
			if (isTimerOvfl() == 1) gotTimout=1;	//return IR_TIME_OVFL;
		}

		if (!gotTimout) {
			times[timesCounter] = getTimerVal()-TIM_OVFL_RELOAD_VAL;
			//TODO: should we check times[timesCounter] so its a realistic time-value? 
			timesCounter++;
		}
	}
	if (timesCounter >= MAX_NR_TIMES-1) {
		//to much irdata, not good, abort
		return IR_TO_MUCH_DATA;
	}
	
	if (testSIRC(&*address, &*command) == IR_OK) {
		*proto = IR_PROTO_SIRC;
		*timeout = IR_SIRC_REPETITION;
		return IR_OK;
	} else if (testRC5(&*address, &*command) == IR_OK) {
		*proto = IR_PROTO_RC5;
		*timeout = IR_RC5_REPETITION;
		return IR_OK;
	}
		
	return IR_NO_PROTOCOL;
	
}
#endif

#if 0
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
#endif

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

//varför är denna funktion så komplex? jo den filtrerar bort korta ir-pulser
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
