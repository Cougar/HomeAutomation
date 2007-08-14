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
#include <drivers/ir/transceiver/irtransceiver.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/*-----------------------------------------------------------------------------
 * Globals
 *---------------------------------------------------------------------------*/
static uint16_t times[MAX_NR_TIMES];			//stores 
static uint8_t timesCounter=0;					//counts the items in Times, always less then MAX_NR_TIMES
static uint8_t timesIndex;						//selects the pulse width in Times to send
volatile uint8_t data_received;
volatile uint8_t data_transmitted;
uint8_t detect_edge;
uint8_t store;

/*-----------------------------------------------------------------------------
 * Prerequisites
 *---------------------------------------------------------------------------*/
#define IR_RX_ACTIVE_LOW	1
#define IR_TX_ACTIVE_LOW	0		//Only 0 is implemented


#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__)
#define TIMSK1	TIMSK
#define TIFR1	TIFR
#define ICIE1	TICIE1
#endif

#define IR_COMPARE_VECTOR		TIMER1_COMPB_vect
#define IR_TIMEOUT_VECTOR		TIMER1_COMPA_vect
#define IR_CAPTURE_VECTOR		TIMER1_CAPT_vect
#define IR_TIMEOUT_REG			OCR1A
#define IR_COMPARE_REG			OCR1B
#define IR_CAPTURE_REG			ICR1
#define IR_COUNT_REG			TCNT1
#define IR_TIMER_INIT()			TCCR1A = 0; \
								TCCR1B = (1<<ICNC1)|(2<<CS10);
#define IR_MASK_COMPARE()		TIMSK1 &= ~(1<<OCIE1B);
#define IR_UNMASK_COMPARE()		TIFR1 = (1<<OCF1B); TIMSK1 |= (1<<OCIE1B);
#define IR_MASK_TIMEOUT()		TIMSK1 &= ~(1<<OCIE1A);
#define IR_UNMASK_TIMEOUT()		TIFR1 = (1<<OCF1A); TIMSK1 |= (1<<OCIE1A);
#define IR_MASK_CAPTURE()		TIMSK1 &= ~(1<<ICIE1);
#define IR_UNMASK_CAPTURE()		TIFR1 = (1<<ICF1); TIMSK1 |= (1<<ICIE1);
#define IR_CAPTURE_FALLING()	TCCR1B &= ~(1<<ICES1); \
								TIFR1 = (1<<ICF1);
#define IR_CAPTURE_RISING()		TCCR1B |= (1<<ICES1); \
								TIFR1 = (1<<ICF1);

//TODO: if active low is 0 then make port 5V at LOW
#if IR_TX_ACTIVE_LOW==1
#define IR_OUTP_HIGH()			TCCR0A |= (1<<COM0A0);
#define IR_OUTP_LOW()			TCCR0A &= ~(1<<COM0A0);
#else
#define IR_OUTP_HIGH()			TCCR0A |= (1<<COM0A0);
#define IR_OUTP_LOW()			TCCR0A &= ~(1<<COM0A0);
#endif


/*-----------------------------------------------------------------------------
 * Interrupt Handlers
 *---------------------------------------------------------------------------*/

ISR(IR_COMPARE_VECTOR)
{
	if ((timesIndex&1) == 1) {		/* if odd, ir-pause */
		IR_OUTP_LOW();
	} else {
		IR_OUTP_HIGH();
	}
	
	if (timesIndex < timesCounter)
	{
		IR_COMPARE_REG += times[timesIndex++];		
	}
	else
	{
		IR_MASK_COMPARE();
		data_transmitted = 1;
	}
}

ISR(IR_TIMEOUT_VECTOR)
{
	if (timesCounter)
	{
		/* Disable interrupts until the application has taken action on the
		 * current data and reenabled reception. */
		IR_MASK_CAPTURE();
		IR_MASK_TIMEOUT();
		/* Notify the application that a pulse train has been received. */
		data_received = 1;
	}
	store = 0;
}

