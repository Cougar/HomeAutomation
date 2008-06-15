/**
 * Mitel DTMF receiver driver.
 * 
 * @date	2007-08-24
 * 
 * @author	Anders Runeson
 *   
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
//#include <config.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <drivers/timer/timer.h>
#include <drivers/DTMF/mt8870/mt8870.h>
#include <config.h>

/*-----------------------------------------------------------------------------
 * Globals
 *---------------------------------------------------------------------------*/
uint8_t *rxbuf;
uint8_t rxlen;

uint8_t data_overflow;
uint8_t data_received;

/*-----------------------------------------------------------------------------
 * Prerequisites
 *---------------------------------------------------------------------------*/

#if defined(__AVR_ATmega8__)
#define INT0_REG GICR
#define INT0_CTRL MCUCR
#elif defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__)
#define INT0_REG EIMSK
#define INT0_CTRL EICRA
#else
#error AVR device not supported!
#endif

#define MT8870_INT_VECTOR		INT0_vect
#define MT8870_INT_ENABLE()		INT0_REG |= _BV(INT0)
#define MT8870_INT_DISABLE()	INT0_REG &= ~(_BV(INT0))

/*-----------------------------------------------------------------------------
 * Interrupt Handlers
 *---------------------------------------------------------------------------*/

ISR(MT8870_INT_VECTOR) {
	/* External interrupt 0, rising edge detected */
	uint8_t datain;
	
	/* Read data on input pins */
	datain = ((MT_Q1_PIN >> MT_Q1_BIT) & 0x1);
	datain |= (((MT_Q2_PIN >> MT_Q2_BIT) & 0x1)<<1);
	datain |= (((MT_Q3_PIN >> MT_Q3_BIT) & 0x1)<<2);
	datain |= (((MT_Q4_PIN >> MT_Q4_BIT) & 0x1)<<3);
	
	/* Parse read data and place in buffer */
	switch(datain) {
	case 0:		//DTMF0  = D
		rxbuf[rxlen++] = 0xd;
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
		rxbuf[rxlen++] = datain;
		break;
	case 0xa:	//DTMF10 = number 0
		rxbuf[rxlen++] = 0;
		break;
	case 0xb:	//DTMF11 = *
		rxbuf[rxlen++] = 0xe;
		break;
	case 0xc:	//DTMF12 = #
		rxbuf[rxlen++] = 0xf;
		break;
	case 0xd:	//DTMF13 = A
		rxbuf[rxlen++] = 0xa;
		break;
	case 0xe:	//DTMF14 = B
		rxbuf[rxlen++] = 0xb;
		break;
	case 0xf:	//DTMF15 = C
		rxbuf[rxlen++] = 0xc;
		break;
	}
	
	/* Set/reset timer for dial timeout */
	Timer_SetTimeout(MT_TIMER, MT_DIAL_TIMEOUT, TimerTypeOneShot, &DTMFin_timer_callback);
	
	if (rxlen == MT_MAX_TONES) {
		data_overflow = 1;
		MT8870_INT_DISABLE();
	}
}


/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

// Timer callback function 
void DTMFin_timer_callback(uint8_t timer) {
	data_received = 1;
	MT8870_INT_DISABLE();
}

void DTMFin_Start(uint8_t *buffer) {
	rxbuf = buffer;
	rxlen = 0;
	data_overflow = 0;
	data_received = 0;
	
	MT8870_INT_ENABLE();
}

MT8870_Ret_t DTMFin_Poll(uint8_t *len) {
	*len = rxlen;
	if (data_overflow) {
		return MT8870_Ret_Overflow;
	}
	if (data_received) {
		return MT8870_Ret_Finished;
	}
	return MT8870_Ret_Not_Finished;
}

void DTMFin_Init(void) {
	/* set up interrupt on rising edge */
	INT0_CTRL |= _BV(ISC01);
	INT0_CTRL |= _BV(ISC00);
	/* set ports to input */
	MT_StD_DDR &= ~(1<<MT_StD_BIT);
	MT_Q1_DDR &= ~(1<<MT_Q1_BIT);
	MT_Q2_DDR &= ~(1<<MT_Q2_BIT);
	MT_Q3_DDR &= ~(1<<MT_Q3_BIT);
	MT_Q4_DDR &= ~(1<<MT_Q4_BIT);
}

