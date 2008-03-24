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
#define STATE_START_IDLE	6

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

#if (SEND_DEBUG)
void send_debug(uint16_t *buffer, uint8_t len) {
	Can_Message_t txMsg;
	
	/* the protocol is unknown so the raw ir-data is sent, makes it easier to develop a new protocol */
	txMsg.DataLength = 8;
	txMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_IR << CAN_SHIFT_SNS_TYPE) | (IR_ID_RAW<<CAN_SHIFT_SNS_ID) | (NODE_ID << CAN_SHIFT_SNS_SID));
	for (uint8_t i = 0; i < len>>2; i++) {
		uint8_t index = i<<2;
		txMsg.Data.bytes[0] = (buffer[index]>>8)&0xff;
		txMsg.Data.bytes[1] = (buffer[index]>>0)&0xff;
		txMsg.Data.bytes[2] = (buffer[index+1]>>8)&0xff;
		txMsg.Data.bytes[3] = (buffer[index+1]>>0)&0xff;
		txMsg.Data.bytes[4] = (buffer[index+2]>>8)&0xff;
		txMsg.Data.bytes[5] = (buffer[index+2]>>0)&0xff;
		txMsg.Data.bytes[6] = (buffer[index+3]>>8)&0xff;
		txMsg.Data.bytes[7] = (buffer[index+3]>>0)&0xff;
		
		/* buffers will be filled when sending more than 2-3 messages, so retry until sent */
		//while (BIOS_CanSend(&txMsg) != CAN_OK) {}
		BIOS_CanSend(&txMsg);
		
		uint16_t dummycnt = 1;
		while (dummycnt > 0) {
			dummycnt++;
			asm("nop");
			asm("nop");
		}
	}
	
	uint8_t lastpacketcnt = len&0x03;
	if (lastpacketcnt > 0) {
		txMsg.DataLength = lastpacketcnt<<1;
		for (uint8_t i = 0; i < lastpacketcnt; i++) {
			txMsg.Data.bytes[i<<1] = (buffer[(len&0xfc)|i]>>8)&0xff;
			txMsg.Data.bytes[(i<<1)+1] = (buffer[(len&0xfc)|i]>>0)&0xff;
		}
		/* buffers will be filled when sending more than 2-3 messages, so retry until sent */
		//while (BIOS_CanSend(&txMsg) != CAN_OK) {}
		BIOS_CanSend(&txMsg);
	}

}
#endif

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
			uint8_t res = IrTransceiver_Receive_Poll(&len);
			if ((res == IR_OK) && (len > 0)) {
				//låt protocols parsa och skicka på can
				uint8_t res2 = parseProtocol(rxbuffer, len, &proto);
				if (res2 == IR_OK && proto.protocol != IR_PROTO_UNKNOWN) {
					txMsg.Data.bytes[0] = IR_BUTTON_DOWN;
					txMsg.Data.bytes[1] = proto.protocol;
					txMsg.Data.bytes[2] = (proto.data>>24)&0xff;
					txMsg.Data.bytes[3] = (proto.data>>16)&0xff;
					txMsg.Data.bytes[4] = (proto.data>>8)&0xff;
					txMsg.Data.bytes[5] = proto.data&0xff;
					txMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_IR << CAN_SHIFT_SNS_TYPE) | (IR_ID_DATA<<CAN_SHIFT_SNS_ID) | (NODE_ID << CAN_SHIFT_SNS_SID));
					txMsg.DataLength = 6;
					BIOS_CanSend(&txMsg);
				} else if (proto.protocol == IR_PROTO_UNKNOWN) {
#if (SEND_DEBUG)
						send_debug(rxbuffer, len);
						proto.timeout=300;
#endif
					
				}
				
				state = STATE_START_PAUSE;
			}
		} else if (state == STATE_START_PAUSE) {
			IrTransceiver_Receive_Start(rxbuffer);
			timePauseStarted = Timebase_CurrentTime();
			state = STATE_PAUSING;
		} else if (state == STATE_PAUSING) {
			//resetta timebase-var om ir finns
			uint8_t res = IrTransceiver_Receive_Poll(&len);
			if (res == IR_NOT_FINISHED || res == IR_OK) {
				timePauseStarted = Timebase_CurrentTime();
			}
			if (res == IR_OK) {
				/* start a new reception */
				IrTransceiver_Receive_Start(rxbuffer);
			}
			
			time = Timebase_CurrentTime();
			if (time - timePauseStarted >= proto.timeout) {
				state = STATE_START_IDLE;
			}
		} else if (state == STATE_START_IDLE) {
			if (proto.protocol != IR_PROTO_UNKNOWN) {
				//skicka på can
				txMsg.Data.bytes[0] = IR_BUTTON_UP;
				BIOS_CanSend(&txMsg);
			}
			state = STATE_IDLE;
		}
		
		
		
		if (rxMsgFull) {
			/* No message reception functionality implemented */
			/* Flush the received message */
    		rxMsgFull = 0; //  
		}
	}
	return 0;
}