ISR(IR_CAPTURE_VECTOR)
{
	static uint16_t prev_time;
	uint16_t pulsewidth;

	/* Read the measured transition time from the capture register. */
	uint16_t time = IR_CAPTURE_REG;
	
	/* Toggle the edge detection. */
	if (detect_edge == 0)
	{
		IR_CAPTURE_RISING();
		detect_edge = 1;
	}
	else
	{
		IR_CAPTURE_FALLING();
		detect_edge = 0;
	}
	
	/* Subtract the current measurement from the previous to get the pulse
	 * width. */
	pulsewidth = time - prev_time;
	prev_time = time;
	
	/* Set the timeout. */
	IR_TIMEOUT_REG = time + IR_MAX_PULSE_WIDTH;
	
	if (store)
	{
		/* Store the measurement. */
		times[timesCounter++] = pulsewidth;
		/* Disable future measurements if we've filled the buffer. */
		//TODO: Report overflow to application
		if (timesCounter == MAX_NR_TIMES)
		{
			IR_MASK_CAPTURE();
		}
	}
	else
	{
		/* The first edge of the pulse train has been detected. Enable the
		 * storage of the following pulsewidths. */
		store = 1;
		/* Enable timeout interrupt for detection of the end of the pulse
		 * train. */
		IR_UNMASK_TIMEOUT();
	}
}



/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

/**
 * Prepare Samsung data
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 */
void prepareSamsung(uint32_t data) {
	/* Set up startbit */
	times[0] = IR_SAMS_ST_BIT;
	times[1] = IR_SAMS_ST_PAUSE;
	
	for (uint8_t i = 0; i < 65; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			if ((data>>(i>>1))&1) {
				times[i+2] = IR_SAMS_LOW_ONE;
			} else {
				times[i+2] = IR_SAMS_LOW_ZERO;
			}
		} else {				/* if even, ir-bit */
			times[i+2] = IR_SAMS_HIGH;
		}
	}
	
	timesCounter = 67;
}

/**
 * Prepare NEC data
 * 
 * @param address
 * 		Pointer to store the address of the received data
 * @param command
 * 		Pointer to store the command of the received data
 */
void prepareNEC(uint32_t data) {
	/* Set up startbit */
	times[0] = IR_NEC_ST_BIT;
	times[1] = IR_NEC_ST_PAUSE;

	timesCounter = 67;
	for (uint8_t i = 0; i < 65; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			if ((data>>(i>>1))&1) {
				times[i+2] = IR_NEC_LOW_ONE;
			} else {
				times[i+2] = IR_NEC_LOW_ZERO;
			}
		} else {			/* if even, ir-bit */
			times[i+2] = IR_NEC_HIGH;
		}
	}
}

void prepareSharp(uint32_t data) {}

void IrTransceiver_Init(void)
{
	/* Set up receiver */
	IR_TIMER_INIT();
	IR_TIMEOUT_REG = IR_MAX_PULSE_WIDTH;
	IR_R_DDR &= ~(1<<IR_R_BIT);
	//DDRC |= (1<<PC5);
	
	/* Set up transmitter */
	IR_T_DDR |= (1<<IR_T_BIT);
	#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__)
	TCCR0A = (0<<COM0A1)|(0<<COM0A0)|(1<<WGM01)|(0<<WGM00);
	TCCR0B = (0<<WGM02)|(1<<CS00)|(0<<CS01)|(0<<CS02);
	#endif

}

void IrTransceiver_Start(void)
{
	/* Clear buffer and arm the edge detection. */
	timesCounter = 0;
	data_received = 0;
	store = 0;
#if (IR_RX_ACTIVE_LOW == 1)
	IR_CAPTURE_FALLING();
	detect_edge = 0;
#else
	IR_CAPTURE_RISING();
	detect_edge = 1;
#endif
	IR_UNMASK_CAPTURE();
}

uint8_t IrTransceiver_Poll(uint16_t **buffer, uint8_t *length)
{
	if (data_received)
	{
		*buffer = times;
		*length = timesCounter;
		return IR_OK;
	}
	else
	{
		return IR_NO_DATA;
	}
}

