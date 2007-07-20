/**
 * IR transmitter driver.
 * 
 * @date	2007-06-24
 * 
 * @author	Anders Runeson
 *   
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <config.h>
#include <drivers/ir/transmitter/irtransmitter.h>
#include <avr/io.h>

/*-----------------------------------------------------------------------------
 * Globals
 *---------------------------------------------------------------------------*/
static uint8_t stateTransmit = STATE_IDLE;
static uint8_t repeateProto = IR_PROTO_UNKNOWN;
static uint8_t repeateAddress;
static uint8_t repeateCommand;
static uint8_t repeateTime=100;
static uint8_t repeateReps=1;
static uint8_t repeateAdditional=0;			//can be used for any protocol as general purpose variable

/*-----------------------------------------------------------------------------
 * Prerequisites
 *---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/


void setIrPort(void) {
	TCCR0A |= (1<<COM0A0);
}

void clearIrPort(void) {
	TCCR0A &= ~(1<<COM0A0);
}

/**
 * Transmit RC5 data, http://www.sbprojects.com/knowledge/ir/rc5.htm
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 * @return
 * 		IR_OK if data received successfully, one of several errormessages if not
 */
uint8_t transmitRC5(uint8_t address, uint8_t command) {
	OCR0A = (((F_CPU/2000)/IR_RC5_F_MOD) -1);		// set up modulation
	
	clearIrPort();									// set port low to be on the safe side
	repeateTime = IR_RC5_REPETITION;				// set up repitition time
	return IR_OK;
}

/**
 * Transmit SIRC data, http://www.sbprojects.com/knowledge/ir/sirc.htm
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 * @return
 * 		IR_OK if data received successfully, one of several errormessages if not
 */
uint8_t transmitSIRC(uint8_t address, uint8_t command) {
	OCR0A = (((F_CPU/2000)/IR_SIRC_F_MOD) -1);		// set up modulation
	
	clearIrPort();									// set port low to be on the safe side
	repeateTime = IR_SIRC_REPETITION;				// set up repitition time
	return IR_OK;
}

/**
 * Transmit Sharp data, http://www.sbprojects.com/knowledge/ir/sharp.htm
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 * @return
 * 		IR_OK if data received successfully, one of several errormessages if not
 */
uint8_t transmitSharp(uint8_t address, uint8_t command) {
	OCR0A = (((F_CPU/2000)/IR_SHARP_F_MOD) -1);		// set up modulation
	
	clearIrPort();									// set port low to be on the safe side
	repeateTime = IR_SHARP_REPETITION;				// set up repitition time
	return IR_OK;
}


/**
 * Transmit Samsung data
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 * @return
 * 		IR_OK if data received successfully, one of several errormessages if not
 */
uint8_t transmitSamsung(uint8_t address, uint8_t command) {
	OCR0A = (((F_CPU/2000)/IR_SAMS_F_MOD) -1);		// set up modulation
	
	/* Send startbit */
	setTimerVal(65536-IR_SAMS_ST_BIT);
	setIrPort();
	while(!isTimerOvfl()) {}
	clearIrPort();
	/* Send startpause */
	setTimerVal(65536-IR_SAMS_ST_PAUSE);
	while(!isTimerOvfl()) {}
	
	/* first send address */
	uint8_t byteToSend = address;
	for (uint8_t i = 0; i < 65; i++) {
		if (i==16) {
			/* then send address again */
			//byteToSend = address;
		} else if (i==32) {
			/* then send command */
			byteToSend = command;
		} else if (i==48) {
			/* then send inverted command */
			byteToSend = ~command;
		}
		if ((i&1) == 1) {		/* if odd, ir-pause */
			if ((byteToSend>>((i>>1)&0x7))&1) {
				setTimerVal(65536-IR_SAMS_LOW_ONE);
			} else {
				setTimerVal(65536-IR_SAMS_LOW_ZERO);
			}
			
			while(!isTimerOvfl()) {}
		} else {				/* if even, ir-bit */
			setIrPort();
			setTimerVal(65536-IR_SAMS_HIGH);
			while(!isTimerOvfl()) {}
			clearIrPort();
		}
	}
	
	clearIrPort();									// set port low to be on the safe side
	repeateTime = IR_SAMS_REPETITION;				// set up repitition time
	return IR_OK;
}


//TODO: skriv doxygen-header som för de andra funktionerna
/**
 * 
 * 
 * 
 * 
 */
void IrTransmit_Init(void) {
	IRDDR |= (1<<IRBIT);
	
	#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__)
	TCCR0A = (0<<COM0A1)|(0<<COM0A0)|(1<<WGM01)|(0<<WGM00);
	TCCR0B = (0<<WGM02)|(1<<CS00)|(0<<CS01)|(0<<CS02);
	#endif
	
	initTimer();
}

uint8_t doTransmit(uint8_t proto, uint8_t address, uint8_t command) {
	if (proto == IR_PROTO_SHARP) {
		return transmitSharp(address, command);
	} else if (proto == IR_PROTO_SAMS) {
		return transmitSamsung(address, command);
	} /*else if (proto == IR_PROTO_NEC) {
		return transmitRC5(address, command);
	} else if (proto == IR_PROTO_RC5) {
		return transmitRC5(address, command);
	} */
	
	
	return IR_NO_PROTOCOL;
	
}


/**
 * Start transmit ir 
 * 
 * @param proto
 * 		The protocol of the data to be transmitted
 * @param address
 * 		The address of the data to be transmitted
 * @param command
 * 		The command of the data to be transmitted
 * @return
 * 		IR_OK if data received successfully, one of several errormessages if not
 */
uint8_t IrTransmit_Start(uint8_t proto, uint8_t address, uint8_t command) {
	repeateProto = proto;
	repeateAddress = address;
	repeateCommand = command;
	repeateTime = 100;			//set a default repeateTime, if transmit protocol misses that
	repeateAdditional = 0;
	
	if (proto == IR_PROTO_SHARP) {
		repeateReps = IR_SHARP_REPS;
	} else if (proto == IR_PROTO_SAMS) {
		repeateReps = IR_SAMS_REPS;
	}
	
	uint8_t retval = doTransmit(proto, address, command);
	if (retval != IR_OK) {
		return retval;
	}
	stateTransmit = STATE_TRANS;
	
	return IR_OK;
}

/**
 * Call this function as often as possible during a transmission
 * should be replaced with an interrupted timer
 * 
 */
void IrTransmit_Poll(void) {
	if (stateTransmit == STATE_TRANS) { 
		if (isTimerOvfl()) {
			repeateTime--;
			if (repeateTime == 0) {
				if (repeateReps > 0) {
					repeateReps--;
				}
				doTransmit(repeateProto, repeateAddress, repeateCommand);
			} else {
				setTimerVal(TIM_1MS_RELOAD_VAL);
			}
		}
	}
}

/**
 * Dont stop polling until the stopfunction returns ok
 * 
 * 
 */
uint8_t IrTransmit_Stop(void) {
	
	if (repeateReps > 0) {
		return IR_NOT_FINISHED;
	}
	stateTransmit = STATE_IDLE;
	
	return IR_OK;
}

