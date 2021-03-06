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
	if (times[0] > IR_SIRC_ST_BIT + IR_SIRC_ST_BIT/IR_SIRC_TOL_DIV || times[0] < IR_SIRC_ST_BIT - IR_SIRC_ST_BIT/IR_SIRC_TOL_DIV) {
		return IR_NOT_CORRECT_DATA;
	}
	
	uint16_t rawbits=0;
	
	for (uint8_t i = 1; i < timesCounter; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			/* check length of pause between bits */
			if (times[i] > IR_SIRC_LOW + IR_SIRC_LOW/IR_SIRC_TOL_DIV || times[i] < IR_SIRC_LOW - IR_SIRC_LOW/IR_SIRC_TOL_DIV) {
				return IR_NOT_CORRECT_DATA;
			}
		} else {			/* if even, ir-bit */
			if (times[i] > IR_SIRC_HIGH_ONE - IR_SIRC_HIGH_ONE/IR_SIRC_TOL_DIV && times[i] < IR_SIRC_HIGH_ONE + IR_SIRC_HIGH_ONE/IR_SIRC_TOL_DIV) {
				/* write a one */
				rawbits |= 1<<((i-2)>>1);
			} else if (times[i] > IR_SIRC_HIGH_ZERO - IR_SIRC_HIGH_ZERO/IR_SIRC_TOL_DIV && times[i] < IR_SIRC_HIGH_ZERO + IR_SIRC_HIGH_ZERO/IR_SIRC_TOL_DIV) {
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
		
		if (times[i] > IR_RC5_HALF_BIT - IR_RC5_HALF_BIT/IR_RC5_TOL_DIV && times[i] < IR_RC5_HALF_BIT + IR_RC5_HALF_BIT/IR_RC5_TOL_DIV) {
			halfbitscnt += 1;
		} else if (times[i] > IR_RC5_BIT - IR_RC5_BIT/IR_RC5_TOL_DIV && times[i] < IR_RC5_BIT + IR_RC5_BIT/IR_RC5_TOL_DIV) {
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

/**
 * Test data on SHARP protocol
 * http://www.sbprojects.com/knowledge/ir/sharp.htm
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 * @return
 * 		IR_OK if data parsed successfully, one of several errormessages if not
 */
uint8_t testSharp(uint8_t *address, uint8_t *command) {
	/* parse times[], max is timesCounter */

	/* check if we have correct amount of data */ 
	if (timesCounter != 31) {
		return IR_NOT_CORRECT_DATA;
	}
	
	uint16_t rawbits=0;
	
	for (uint8_t i = 1; i < timesCounter; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			/* check length of pause between bits */
			if (times[i] > IR_SHARP_LOW_ONE - IR_SHARP_LOW_ONE/IR_SHARP_TOL_DIV && times[i] < IR_SHARP_LOW_ONE + IR_SHARP_LOW_ONE/IR_SHARP_TOL_DIV) {
				/* write a one */
				rawbits |= 1<<((i-1)>>1);
			} else if (times[i] > IR_SHARP_LOW_ZERO - IR_SHARP_LOW_ZERO/IR_SHARP_TOL_DIV && times[i] < IR_SHARP_LOW_ZERO + IR_SHARP_LOW_ZERO/IR_SHARP_TOL_DIV) {
				/* do nothing, a zero is already in rawbits */
			} else {
				return IR_NOT_CORRECT_DATA;
			}
		} else {			/* if even, ir-bit */
			if (times[i] > IR_SHARP_HIGH + IR_SHARP_HIGH/IR_SHARP_TOL_DIV || times[i] < IR_SHARP_HIGH - IR_SHARP_HIGH/IR_SHARP_TOL_DIV) {
				return IR_NOT_CORRECT_DATA;
			}
		}
	}
	
	*command = ((uint8_t)(rawbits>>5)&0xff);
	*address = ((uint8_t)rawbits&0x1f);
	
	return IR_OK;
}

/**
 * Test data on NEC protocol
 * http://www.sbprojects.com/knowledge/ir/nec.htm
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 * @return
 * 		IR_OK if data parsed successfully, one of several errormessages if not
 */
uint8_t testNEC(uint8_t *address, uint8_t *command) {
	/* parse times[], max is timesCounter */

	/* check if we have correct amount of data */ 
	if (timesCounter != 67) {
		return IR_NOT_CORRECT_DATA;
	}
	
	/* check startbit */
	if (times[0] > IR_NEC_ST_BIT + IR_NEC_ST_BIT/IR_NEC_TOL_DIV || times[0] < IR_NEC_ST_BIT - IR_NEC_ST_BIT/IR_NEC_TOL_DIV) {
		return IR_NOT_CORRECT_DATA;
	}

	/* check pause after startbit */
	if (times[1] > IR_NEC_ST_PAUSE + IR_NEC_ST_PAUSE/IR_NEC_TOL_DIV || times[1] < IR_NEC_ST_PAUSE - IR_NEC_ST_PAUSE/IR_NEC_TOL_DIV) {
		return IR_NOT_CORRECT_DATA;
	}

	*command = 0;
	*address = 0;
	
	for (uint8_t i = 3; i < timesCounter; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			/* check length of pause between bits */
			if (times[i] > IR_NEC_LOW_ONE - IR_NEC_LOW_ONE/IR_NEC_TOL_DIV && times[i] < IR_NEC_LOW_ONE + IR_NEC_LOW_ONE/IR_NEC_TOL_DIV) {
				if (i>2 && i<18) {
					/* write a one */
					*address |= 1<<((i-3)>>1);
				}
				if (i>34 && i<50) {
					/* write a one */
					*command |= 1<<((i-35)>>1);
				}
			} else if (times[i] > IR_NEC_LOW_ZERO - IR_NEC_LOW_ZERO/IR_NEC_TOL_DIV && times[i] < IR_NEC_LOW_ZERO + IR_NEC_LOW_ZERO/IR_NEC_TOL_DIV) {
				/* do nothing, a zero is already in rawbits */
			} else {
				return IR_NOT_CORRECT_DATA;
			}
		} else {			/* if even, ir-bit */
			if (times[i] > IR_NEC_HIGH + IR_NEC_HIGH/IR_NEC_TOL_DIV || times[i] < IR_NEC_HIGH - IR_NEC_HIGH/IR_NEC_TOL_DIV) {
				return IR_NOT_CORRECT_DATA;
			}
		}
	}
	
	return IR_OK;
}


/**
 * Test data on Samsung protocol
 * Very much like NEC, different start bit/pause lengths etc.
 * http://www.sbprojects.com/knowledge/ir/nec.htm
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 * @return
 * 		IR_OK if data parsed successfully, one of several errormessages if not
 */
uint8_t testSamsung(uint8_t *address, uint8_t *command) {
	/* parse times[], max is timesCounter */

	/* check if we have correct amount of data */ 
	if (timesCounter != 67) {
		return IR_NOT_CORRECT_DATA;
	}
	
	/* check startbit */
	if (times[0] > IR_SAMS_ST_BIT + IR_SAMS_ST_BIT/IR_SAMS_TOL_DIV || times[0] < IR_SAMS_ST_BIT - IR_SAMS_ST_BIT/IR_SAMS_TOL_DIV) {
		return IR_NOT_CORRECT_DATA;
	}

	/* check pause after startbit */
	if (times[1] > IR_SAMS_ST_PAUSE + IR_SAMS_ST_PAUSE/IR_SAMS_TOL_DIV || times[1] < IR_SAMS_ST_PAUSE - IR_SAMS_ST_PAUSE/IR_SAMS_TOL_DIV) {
		return IR_NOT_CORRECT_DATA;
	}

	*command = 0;
	*address = 0;
	
	for (uint8_t i = 2; i < timesCounter; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			/* check length of pause between bits */
			if (times[i] > IR_SAMS_LOW_ONE - IR_SAMS_LOW_ONE/IR_SAMS_TOL_DIV && times[i] < IR_SAMS_LOW_ONE + IR_SAMS_LOW_ONE/IR_SAMS_TOL_DIV) {
				if (i>2 && i<18) {
					/* write a one */
					*address |= 1<<((i-3)>>1);
				}
				if (i>34 && i<50) {
					/* write a one */
					*command |= 1<<((i-35)>>1);
				}
			} else if (times[i] > IR_SAMS_LOW_ZERO - IR_SAMS_LOW_ZERO/IR_SAMS_TOL_DIV && times[i] < IR_SAMS_LOW_ZERO + IR_SAMS_LOW_ZERO/IR_SAMS_TOL_DIV) {
				/* do nothing, a zero is already in rawbits */
			} else {
				return IR_NOT_CORRECT_DATA;
			}
		} else {			/* if even, ir-bit */
			if (times[i] > IR_SAMS_HIGH + IR_SAMS_HIGH/IR_SAMS_TOL_DIV || times[i] < IR_SAMS_HIGH - IR_SAMS_HIGH/IR_SAMS_TOL_DIV) {
				return IR_NOT_CORRECT_DATA;
			}
		}
	}
	
	return IR_OK;
}

