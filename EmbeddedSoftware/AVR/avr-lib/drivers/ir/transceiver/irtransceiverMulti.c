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
#include <drivers/mcu/pcint.h>
#include <drivers/mcu/gpio.h>
#include <drivers/ir/transceiver/irtransceiverMulti.h>
#include <drivers/ir/protocols.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/*-----------------------------------------------------------------------------
 * Globals
 *---------------------------------------------------------------------------*/
#if IR_RX_ENABLE==1
struct {
	uint8_t		enable;
	uint16_t	timeout;
	uint8_t		timeoutEnable;
	uint16_t	*rxbuf;
	uint8_t		rxlen;
	uint8_t		storeEnable;
	irCallback_t callback;
} drvIrRxChannel[3];


#endif
#if IR_TX_ENABLE==1
uint16_t *txbuf;
uint8_t txlen;
static uint8_t bufIndex;							//selects the pulse width in buffer to send
volatile uint8_t data_transmitted;
#endif



/*-----------------------------------------------------------------------------
 * Prerequisites
 *---------------------------------------------------------------------------*/
#ifndef IR_RX_ACTIVE_LOW
#define IR_RX_ACTIVE_LOW    1
#endif

#ifndef IR_TX_ACTIVE_LOW
#define IR_TX_ACTIVE_LOW	0
#endif

#ifndef IR_TX_ENABLE
#define IR_TX_ENABLE	0
#endif

#ifndef IR_RX_ENABLE
#define IR_RX_ENABLE	0
#endif


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
/*#define IR_TIMER_INIT()			TCCR1A = 0; \
								TCCR1B = (1<<ICNC1)|(2<<CS10);
								*/
#define IR_TIMER_INIT()			TCCR1A = 0; \
								TCCR1B = (2<<CS10);
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

#define IR_OUTP_HIGH()			TCCR0A |= (1<<COM0A0);
#define IR_OUTP_LOW()			TCCR0A &= ~(1<<COM0A0);


/*-----------------------------------------------------------------------------
 * Interrupt Handlers
 *---------------------------------------------------------------------------*/


/*
timer 1 has two comprare registers and compare ISRs, one is used below for "timeout" on receive and the other is used here for transmitter
//TODO: how to handle more than one transmitter on one compare interrupt, spara en array med tider och nån enableflagga för varje kanal
*/