#if 0
uint8_t IrTransceiver_Transmit(uint16_t **buffer, uint8_t *length)
{
	data_transmitted = 0;
	if (timesCounter == 0) return IR_NO_DATA;
	timesIndex = 1;
	IR_COMPARE_REG = IR_COUNT_REG + times[0];
#if (IR_RX_ACTIVE_LOW == 1)
	PORTC &= ~(1<<PC5);
#else
	PORTC |= (1<<PC5);
#endif
	IR_UNMASK_COMPARE();
	
	while (!data_transmitted);
	
	return IR_OK;
}
#endif

uint8_t IrTransceiver_Transmit_Poll(void) {
	if (data_transmitted == 0) {
		return IR_NOT_FINISHED;
	} else {
		return IR_OK;
	}
}

uint8_t IrTransceiver_Transmit(uint8_t proto, uint32_t data, uint16_t *timeout, uint8_t *repeates)
{
	timesCounter = 0;
	//prepare rawdata here
	if (proto == IR_PROTO_SHARP) {
		prepareSharp(data);
		OCR0A = (((F_CPU/2000)/IR_SHARP_F_MOD) -1);		// set up modulation
		*repeates = IR_SHARP_REPS;
		*timeout = IR_SHARP_REPETITION;
	} else if (proto == IR_PROTO_SAMS) {
		prepareSamsung(data);
		OCR0A = (((F_CPU/2000)/IR_SAMS_F_MOD) -1);		// set up modulation
		*repeates = IR_SAMS_REPS;
		*timeout = IR_SAMS_REPETITION;
	} else if (proto == IR_PROTO_NEC) {
		prepareNEC(data);
		OCR0A = (((F_CPU/2000)/IR_NEC_F_MOD) -1);		// set up modulation
		*repeates = IR_NEC_REPS;
		*timeout = IR_NEC_REPETITION;
	}
	
	data_transmitted = 0;
	if (timesCounter == 0) return IR_NO_DATA;
	timesIndex = 1;
	IR_COMPARE_REG = IR_COUNT_REG + times[0];
	
	IR_OUTP_HIGH();	
	
	IR_UNMASK_COMPARE();
	
	return IR_OK;
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

/**
 * Test data on SIRC protocol, 12-bit version
 * http://www.sbprojects.com/knowledge/ir/sirc.htm
 * 
 * @param data
 * 		Pointer to store the received data
 * @return
 * 		IR_OK if data parsed successfully, one of several errormessages if not
 */
uint8_t testSIRC(uint32_t *data) {
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
	
	*data = rawbits;
	
	return IR_OK;
}

/**
 * Test data on RC5 protocol 
 * http://www.sbprojects.com/knowledge/ir/rc5.htm
 * 
 * @param data
 * 		Pointer to store the received data
 * @return
 * 		IR_OK if data parsed successfully, one of several errormessages if not
 */
uint8_t testRC5(uint32_t *data) {
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
	
	//support RC5-extended keeping second startbit 
	//remove togglebit
	*data = rawbits&0x37ff;

	return IR_OK;
}

/**
 * Test data on SHARP protocol
 * http://www.sbprojects.com/knowledge/ir/sharp.htm
 * 
 * @param data
 * 		Pointer to store the received data
 * @return
 * 		IR_OK if data parsed successfully, one of several errormessages if not
 */
uint8_t testSharp(uint32_t *data) {
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
	
	*data = rawbits;
	
	return IR_OK;
}

/**
 * Test data on NEC protocol
 * http://www.sbprojects.com/knowledge/ir/nec.htm
 * 
 * @param data
 * 		Pointer to store the received data
 * @return
 * 		IR_OK if data parsed successfully, one of several errormessages if not
 */
uint8_t testNEC(uint32_t *data) {
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

	*data = 0;
	for (uint8_t i = 3; i < timesCounter; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			/* check length of pause between bits */
			if (times[i] > IR_NEC_LOW_ONE - IR_NEC_LOW_ONE/IR_NEC_TOL_DIV && times[i] < IR_NEC_LOW_ONE + IR_NEC_LOW_ONE/IR_NEC_TOL_DIV) {
				/* write a one */
				*data |= 1UL<<((i-3)>>1);
			} else if (times[i] > IR_NEC_LOW_ZERO - IR_NEC_LOW_ZERO/IR_NEC_TOL_DIV && times[i] < IR_NEC_LOW_ZERO + IR_NEC_LOW_ZERO/IR_NEC_TOL_DIV) {
				/* do nothing, a zero is already in place */
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
 * @param data
 * 		Pointer to store the received data
 * @return
 * 		IR_OK if data parsed successfully, one of several errormessages if not
 */
uint8_t testSamsung(uint32_t *data) {
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

	*data = 0;
	for (uint8_t i = 3; i < timesCounter; i++) {
		if ((i&1) == 1) {		/* if odd, ir-pause */
			/* check length of pause between bits */
			if (times[i] > IR_SAMS_LOW_ONE - IR_SAMS_LOW_ONE/IR_SAMS_TOL_DIV && times[i] < IR_SAMS_LOW_ONE + IR_SAMS_LOW_ONE/IR_SAMS_TOL_DIV) {
				/* write a one */
				*data |= 1UL<<((i-3)>>1);
			} else if (times[i] > IR_SAMS_LOW_ZERO - IR_SAMS_LOW_ZERO/IR_SAMS_TOL_DIV && times[i] < IR_SAMS_LOW_ZERO + IR_SAMS_LOW_ZERO/IR_SAMS_TOL_DIV) {
				/* do nothing, a zero is already in place */
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

//uint8_t IrReceive_CheckIR(uint8_t *proto, uint8_t *address, uint8_t *command, uint16_t *timeout) {
uint8_t IrReceive_CheckIR(uint8_t *proto, uint32_t *data, uint16_t *timeout) {
	IrTransceiver_Start();
	
	while (!data_received);
	
	if (testSIRC(&*data) == IR_OK) {
		*proto = IR_PROTO_SIRC;
		*timeout = IR_SIRC_REPETITION;
		return IR_OK;
	} else if (testRC5(&*data) == IR_OK) {
		*proto = IR_PROTO_RC5;
		*timeout = IR_RC5_REPETITION;
		return IR_OK;
	} else if (testSharp(&*data) == IR_OK) {
		*proto = IR_PROTO_SHARP;
		*timeout = IR_SHARP_REPETITION;
		return IR_OK;
	} else if (testNEC(&*data) == IR_OK) {
		*proto = IR_PROTO_NEC;
		*timeout = IR_NEC_REPETITION;
		return IR_OK;
	} else if (testSamsung(&*data) == IR_OK) {
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
	IrTransceiver_Init();
	/*IR_R_DDR &= ~(1<<IR_R_BIT);*/
}

//varför är denna funktion så komplex? jo den filtrerar bort korta ir-pulser
//TODO: skriv doxygen-header som för de andra funktionerna
uint8_t IrReceive_CheckIdle(void) {
	if (IR_R_PIN & (1<<IR_R_BIT)) return IR_NO_DATA;		//om irmodulen ger en etta så återgå
	
	//nu lägger irmodulen ut en nolla, "startbiten" alltså
	
	uint16_t timerVal;
	
	//Läs in längden på startbiten
	initTimer();
	while (!(IR_R_PIN & (1<<IR_R_BIT))) {		//vänta på att irmodulen lägger ut en etta
		//om timeout (om timer-ovflow-flaggan sätt)
		if (isTimerOvfl() == 1) return IR_TIME_OVFL;
	}
	timerVal = getTimerVal();
	
	if ((timerVal < IR_MAX_PULSE_WIDTH) && (timerVal > IR_MIN_PULSE_WIDTH)) {
		return IR_OK;
	} //else if ...
	
	
	return IR_NO_PROTOCOL;
}
