#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
#include <drivers/timer/timer.h>
#include <drivers/adc/adc.h>

#define APP_TYPE    0xf001
#define APP_VERSION 0x0002


#define R51	12000
#define R50	47000

#if (5 * (R51 + R50))/(R51) > 64
	#error "ADC_FACTOR must be less then 64, change R51 and R52"
#else
	#define ADC_FACTOR	(5 * (R51 + R50))/(R51)
	#define ADC_SCALE	10
#endif

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

// Timer callback function used for some timer tests
void timer_callback(uint8_t timer) {
}

int main(void)
{
	// Enable interrupts as early as possible
	sei();
	
	Timer_Init();
	ADC_Init();
	
	unsigned long time;
	
	Can_Message_t txMsg;
	txMsg.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) | (NODE_ID << CAN_SHIFT_NMT_SID);
	txMsg.DataLength = 4;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.Data.words[0] = APP_TYPE;
	txMsg.Data.words[1] = APP_VERSION;
	
	uint16_t reg5Vfeedback;
	uint16_t currentfeedback;
	uint8_t DUTconnected=0;
	uint8_t DUTconnectcnt=0;

	// Set up callback for CAN reception, this is optional if only sending is required.
	BIOS_CanCallback = &can_receive;
	// Send CAN_NMT_APP_START
	BIOS_CanSend(&txMsg);
	
	// Set up three timers (assume at least three has been defined)
	// The timeout is specified in ticks, which is equal to ms if
	// the tick frequency is set to 1000.
	Timer_SetTimeout(0, 3000, TimerTypeFreeRunning, 0);
	Timer_SetTimeout(1, 100, TimerTypeFreeRunning, 0);
	DDRB |= (1<<PB7);
	
	PORTD &= ~(1<<PD7);//turn off output
	DDRD |= (1<<PD7);
	DDRC &= ~(1<<PC2);		//set EXP_N to input
	PORTC |= (1<<PC2);		//set EXP_N to pullup

	while (1) {
		if (Timer_Expired(1)) {
			if (!(PINC & (1<<PC2))) {
				if (DUTconnectcnt < 10) {
					DUTconnectcnt++;
				}
			} else {
				DUTconnected = 0;
				//DUTconnectcnt --;
				DUTconnectcnt = 0;
			}
			if (DUTconnectcnt == 10) {
				DUTconnected = 1;
			} //else if (DUTconnectcnt == 0) {
				//DUTconnected = 0;
			//}

			reg5Vfeedback = ADC_Get(ADREG5VFEEDBACK);
			reg5Vfeedback = (reg5Vfeedback & 0x03ff) * ADC_FACTOR;	//get voltage in mV (typical 5000mV)
			currentfeedback = ADC_Get(ADCURRENTFEEDBACK);
			currentfeedback = (currentfeedback>>1);		//get current in mA (  cur [A] = (ad*Vcc /1024)/R, R=10  ) (typical 20mA)
			
			if (reg5Vfeedback > 6000 || currentfeedback > 40) {
				PORTD &= ~(1<<PD7);		//turn off output
			}
		}
		if (Timer_Expired(0)) {
			/*reg5Vfeedback = ADC_Get(ADREG5VFEEDBACK);
			reg5Vfeedback = (reg5Vfeedback & 0x03ff) * ADC_FACTOR;	//get voltage in mV (typical 5000mV)
			currentfeedback = ADC_Get(ADCURRENTFEEDBACK);
			currentfeedback = (currentfeedback>>1);		//get current in mA (  cur [A] = (ad*Vcc /1024)/R, R=10  ) (typical 20mA)
*/
			//PORTB ^= (1<<PB7);
			//PORTD ^= (1<<PD7);	//toggle output
			
/*if (DUTconnected) {
			PORTD |= (1<<PD7);	//toggle output
}*/

			txMsg.Id = 0;
			txMsg.DataLength = 6;
			txMsg.RemoteFlag = 0;
			txMsg.ExtendedFlag = 1;
			txMsg.Data.bytes[0] = (reg5Vfeedback>>8)&0xff;
			txMsg.Data.bytes[1] = reg5Vfeedback&0xff;
			txMsg.Data.bytes[2] = (currentfeedback>>8)&0xff;
			txMsg.Data.bytes[3] = currentfeedback&0xff;

			txMsg.Data.bytes[4] = DUTconnected;
			txMsg.Data.bytes[5] = ((PORTD & (1<<PD7))>>PD7);
	
			// Send CAN_NMT_APP_START
			BIOS_CanSend(&txMsg);
		}
		
		if (rxMsgFull) {

    		rxMsgFull = 0; //  
		}
	}
	
	return 0;
}
