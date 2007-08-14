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
//static uint16_t times[MAX_NR_TIMES];			//stores
uint16_t *rxbuf, *txbuf;		
uint8_t rxlen, txlen;
//static uint8_t timesCounter=0;					//counts the items in Times, always less then MAX_NR_TIMES
//static uint8_t timesIndex;						//selects the pulse width in Times to send
volatile uint8_t data_received;
volatile uint8_t data_transmitted;
uint8_t detect_edge;
uint8_t store;

/*-----------------------------------------------------------------------------
 * Prerequisites
 *---------------------------------------------------------------------------*/
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

#define IR_RX_ACTIVE_LOW	1
#define IR_TX_ACTIVE_LOW	1

/*-----------------------------------------------------------------------------
 * Interrupt Handlers
 *---------------------------------------------------------------------------*/

ISR(IR_COMPARE_VECTOR)
{
	PORTC ^= (1<<PC5);
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

void IrTransceiver_Init(void)
{
	IR_TIMER_INIT();
	IR_TIMEOUT_REG = IR_MAX_PULSE_WIDTH;
	IRDDR &= ~(1<<IRBIT);
	DDRC |= (1<<PC5);
}

void IrTransceiver_Start(uint16_t *buffer)
{
	/* Setup buffer and arm the edge detection. */
	rxbuf = buffer;
	rxlen = 0;
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

uint8_t IrTransceiver_Poll(void)
{
	if (data_received)
	{
		return rxlen;
	}
	else
	{
		return 0;
	}
}

uint8_t IrTransceiver_Transmit(uint16_t *buffer, uint8_t length)
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

/* Behöver denna verkligen användas numer? Isf, kan samma sak implementeras
 * i applikationens tillståndsmaskin/main-loop med hjälp av standard timer.h
 * för timeout-funktioner? */
#if 0 
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
	
	if ((timerVal < IR_MAX_PULSE_WIDTH) && (timerVal > IR_MIN_PULSE_WIDTH)) {
		return IR_OK;
	} //else if ...
	
	
	return IR_NO_PROTOCOL;
}
#endif
