#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
#include <drivers/DTMF/dtmf.h>
//#include <drivers/DTMF/mt8870/mt8870.h>
#include <drivers/timer/timer.h>

#define STATE_IDLE			0
#define STATE_COPYDATA		1
#define STATE_WAIT			2
#define STATE_SEND			3

#define APP_TYPE    0xf001
#define APP_VERSION 0x0003

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

int main(void)
{
	// Enable interrupts as early as possible
	sei();
	
	Timer_Init();

	DTMFin_Init();

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
	

	uint8_t pollrxbuffer[MT_MAX_TONES];
    uint8_t pollrxlen=0;
	uint8_t rxbuffer[MT_MAX_TONES];
    uint8_t rxlen=0;
    uint8_t state=STATE_IDLE;

	while (1) {
		if (state == STATE_IDLE) {
			if (DTMFin_Pop(pollrxbuffer, &pollrxlen) == DTMF_Ret_Data_avail) {
				state = STATE_COPYDATA;
			} else {
				//go into sleep or something, incoming data from mt8870 is interrupted so we will wake up
			}
		} else if (state == STATE_COPYDATA) { 
			state = STATE_WAIT;
			for (uint8_t i=0; i<pollrxlen;i++) {
				rxbuffer[rxlen++] = pollrxbuffer[i];
				if (rxlen >= MT_MAX_TONES) {
					//overflow
					rxlen = 0;
					state = STATE_IDLE;
					break;
				}
			}
			Timer_SetTimeout(0, MT_DIAL_TIMEOUT, TimerTypeOneShot, 0);
		} else if (state == STATE_WAIT) {
			if (Timer_Expired(0)) {
				state = STATE_SEND;
			}
			
			DTMF_Ret_t retval = DTMFin_Pop(pollrxbuffer, &pollrxlen);
			if (retval == DTMF_Ret_Data_avail) {
				state = STATE_COPYDATA;
			} else if (retval == DTMF_Ret_Overflow) {
				rxlen=0;
				state = STATE_IDLE;
			}
		} else if (state == STATE_SEND) {
			//if ((rxbuffer[0] == 0xa || rxbuffer[0] == 0xb) && rxbuffer[rxlen-1] == 0xc) {
			for (uint8_t i = 0; i < 16; i=i+2) {
				txMsg.Data.bytes[i>>1] = rxbuffer[i]<<4;
				if (i+1 < rxlen) {
					txMsg.Data.bytes[i>>1] |= rxbuffer[i+1];
				}
			}
			txMsg.DataLength = (rxlen>>1)+(rxlen&1);
			txMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_PHONENR << CAN_SHIFT_SNS_TYPE) | (0<<CAN_SHIFT_SNS_ID) | (NODE_ID << CAN_SHIFT_SNS_SID));
			BIOS_CanSend(&txMsg);
	
			//}
			rxlen = 0;
			state = STATE_IDLE;
		}
	}
	
	return 0;
}
