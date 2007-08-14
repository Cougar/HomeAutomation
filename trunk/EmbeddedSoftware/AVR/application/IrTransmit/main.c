#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
#include <drivers/timer/timebase.h>
#include <drivers/ir/transceiver/irtransceiver.h>

#define APP_TYPE    CAN_APPTYPES_IRTRANSMITTER
#define APP_VERSION 0x0001

#define STATE_IDLE				0
#define STATE_START_TRANSMIT	1
#define STATE_TRANSMITTING		2
#define STATE_START_PAUSE		3
#define STATE_PAUSING			4
#define STATE_STOP				5

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
	
	uint8_t state=STATE_IDLE;
	uint8_t repeatcnt=0;
	uint8_t stop=0;
	uint8_t protocol=0; uint32_t ir_data=0; uint16_t timeout=0; uint8_t repeats=0;
	
	while (1) {
		time = Timebase_CurrentTime();
		if (state == STATE_IDLE) {
		} else if (state == STATE_START_TRANSMIT) {
			if (IrTransceiver_Transmit(protocol, ir_data, &timeout, &repeats) == IR_OK) {
				state = STATE_TRANSMITTING;
			} else {
				state = STATE_IDLE;
			}
		} else if (state == STATE_TRANSMITTING) {
			//polla om den är klar, om klar gå till STATE_START_PAUSE
			if (IrTransceiver_Transmit_Poll() != IR_NOT_FINISHED) {
				state = STATE_START_PAUSE;
			}
		} else if (state == STATE_START_PAUSE) {
			if (repeatcnt<repeats) {
				repeatcnt++;
			}
			timePauseStarted = Timebase_CurrentTime();
			state = STATE_PAUSING;
		} else if (state == STATE_PAUSING) {
			//när timeout har gått (timebase) så gå till STATE_START_TRANSMIT
			if (time - timePauseStarted >= timeout) {
				state = STATE_START_TRANSMIT;
			}
			if (stop == 1 && repeatcnt >= repeats) {
				state = STATE_STOP;
			}

		} else if (state == STATE_STOP) {
			stop = 0;
			timeout = 0;
			repeatcnt = 0;
			state = STATE_IDLE;
		}

		
		if (rxMsgFull) {
			if ( ((rxMsg.Id & CAN_MASK_CLASS)>>CAN_SHIFT_CLASS) == CAN_ACT) {
				uint16_t acttype =(uint16_t)((rxMsg.Id & CAN_MASK_ACT_TYPE) >> CAN_SHIFT_ACT_TYPE);
				uint8_t actid = (uint8_t)((rxMsg.Id & CAN_MASK_ACT_ID) >> CAN_SHIFT_ACT_ID);
				
				if (rxMsg.DataLength==6) {
					if (acttype == ACT_TYPE_IR) {
						if (state == STATE_IDLE && rxMsg.Data.bytes[0] == IR_BUTTON_DOWN) {
							protocol = rxMsg.Data.bytes[1];
							ir_data = rxMsg.Data.bytes[5];
							ir_data |= (rxMsg.Data.bytes[4]<<8);
							ir_data |= ((uint32_t)rxMsg.Data.bytes[3]<<16);
							ir_data |= ((uint32_t)rxMsg.Data.bytes[2]<<24);

							state = STATE_START_TRANSMIT;
						} else if (state != STATE_IDLE && rxMsg.Data.bytes[0] == IR_BUTTON_UP) {
							stop = 1;
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