#if IR_TX_ENABLE==1
ISR(IR_COMPARE_VECTOR)
{
	if ((bufIndex&1) == 1) {		/* if odd, ir-pause */
		IR_OUTP_LOW();
	} else {
		IR_OUTP_HIGH();
	}
	
	//The following three lines is just to allow a start signal to be zero.
/*	bufIndex++;
	while (txbuf[bufIndex] == 0){
		bufIndex++;
	}*/
	
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
#endif

#if IR_RX_ENABLE==1
//TODO: hur hantera detta i multi receiver
//TODO: ha en array med tider och nån enableflagga för varje kanal
//TODO: mot slutet av ISRen laddas nästa tid in...

/* When this timeout occurs a pulsetrain is complete */
ISR(IR_TIMEOUT_VECTOR)
{
	/* Find which channel timed out */
	uint16_t timeDiff = 0xffff;
	uint8_t channel = 0;
/*	for (uint8_t i=0; i < 3; i++)
	{
		if ((drvIrRxChannel[channel].timeoutEnable==TRUE) && (IR_TIMEOUT_REG-drvIrRxChannel[i].timeout < timeDiff))
		{
			timeDiff = IR_TIMEOUT_REG-drvIrRxChannel[i].timeout;
			channel = i;
		}
	}
*/
	/* If more than 3 flanks was recevied */
	if (drvIrRxChannel[channel].rxlen > 3)
	{
		/* Notify the application that a pulse train has been received. */
		drvIrRxChannel[channel].callback(channel, drvIrRxChannel[channel].rxbuf, drvIrRxChannel[channel].rxlen);
	}

	drvIrRxChannel[channel].storeEnable = FALSE;
	drvIrRxChannel[channel].timeoutEnable = FALSE;

	IR_MASK_TIMEOUT();
	
//TODO: reset the timeout for next enabled channel
/*
	uint16_t nextTimeout = drvIrRxChannel[channel].timeout;
	for (uint8_t i=0; i < 3; i++)
	{
//TODO: hitta den med timeout härnäst (svårt med tanke på att den tiden kan ha passerat eftersom vi är under interrupt)
//kanske inte är så noga, i värsta fall tar det 65ms extra
		if ((drvIrRxChannel[channel].timeoutEnable) && (drvIrRxChannel[i].timeout < nextTimeout))
		{
			nextTimeout = drvIrRxChannel[i].timeout;
		}
	}
	IR_UNMASK_TIMEOUT();
	IR_TIMEOUT_REG = nextTimeout;
*/
}
#endif


/*-----------------------------------------------------------------------------
 * Private Functions
 *---------------------------------------------------------------------------*/
 
#if IR_RX_ENABLE==1

/* these wrapper functions are called from the pcint driver (callback) */
void IrTransceiver_Store_ch0(uint8_t id, uint8_t status)
{
	IrTransceiver_Store(0);
}
void IrTransceiver_Store_ch1(uint8_t id, uint8_t status)
{
	IrTransceiver_Store(1);
}
void IrTransceiver_Store_ch2(uint8_t id, uint8_t status)
{
	IrTransceiver_Store(2);
}

void IrTransceiver_Store(uint8_t channel)
{
	static uint16_t prev_time[3];
	uint16_t pulsewidth;

	/* Read the timer counter register to get the current "time". */
	uint16_t time = IR_COUNT_REG;
	
	/* Subtract the current measurement from the previous to get the pulse width. */
	pulsewidth = time - prev_time[channel];
	prev_time[channel] = time;

	if (drvIrRxChannel[channel].storeEnable)
	{
		/* Store the measurement. */
		drvIrRxChannel[channel].rxbuf[drvIrRxChannel[channel].rxlen++] = pulsewidth;

		/* Disable future measurements if we've filled the buffer. */
		//TODO: What to do in multi recevier?
		//TODO: Report overflow to application
		if (drvIrRxChannel[channel].rxlen == MAX_NR_TIMES)
		{
			drvIrRxChannel[channel].rxlen = MAX_NR_TIMES-1;
		}
	}
	else if (drvIrRxChannel[channel].enable == TRUE)
	{
		/* The first edge of the pulse train has been detected. Enable the storage of the following pulsewidths. */
		drvIrRxChannel[channel].storeEnable = TRUE;
		drvIrRxChannel[channel].rxlen = 0;
		drvIrRxChannel[channel].timeoutEnable = TRUE;

		/* Set the timeout for detection of the end of the pulse train. */
		drvIrRxChannel[channel].timeout = time + IR_MAX_PULSE_WIDTH;
		uint16_t nextTimeout = drvIrRxChannel[channel].timeout;
		for (uint8_t i=0; i < 3; i++)
		{
	//TODO: hitta den med timeout härnäst (svårt med tanke på att den tiden kan ha passerat eftersom vi är under interrupt)
	//kanske inte är så noga, i värsta fall tar det 65ms extra
			if ((drvIrRxChannel[channel].timeoutEnable) && (drvIrRxChannel[i].timeout < nextTimeout))
			{
				nextTimeout = drvIrRxChannel[i].timeout;
			}
		}
		IR_TIMEOUT_REG = nextTimeout;

		/* Enable timeout interrupt for detection of the end of the pulse train. */
		IR_UNMASK_TIMEOUT();

		//TODO: if buffer resource coordinator is implemented this is where a buffer should be assinged to this channel
	}
}
#endif


/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

/* call this function like this:
	IrTransceiver_Init_TX_Channel(channelnumber, buffer, callback, pcint-id, GPIO_D6);
	buffer is a memory location 
	callback is a function to call when a receive/transmitt is complete
	also call with IO port to use	
*/

#if IR_RX_ENABLE==1
void IrTransceiver_Init_TX_Channel(uint8_t channel, uint16_t *buffer, irCallback_t callback, uint8_t pcint_id, volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr, uint8_t pcint)
{
	if (channel == 0)
	{
		Pcint_SetCallback(pcint_id, pcint, &IrTransceiver_Store_ch0);
	}
	else if (channel == 1)
	{
		Pcint_SetCallback(pcint_id, pcint, &IrTransceiver_Store_ch1);
	}
	else if (channel == 2)
	{
		Pcint_SetCallback(pcint_id, pcint, &IrTransceiver_Store_ch2);
	}
	
	if (channel < 3)
	{
		/* Set port direction to input */
		*ddr &= ~(1 << nr);
		
		drvIrRxChannel[channel].rxbuf = buffer;
		drvIrRxChannel[channel].rxlen = 0;
		drvIrRxChannel[channel].storeEnable = FALSE;
		drvIrRxChannel[channel].timeoutEnable = FALSE;
		drvIrRxChannel[channel].callback = callback;
		drvIrRxChannel[channel].enable = TRUE;
	}
}

/* call this function like this:
	IrTransceiver_DeInit_TX_Channel(channelnumber, pcint-id, GPIO_D6);
*/

void IrTransceiver_DeInit_TX_Channel(uint8_t channel, uint8_t pcint_id, volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr, uint8_t pcint)
{
	/* Deactivate interrupt */
	Pcint_SetCallback(pcint_id, pcint, 0);
	
	if (channel < 3)
	{
		/* Deactivate channel */
		drvIrRxChannel[channel].enable = FALSE;
		drvIrRxChannel[channel].rxlen = 0;
		drvIrRxChannel[channel].storeEnable = FALSE;
		drvIrRxChannel[channel].timeoutEnable = FALSE;
		drvIrRxChannel[channel].callback = 0;
	}
}

uint8_t IrTransceiver_GetStoreEnable(uint8_t channel)
{
	return drvIrRxChannel[channel].storeEnable;
}

void IrTransceiver_Disable(uint8_t channel)
{
	if (channel < 3)
	{
		/* Soft disable channel */
		drvIrRxChannel[channel].enable = FALSE;
	}
}
void IrTransceiver_Enable(uint8_t channel)
{
	if (channel < 3)
	{
		/* Soft enable channel */
		drvIrRxChannel[channel].enable = TRUE;
	}
}
void IrTransceiver_Reset(uint8_t channel)
{
	if (channel < 3)
	{
		/* Reset channel */
		drvIrRxChannel[channel].rxlen = 0;
		drvIrRxChannel[channel].storeEnable = FALSE;
		drvIrRxChannel[channel].timeoutEnable = FALSE;
	}
}

#endif

/*
void IrTransceiver_Init_RX_Channel(uint8_t channel, uint16_t *buffer, irCallback_t callback, volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr, uint8_t pcint)
{
	
}
*/

void IrTransceiver_Init(void)
{
	for (uint8_t i = 0; i < 3; i++)
	{
		drvIrRxChannel[i].storeEnable = FALSE;
		drvIrRxChannel[i].timeoutEnable = FALSE;
		drvIrRxChannel[i].rxlen = 0;
	}

	IR_TIMER_INIT();
		
#if IR_TX_ENABLE==1
	/* Set up transmitter */
	IR_T_DDR |= (1<<IR_T_BIT);
	#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__)
	TCCR0A = (0<<COM0A1)|(0<<COM0A0)|(1<<WGM01)|(0<<WGM00);
	TCCR0B = (0<<WGM02)|(1<<CS00)|(0<<CS01)|(0<<CS02);
	#endif
	
#if IR_TX_ACTIVE_LOW==1
	/* when pwm is disconnected from port (during low) the port should output 5V */
	IR_T_PORT |= (1<<IR_T_BIT);
#else
	/* when pwm is disconnected from port (during low) the port should output 0V */
	IR_T_PORT &= ~(1<<IR_T_BIT);
#endif

#endif

}

uint8_t IrTransceiver_Transmit_Poll(void) {
#if IR_TX_ENABLE==1
	if (data_transmitted == 0) {
		return IR_NOT_FINISHED;
	} else {
		return IR_OK;
	}
#else
	return 0;
#endif
}

uint8_t IrTransceiver_Transmit(uint16_t *buffer, uint8_t length, uint8_t modfreq)
{
#if IR_TX_ENABLE==1
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
#else
	return 0;
#endif
}
