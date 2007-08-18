#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.

#include <drivers/timer/timebase.h>
#include <drivers/ir/transceiver/irtransceiver.h>
#include <drivers/ir/protocols.h>

#define APP_TYPE    CAN_APPTYPES_IRRECEIVER
#define APP_VERSION 0x0004

#define STATE_IDLE			0
#define STATE_IR_REPEAT		1
#define STATE_START_RECEIVE	2
#define STATE_RECEIVING		3
#define STATE_START_PAUSE	4
#define STATE_PAUSING		5

#define IR_ID_DATA	0
#define IR_ID_RAW	1
#define IR_ID_DEBUG	10

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
	
	Timebase_Init();
	IrTransceiver_Init();
	
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
	
	uint32_t time = Timebase_CurrentTime();
	uint32_t timePauseStarted = 0;
	
	uint8_t state = STATE_IDLE;
	
	Ir_Protocol_Data_t proto;
	proto.timeout=0; proto.data=0; proto.repeats=0; proto.protocol=0;
	uint8_t len=0;
	uint16_t rxbuffer[MAX_NR_TIMES];
	
	while (1) {
		if (state == STATE_IDLE) {
			IrTransceiver_Receive_Start(rxbuffer);
			state = STATE_START_RECEIVE;
		} else if (state == STATE_START_RECEIVE) {
			state = STATE_RECEIVING;
		} else if (state == STATE_RECEIVING) {
			len = IrTransceiver_Receive_Poll();
			if (len > 0) {
				//låt protocols parsa och skicka på can
				if (parseProtocol(rxbuffer, len, &proto) == IR_OK) {
					if (proto.protocol != IR_PROTO_UNKNOWN) {
						txMsg.Data.bytes[0] = IR_BUTTON_DOWN;
						txMsg.Data.bytes[1] = proto.protocol;
						txMsg.Data.bytes[2] = (proto.data>>24)&0xff;
						txMsg.Data.bytes[3] = (proto.data>>16)&0xff;
						txMsg.Data.bytes[4] = (proto.data>>8)&0xff;
						txMsg.Data.bytes[5] = proto.data&0xff;
						txMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_IR << CAN_SHIFT_SNS_TYPE) | (IR_ID_DATA<<CAN_SHIFT_SNS_ID) | (NODE_ID << CAN_SHIFT_SNS_SID));
						txMsg.DataLength = 6;
						BIOS_CanSend(&txMsg);
					}
				}
				
				state = STATE_START_PAUSE;
			}
		} else if (state == STATE_START_PAUSE) {
			timePauseStarted = Timebase_CurrentTime();
			state = STATE_PAUSING;
		} else if (state == STATE_PAUSING) {
			//resetta timebase-var om ir finns, detta bör göras snyggare
			for (uint8_t i=0; i<10; i++) {
				if (IrTransceiver_Receive_Pause_Poll() == 0) {
					break;
				} 
				if (i==9) {
					timePauseStarted = Timebase_CurrentTime();
				}
			}
			
			time = Timebase_CurrentTime();
			if (time - timePauseStarted >= proto.timeout) {
				if (proto.protocol != IR_PROTO_UNKNOWN) {
					//skicka på can
					txMsg.Data.bytes[0] = IR_BUTTON_UP;
					BIOS_CanSend(&txMsg);
				}
				state = STATE_IDLE;
			}
		}
		
		
		if (rxMsgFull) {
			/* No message reception functionality implemented */
			/* Flush the received message */
    		rxMsgFull = 0; //  
		}
	}
	return 0;
}
