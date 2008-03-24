/**
 * CanSensors. 
 *
 *
 * @date    2007-10-08
 * @author	Martin Norden
 *
 * TODO: everything, this is release 0.00001. Use /timer.h to read RPM and injectors!
 */

#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
#include <drivers/uart/serial.h>
#include <drivers/timer/timer.h>

#define APP_TYPE    0xf001
#define APP_VERSION 0x0002

// A simple message "queue", with space for one message only.
// These are declared volatile to tell the compiler not to optimize away accesses.
volatile Can_Message_t rxMsg; // Message storage
volatile uint8_t rxMsgFull;   // Synchronization flag

// CAN message reception callback.
// This function runs with interrupts disabled, keep it as short as possible.
void can_receive(Can_Message_t *msg) {
	if (!rxMsgFull) {
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}

uint16_t readvaluez(void);
uint16_t ADValue;
uint8_t activatedSensor;

void readSensor(uint8_t sensortoRead, uint8_t transition);
void setADChannel(uint8_t adcchannel);

#define LOCSENSOR_BOOST		0x00
#define LOCSENSOR_VOLTAGE	0x01

int main(void)
{
	// Enable interrupts as early as possible
	sei();
	
	Timer_Init();
	Serial_Init();
	

	
	
	Can_Message_t txMsg;
	txMsg.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) | (NODE_ID << CAN_SHIFT_NMT_SID);
	txMsg.DataLength = 2;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.Data.words[0] = APP_TYPE;
	txMsg.Data.words[1] = APP_VERSION;
	
	// Set up callback for CAN reception, this is optional if only sending is required.
	BIOS_CanCallback = &can_receive;
	// Send CAN_NMT_APP_START
	BIOS_CanSend(&txMsg);

	/* Enable AVcc as Voltage Reference */
	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1<<REFS1);

	/* Right adjust the result */
	ADMUX &= ~(1<<ADLAR);

	/* Wake up ADC and enable it */
	PRR &= ~(1<<PRADC);
	ADCSRA |= (1<<ADEN);
	
	setADChannel(4);
	

	while (1) {
		//Check if one second has passed, in that case: read the next sensor that is not activated
		
		//Read activated sensor and send value to el displayo
		switch (activatedSensor){
		case LOCSENSOR_BOOST:
			ADValue = readvaluez()/8;
			txMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_PRESSURE << CAN_SHIFT_SNS_TYPE) | (NODE_ID << CAN_SHIFT_SNS_SID));
			txMsg.Id |= (1 << CAN_SHIFT_SNS_ID); /* Set sensor id */
			
			txMsg.Data.bytes[0] = ADValue/255;
			txMsg.Data.bytes[1] = ADValue%255;
			
			BIOS_CanSend(&txMsg);
			
		break;
		}		
		
		if (rxMsgFull) {
			// Ooh! We have a message!

			//Message setting active sensor? Let's change the ad-channel!
			
			
    		rxMsgFull = 0; //  delete it!
		}
	}
	
	return 0;
}

void setADChannel(uint8_t adcchannel){	
	switch(adcchannel){
	case 0:
		/* Enable ADC0 */
		ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2)|(1<<MUX3));
	break;
	case 1:
		/* Enable ADC1 */
		ADMUX |= (1<<MUX0);
		ADMUX &= ~((1<<MUX1)|(1<<MUX2)|(1<<MUX3));
	break;
	case 2:
		/* Enable ADC2 */
		ADMUX |= (1<<MUX1);
		ADMUX &= ~((1<<MUX0)|(1<<MUX2)|(1<<MUX3));
	break;
	case 3:
		/* Enable ADC3 */
		ADMUX |= (1<<MUX0)|(1<<MUX1);
		ADMUX &= ~((1<<MUX2)|(1<<MUX3));
	break;
	case 4:
		/* Enable ADC4 */
		ADMUX |= (1<<MUX2);
		ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX3));
	break;
	case 5:
		/* Enable ADC5 */
		ADMUX |= (1<<MUX0)|(1<<MUX2);
		ADMUX &= ~((1<<MUX1)|(1<<MUX3));
	break;
	case 6:
		/* Enable ADC6 */
		ADMUX |= (1<<MUX1)|(1<<MUX2);
		ADMUX &= ~((1<<MUX0)|(1<<MUX3));
	break;
	case 7:
		/* Enable ADC7 (only for TQFP package) */
		ADMUX |= (1<<MUX0)|(1<<MUX1)|(1<<MUX2);
		ADMUX &= ~(1<<MUX3);
	break;
	}	
	/* Make the first conversion (takes 25 ADC clock cycles) and throw away */
	readvaluez();
}

uint16_t readvaluez(void)
{
	/* Start measurement (takes 13 ADC clock cycles) */
	ADCSRA |= (1<<ADSC);

	while( ADCSRA & (1<<ADSC) ){ /* Wait for conversion to complete */ }

	/* Get the result, return */
	return ADCW;
}

