#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
//#include <drivers/uart/serial.h>
#include <drivers/timer/timebase.h>
#include <drivers/ir/receiver/irreceiver.h>

#define APP_TYPE    CAN_APPTYPES_IRRECEIVER
#define APP_VERSION 0x0003

#define STATE_IDLE		0
#define STATE_IR_REPEAT	1

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
	//Serial_Init();
	IrReceive_Init();
	
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

	//txMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_IR << CAN_SHIFT_SNS_TYPE) | (IR_ID_DATA<<CAN_SHIFT_SNS_ID) | (NODE_ID << CAN_SHIFT_SNS_SID));
	
	unsigned long time = Timebase_CurrentTime();
	unsigned long irTimeoutTime = 100;
	
	uint8_t ir_protocol, ir_address, ir_command;
	uint16_t ir_timeout;
	uint8_t state = STATE_IDLE;

	while (1) {
		#if 0
		time = Timebase_CurrentTime();
		if (state == STATE_IDLE) {
			if (IrReceive_CheckIR(&ir_protocol, &ir_address, &ir_command, &ir_timeout) == IR_OK) {
				txMsg.Data.bytes[0] = 0x00;
				txMsg.Data.bytes[1] = ir_protocol;
				txMsg.Data.bytes[2] = ir_address;
				txMsg.Data.bytes[3] = ir_command;
				txMsg.DataLength = 4;
				txMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_IR << CAN_SHIFT_SNS_TYPE) | (IR_ID_DATA<<CAN_SHIFT_SNS_ID) | (NODE_ID << CAN_SHIFT_SNS_SID));
				BIOS_CanSend(&txMsg);
				irTimeoutTime = Timebase_CurrentTime();
				state = STATE_IR_REPEAT;
			}
		} else if (state == STATE_IR_REPEAT) {
			//kolla efter ny ir, om ingen ny ir och timeout så sätt state till IDLE
			if (time - irTimeoutTime >= ir_timeout) {		
				state = STATE_IDLE;
				txMsg.Data.bytes[0] = 0x0f;
				BIOS_CanSend(&txMsg);
			}
			if (IrReceive_CheckIdle() == IR_OK) {
				irTimeoutTime = Timebase_CurrentTime();
			}
		}
		#endif
		
		#if 1
		time = Timebase_CurrentTime();
		
		if (state == STATE_IDLE) {
			/* test for irdata */
			uint8_t returnval = IrReceive_CheckIR(&ir_protocol, &ir_address, &ir_command, &ir_timeout);
			if (returnval == IR_OK) {
				/* a protocol was found for irdata, send on can */
				txMsg.Data.bytes[0] = 0x00;
				txMsg.Data.bytes[1] = ir_protocol;
				txMsg.Data.bytes[2] = ir_address;
				txMsg.Data.bytes[3] = ir_command;
				txMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_IR << CAN_SHIFT_SNS_TYPE) | (IR_ID_DATA<<CAN_SHIFT_SNS_ID) | (NODE_ID << CAN_SHIFT_SNS_SID));
				txMsg.DataLength = 4;
				BIOS_CanSend(&txMsg);
				
				/* set up timeout */
				irTimeoutTime = Timebase_CurrentTime();
				state = STATE_IR_REPEAT;
				
			} else if (returnval == IR_NO_PROTOCOL) {
				/* the protocol is unknown so the raw ir-data is sent, makes it easier to develop a new protocol */
				ir_protocol = IR_PROTO_UNKNOWN; 	//used in STATE_IR_REPEATE, to know if to send a release
				txMsg.DataLength = 8;
				txMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_IR << CAN_SHIFT_SNS_TYPE) | (IR_ID_RAW<<CAN_SHIFT_SNS_ID) | (NODE_ID << CAN_SHIFT_SNS_SID));
				uint8_t datacnt = getRawDataCnt();
				for (uint8_t i = 0; i < datacnt>>2; i++) {
					uint8_t index = i<<2;
					txMsg.Data.words[0] = getRawData(index);
					txMsg.Data.words[1] = getRawData(index+1);
					txMsg.Data.words[2] = getRawData(index+2);
					txMsg.Data.words[3] = getRawData(index+3);
					
					/* buffers will be filled when sending more than 2-3 messages, so retry until sent */
					while (BIOS_CanSend(&txMsg) != CAN_OK) {}
				}
				
				uint8_t lastpacketcnt = datacnt&0x03;
				if (lastpacketcnt > 0) {
					txMsg.DataLength = lastpacketcnt<<1;
					for (uint8_t i = 0; i < lastpacketcnt; i++) {
						txMsg.Data.words[i] = getRawData((datacnt&0xfc)|i);
					}
					BIOS_CanSend(&txMsg);
				}

				irTimeoutTime = Timebase_CurrentTime();
				state = STATE_IR_REPEAT;
				ir_timeout = 200;			/* Set a default timeout */
			} else if (returnval != IR_NO_DATA) {
				/* debug, send the returned message on errors (except for IR_NO_DATA-errors) */
				txMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_IR << CAN_SHIFT_SNS_TYPE) | (IR_ID_DEBUG<<CAN_SHIFT_SNS_ID) | (NODE_ID << CAN_SHIFT_SNS_SID));
				txMsg.DataLength = 1;
				txMsg.Data.bytes[0] = returnval;
				BIOS_CanSend(&txMsg);
			}

		} else if (state == STATE_IR_REPEAT) {
			/* check for new ir-data, if no new data during the timout-time then set state to IDLE */
			if (time - irTimeoutTime >= ir_timeout) {		
				state = STATE_IDLE;
				/* if the protocol was known then send a release-message */
				if (ir_protocol != IR_PROTO_UNKNOWN) {
					txMsg.Data.bytes[0] = 0x0f;
					BIOS_CanSend(&txMsg);
				}
			}
			if (IrReceive_CheckIdle() == IR_OK) {
				irTimeoutTime = Timebase_CurrentTime();
			}
		}
			
		#endif

		
		if (rxMsgFull) {
			/* No message reception functionality implemented */
			
			/* Flush the received message */
    		rxMsgFull = 0; //  
		}
	}
	
	return 0;
}
