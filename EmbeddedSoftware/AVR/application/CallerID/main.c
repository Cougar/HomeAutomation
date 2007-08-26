#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
#include <drivers/uart/serial.h>
#include <drivers/callerid/mt8870/mt8870.h>

#define STATE_IDLE			0
#define STATE_GET_DATA		1
#define STATE_START_WAIT	2
#define STATE_WAIT			3
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

int main(void)
{
	// Enable interrupts as early as possible
	sei();
	
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
	
	DTMFin_Init();

	uint8_t rxbuffer[MAX_TONES];
    uint8_t state=0;
    uint8_t rxlen=0;

	while (1) {
		
		if (state == STATE_IDLE) {
			DTMFin_Start(rxbuffer);
			state = STATE_START_WAIT;
		} else if (state == STATE_START_WAIT) {
			state = STATE_WAIT;
		} else if (state == STATE_WAIT) {
			uint8_t retval = DTMFin_Poll(&rxlen);
			if (retval == RET_FINISHED) {
				state = STATE_STOP;
			} else if (retval == RET_OVERFLOW) {
				state = STATE_IDLE;
			}
		} else if (state == STATE_STOP) {
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
			state = STATE_IDLE;
		}
	}
	
	return 0;
}