uint8_t IrReceive_CheckIR(uint8_t *proto, uint8_t *address, uint8_t *command, uint16_t *timeout) {
	if (IRPIN & (1<<IRBIT)) return IR_NO_DATA;		//om irmodulen ger en etta så återgå
	
	initTimer();
	
	//this routine should sample the timings for received irdata
	timesCounter = 0;
	
	uint8_t gotTimout = 0;	// a timeout is a very long break in irdata (such as occurs between ir frames)
	while (!gotTimout && timesCounter < MAX_NR_TIMES-1) { //loop as long as no timeout occured and not to many pulses received
		setTimerVal(TIM_OVFL_RELOAD_VAL);
		//wait for positiv slope, while checking timer overflow
		while (!(IRPIN & (1<<IRBIT))) {
			//if timeout (if timer-ovflow-flaggan is set)
			if (isTimerOvfl() == 1) return IR_TIME_OVFL;	//this timeout is not good (irsignal low which means active)
		}
		
		times[timesCounter] = getTimerVal()-TIM_OVFL_RELOAD_VAL;
		//TODO: should we check times[timesCounter] so its a realistic time-value? use IR_MIN_PULSE_WIDTH
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
	} else if (testSharp(&*address, &*command) == IR_OK) {
		*proto = IR_PROTO_SHARP;
		*timeout = IR_SHARP_REPETITION;
		return IR_OK;
	} else if (testNEC(&*address, &*command) == IR_OK) {
		*proto = IR_PROTO_NEC;
		*timeout = IR_NEC_REPETITION;
		return IR_OK;
	} else if (testSamsung(&*address, &*command) == IR_OK) {
		*proto = IR_PROTO_SAMS;
		*timeout = IR_SAMS_REPETITION;
		return IR_OK;
	}
		
	return IR_NO_PROTOCOL;
	
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

//varför är denna funktion så komplex? jo den filtrerar bort för korta ir-pulser
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
	
	if ((timerVal < IR_MAX_PULSE_WIDTH) && (timerVal > IR_MIN_PULSE_WIDTH)) {
		return IR_OK;
	} //else if ...
	
	
	return IR_NO_PROTOCOL;
}
