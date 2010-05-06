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
#include <drivers/DTMF/dtmf.h>
#include <drivers/DTMF/mt8870/mt8870.h>
#include <config.h>

/*-----------------------------------------------------------------------------
 * Globals
 *---------------------------------------------------------------------------*/
uint8_t mt_rxbuf[MT_MAX_TONES];
uint8_t mt_rxlen;

uint8_t mt_data_overflow;

/*-----------------------------------------------------------------------------
 * Prerequisites
 *---------------------------------------------------------------------------*/

#if defined(__AVR_ATmega8__)
#define INT0_REG GICR
#define INT0_CTRL MCUCR
#elif defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__)
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
	uint8_t mt_datain;
	
	/* Read data on input pins */
	mt_datain = ((MT_Q1_PIN >> MT_Q1_BIT) & 0x1);
	mt_datain |= (((MT_Q2_PIN >> MT_Q2_BIT) & 0x1)<<1);
	mt_datain |= (((MT_Q3_PIN >> MT_Q3_BIT) & 0x1)<<2);
	mt_datain |= (((MT_Q4_PIN >> MT_Q4_BIT) & 0x1)<<3);
	
	/* Parse read data and place in buffer */
	switch(mt_datain) {
	case 0:		//DTMF0  = D
		mt_rxbuf[mt_rxlen++] = 0xd;
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
		mt_rxbuf[mt_rxlen++] = mt_datain;
		break;
	case 0xa:	//DTMF10 = number 0
		mt_rxbuf[mt_rxlen++] = 0;
		break;
	case 0xb:	//DTMF11 = *
		mt_rxbuf[mt_rxlen++] = 0xe;
		break;
	case 0xc:	//DTMF12 = #
		mt_rxbuf[mt_rxlen++] = 0xf;
		break;
	case 0xd:	//DTMF13 = A
		mt_rxbuf[mt_rxlen++] = 0xa;
		break;
	case 0xe:	//DTMF14 = B
		mt_rxbuf[mt_rxlen++] = 0xb;
		break;
	case 0xf:	//DTMF15 = C
		mt_rxbuf[mt_rxlen++] = 0xc;
		break;
	}
	
	if (mt_rxlen >= MT_MAX_TONES) {
		mt_data_overflow = 1;
		mt_rxlen=0;
	}
}


/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

DTMF_Ret_t mt8870_Pop(uint8_t *buffer, uint8_t *len) {
	DTMF_Ret_t mt_returndata = DTMF_Ret_No_data;
	*len = 0;
	if (mt_rxlen > 0) {
		cli();
		*len = mt_rxlen;
		for (uint8_t i=0; i<*len; i++) {
			buffer[i] = mt_rxbuf[i];
		}
		mt_rxlen = 0;
		sei();
		if (mt_data_overflow) {
			mt_data_overflow = 0;
			mt_returndata = DTMF_Ret_Overflow;
		} else {
			mt_returndata = DTMF_Ret_Data_avail;
		}
	}
	return mt_returndata;
}

void mt8870_Init(void) {
	/* set up interrupt on rising edge */
	INT0_CTRL |= _BV(ISC01);
	INT0_CTRL |= _BV(ISC00);
	/* set ports to input */
	MT_StD_DDR &= ~(1<<MT_StD_BIT);
	MT_Q1_DDR &= ~(1<<MT_Q1_BIT);
	MT_Q2_DDR &= ~(1<<MT_Q2_BIT);
	MT_Q3_DDR &= ~(1<<MT_Q3_BIT);
	MT_Q4_DDR &= ~(1<<MT_Q4_BIT);
	
	mt_rxlen = 0;
	mt_data_overflow = 0;
	MT8870_INT_ENABLE();
}

