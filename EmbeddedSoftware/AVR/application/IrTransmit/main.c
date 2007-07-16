#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
//#include <drivers/uart/serial.h>
#include <drivers/timer/timebase.h>
#include <drivers/ir/transmitter/irtransmitter.h>

#define APP_TYPE    CAN_APPTYPES_IRTRANSMITTER
#define APP_VERSION 0x0001

#define STATE_IDLE			0
#define STATE_REPEAT		1
#define STATE_STOPPING		2

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
	//Serial_Init();
	IrTransmit_Init();
	
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

	//txMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_IR << CAN_SHIFT_SNS_TYPE) | (NODE_ID << CAN_SHIFT_SNS_SID));
	
	//unsigned long time = Timebase_CurrentTime();
	//unsigned long irTimeoutTime = 100;
	
	//uint8_t ir_protocol, ir_address, ir_command;
	//uint16_t ir_timeout;
	uint8_t state = STATE_IDLE;

	while (1) {
		//time = Timebase_CurrentTime();
		if (state == STATE_IDLE) {
		} else if (state == STATE_REPEAT) {
			IrTransmit_Poll();
		} else if (state == STATE_STOPPING) {
			IrTransmit_Poll();
			if (IrTransmit_Stop() == IR_OK) {
				state = STATE_IDLE;
			}
		}

		
		if (rxMsgFull) {
			if ( ((rxMsg.Id & CAN_MASK_CLASS)>>CAN_SHIFT_CLASS) == CAN_ACT) {
				uint16_t acttype =(uint16_t)((rxMsg.Id & CAN_MASK_ACT_TYPE) >> CAN_SHIFT_ACT_TYPE);
				uint8_t actid = (uint8_t)((rxMsg.Id & CAN_MASK_ACT_ID) >> CAN_SHIFT_ACT_ID);
				
				if (rxMsg.DataLength==4) {
					if (acttype == ACT_TYPE_IR) {
						if (state == STATE_IDLE && rxMsg.Data.bytes[0] == IR_BUTTON_DOWN) {
							if (IrTransmit_Start(rxMsg.Data.bytes[1], rxMsg.Data.bytes[2], rxMsg.Data.bytes[3]) == IR_OK) {
								state = STATE_REPEAT;
							}
						} else if (state == STATE_REPEAT && rxMsg.Data.bytes[0] == IR_BUTTON_UP) {
							state = STATE_STOPPING;
						}
					}
				}
			}
			
			// Flush the received message
    		rxMsgFull = 0; //  
		}
	}
	
	return 0;
}
