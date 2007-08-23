#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
#include <drivers/uart/serial.h>
#include <drivers/timer/timer.h>

/*
 * De få signaler som man behöver koppla in från MT8870 är
 * StD, Q1, Q2, Q3 och Q4
 */

#define MT_StD_PIN      PIND
#define MT_StD_DDR      DDRD
#define MT_StD_BIT      PD2

#define MT_Q1_PIN       PINC
#define MT_Q1_DDR       DDRC
#define MT_Q1_BIT       PC0

#define MT_Q2_PIN       PINC
#define MT_Q2_DDR       DDRC
#define MT_Q2_BIT       PC1

#define MT_Q3_PIN       PINC
#define MT_Q3_DDR       DDRC
#define MT_Q3_BIT       PC2

#define MT_Q4_PIN       PINC
#define MT_Q4_DDR       DDRC
#define MT_Q4_BIT       PC3

#define DIAL_TIMEOUT	1800



#define STATE_IDLE			0
#define STATE_GET_DATA		1
#define STATE_WAIT1			2
#define STATE_WAIT2			3
#define STATE_STOP			4

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

// Timer callback function used for some timer tests
void timer_callback(uint8_t timer) {
	Can_Message_t msg;
	
	msg.ExtendedFlag = 1;
	msg.Id = (CAN_TST << CAN_SHIFT_CLASS) | NODE_ID;
	msg.RemoteFlag = 0;
	msg.DataLength = 1;
	msg.Data.bytes[0] = timer;
	
	BIOS_CanSend(&msg);
}

int main(void)
{
	// Enable interrupts as early as possible
	sei();
	
	Timer_Init();
	Serial_Init();
	
	Can_Message_t txMsg;
	txMsg.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) | (NODE_ID << CAN_SHIFT_NMT_SID);
	txMsg.DataLength = 4;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.Data.words[0] = APP_TYPE;
	txMsg.Data.words[1] = APP_VERSION;
	
	// Set up callback for CAN reception, this is optional if only sending is required.
	BIOS_CanCallback = &can_receive;
	// Send CAN_NMT_APP_START
	BIOS_CanSend(&txMsg);
	
	//set ports to input
	MT_StD_DDR &= ~(1<<MT_StD_BIT);
	MT_Q1_DDR &= ~(1<<MT_Q1_BIT);
	MT_Q2_DDR &= ~(1<<MT_Q2_BIT);
	MT_Q3_DDR &= ~(1<<MT_Q3_BIT);
	MT_Q4_DDR &= ~(1<<MT_Q4_BIT);

	
	printf("AVR Test Application\n");
	
	txMsg.Id = (CAN_TST << CAN_SHIFT_CLASS) | NODE_ID;
	txMsg.Data.dwords[0] = 0x01020304;
	txMsg.DataLength = 8;

	uint8_t receviercounter = 0;
    uint8_t datain = 0;
    uint8_t state=0;

	while (1) {
		
		if (state == STATE_IDLE) {
			if ((MT_StD_PIN & (1<<MT_StD_BIT)) != 0) {
				state = STATE_GET_DATA;
			}
		} else if (state == STATE_GET_DATA) {
			//nu finns data tillgänglig på Qx
			datain = ((MT_Q1_PIN >> MT_Q1_BIT) & 0x1);
			datain |= (((MT_Q2_PIN >> MT_Q2_BIT) & 0x1)<<1);
			datain |= (((MT_Q3_PIN >> MT_Q3_BIT) & 0x1)<<2);
			datain |= (((MT_Q4_PIN >> MT_Q4_BIT) & 0x1)<<3);
			if (datain == 0) { datain = 0x1d; }		//DTMF0  = D    //this is temporary
			if (datain == 11) { datain = 0x1e; }	//DTMF12 = *    //this is temporary
			if (datain == 12) { datain = 0x1f; }	//DTMF11 = #    //this is temporary
			if (datain == 15) { datain = 0xc; }		//DTMF11 = C
			if (datain == 10) { datain = 0; }		//DTMF10 = number 0
			if (datain == 13) { datain = 0xa; }		//DTMF13 = A
			if (datain == 14) { datain = 0xb; }		//DTMF14 = B
			if (datain == 0x1d) { datain = 0xd; }	//DTMF0  = D    //now permanent
			if (datain == 0x1e) { datain = 0xe; }	//DTMF12 = *    //now permanent
			if (datain == 0x1f) { datain = 0xf; }	//DTMF11 = #    //now permanent
		
			if ((receviercounter & 0x1) == 0) {
				txMsg.Data.bytes[(receviercounter>>1)] = (datain<<4);
			} else {
				txMsg.Data.bytes[(receviercounter>>1)] |= datain;
			}
			receviercounter++;

			state = STATE_WAIT1;
		} else if (state == STATE_WAIT1) {
			if ((MT_StD_PIN & (1<<MT_StD_BIT)) == 0) {
				Timer_SetTimeout(0, DIAL_TIMEOUT, TimerTypeOneShot, 0);
				state = STATE_WAIT2;
			}	
			
		} else if (state == STATE_WAIT2) {
			if ((MT_StD_PIN & (1<<MT_StD_BIT)) != 0) {
				if (receviercounter != 16) {
					state = STATE_GET_DATA;
				} else {
					state = STATE_WAIT1;
				}
			}
			
			if (Timer_Expired(0)) {
				state = STATE_STOP;
			}
		} else if (state == STATE_STOP) {
			if (datain == 0xc) {    //om sista tonen var c
				if ((receviercounter & 0x1) == 0) {
					txMsg.DataLength = (receviercounter>>1);
				} else {
					txMsg.DataLength = (receviercounter>>1)+1;
					//txMsg.Data.bytes[(receviercounter>>1)] |= 0xc; //spara ett extra c om udda antal
				}
				txMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_PHONENR << CAN_SHIFT_SNS_TYPE) | (0<<CAN_SHIFT_SNS_ID) | (NODE_ID << CAN_SHIFT_SNS_SID));
				BIOS_CanSend(&txMsg);
			}				
			
			receviercounter = 0;
			state = STATE_IDLE;
		}
		
	}
	
	return 0;
}
