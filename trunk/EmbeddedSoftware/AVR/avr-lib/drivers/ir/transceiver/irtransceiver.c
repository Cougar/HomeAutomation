/**
 * IR receiver and transmitter driver.
 * 
 * @date	2006-12-10
 * 
 * @author	Anders Runeson, Andreas Fritiofson
 *   
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <config.h>
#include <drivers/ir/transceiver/irtransceiver.h>
#include <drivers/ir/protocols.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/*-----------------------------------------------------------------------------
 * Globals
 *---------------------------------------------------------------------------*/
uint16_t *rxbuf, *txbuf;
uint8_t rxlen, txlen;

static uint8_t bufIndex;							//selects the pulse width in buffer to send
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
#define IR_MODULATION_REG		OCR0A
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

//TODO: if active low is 0 then make port 5V at "LOW"
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
	if ((bufIndex&1) == 1) {		/* if odd, ir-pause */
		IR_OUTP_LOW();
	} else {
		IR_OUTP_HIGH();
	}
	
	if (bufIndex < txlen)
	{
		IR_COMPARE_REG += txbuf[bufIndex++];		
		//IR_COMPARE_REG += *(txbuf + bufIndex++);		
	}
	else
	{
		IR_MASK_COMPARE();
		data_transmitted = 1;
	}
}

ISR(IR_TIMEOUT_VECTOR)
{
	if (rxlen)
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
		rxbuf[rxlen++] = pulsewidth;
		//*(rxbuf+rxlen++) = pulsewidth;
		/* Disable future measurements if we've filled the buffer. */
		//TODO: Report overflow to application
		if (rxlen == MAX_NR_TIMES)
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

void IrTransceiver_Receive_Start(uint16_t *buffer)
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

uint8_t IrTransceiver_Receive_Poll(uint8_t *len)
{
	*len = rxlen;
	if (data_received)
	{
		return IR_OK;
	}
	else if (rxlen > 0) 
	{
		return IR_NOT_FINISHED;
	} else {
		return IR_NO_DATA;
	}
}

uint8_t IrTransceiver_Transmit_Poll(void) {
	if (data_transmitted == 0) {
		return IR_NOT_FINISHED;
	} else {
		return IR_OK;
	}
}

uint8_t IrTransceiver_Transmit(uint16_t *buffer, uint8_t length, uint8_t modfreq)
{
	data_transmitted = 0;
	
	if (length == 0) return IR_NO_DATA;

	txbuf = buffer;
	txlen = length;
	bufIndex = 1;
	
	IR_MODULATION_REG = modfreq;
	
	IR_COMPARE_REG = IR_COUNT_REG + txbuf[0];
	
	IR_OUTP_HIGH();	
	
	IR_UNMASK_COMPARE();
	
	//while (!data_transmitted);
	
	return IR_OK;
}
