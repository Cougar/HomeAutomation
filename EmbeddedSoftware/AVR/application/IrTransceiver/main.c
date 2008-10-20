#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
#include <drivers/timer/timebase.h>
#include <drivers/ir/transceiver/irtransceiver.h>
#include <drivers/ir/protocols.h>

//Transmitter
#define APP_TYPE_TRANSMIT    CAN_APPTYPES_IRTRANSMITTER
#define APP_VERSION_TRANSMIT 0x0002

#define STATE_IDLE_TRANSMIT				0
#define STATE_START_TRANSMIT	1
#define STATE_TRANSMITTING		2
#define STATE_START_PAUSE_TRANSMIT		3
#define STATE_PAUSING_TRANSMIT			4
#define STATE_STOP_TRANSMIT				5


//Receiver
#define APP_TYPE_RECEIVE    CAN_APPTYPES_IRRECEIVER
#define APP_VERSION_RECEIVE 0x0004

#define STATE_IDLE_RECEIVE			0
#define STATE_IR_REPEAT_RECEIVE		1
#define STATE_START_RECEIVE	2
#define STATE_RECEIVING		3
#define STATE_START_PAUSE_RECEIVE	4
#define STATE_PAUSING_RECEIVE		5
#define STATE_START_IDLE_RECEIVE	6

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
	
	Can_Message_t txMsg_receive;
	txMsg_receive.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) | 
		(NODE_ID << CAN_SHIFT_NMT_SID);
	txMsg_receive.DataLength = 4;
	txMsg_receive.RemoteFlag = 0;
	txMsg_receive.ExtendedFlag = 1;
	txMsg_receive.Data.words[0] = APP_TYPE_RECEIVE;
	txMsg_receive.Data.words[1] = APP_VERSION_RECEIVE;

	// We will pretend to be two applications
	Can_Message_t txMsg_transmit;
	txMsg_transmit.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) | 
		(NODE_ID << CAN_SHIFT_NMT_SID);
	txMsg_transmit.DataLength = 4;
	txMsg_transmit.RemoteFlag = 0;
	txMsg_transmit.ExtendedFlag = 1;
	txMsg_transmit.Data.words[0] = APP_TYPE_TRANSMIT;
	txMsg_transmit.Data.words[1] = APP_VERSION_TRANSMIT;
	
	// Set up callback for CAN reception, this is optional if only sending is required.
	BIOS_CanCallback = &can_receive;
	// Send CAN_NMT_APP_START for the receiver part, this is first since
	// we don't send commands to it
	BIOS_CanSend(&txMsg_receive);
	
	// Send CAN_NMT_APP_START for the transmitter part aswell
	BIOS_CanSend(&txMsg_transmit);
	
	uint32_t time = Timebase_CurrentTime();
	uint32_t timePauseStarted_transmit = 0;
	uint32_t timePauseStarted_receive = 0;
	
	// Transmitter
	uint8_t state_transmit=STATE_IDLE_TRANSMIT;
	uint8_t repeatcnt_transmit=0;
	uint8_t stop_transmit=0;

	Ir_Protocol_Data_t proto_transmit;
	proto_transmit.timeout=0; 
	proto_transmit.data=0; 
	proto_transmit.repeats=0; 
	proto_transmit.protocol=0; 
	proto_transmit.framecnt = 0;
	
	uint8_t len_transmit=0;
	
	uint16_t txbuffer[MAX_NR_TIMES];
	
	//Receiver
	uint8_t state_receive = STATE_IDLE_RECEIVE;
	
	Ir_Protocol_Data_t proto_receive;
	proto_receive.timeout=0; 
	proto_receive.data=0; 
	proto_receive.repeats=0; 
	proto_receive.protocol=0;
	
	uint8_t len_receive=0;

	uint16_t rxbuffer[MAX_NR_TIMES];
	
	while (1) {

    //Transmitter
		if (state_transmit == STATE_IDLE_TRANSMIT) {
		} 
		else if (state_transmit == STATE_START_TRANSMIT) {
			if (expandProtocol(txbuffer, &len_transmit, &proto_transmit) == IR_OK) {
				if (IrTransceiver_Transmit(txbuffer, 
					len_transmit, proto_transmit.modfreq) == IR_OK) {
					state_transmit = STATE_TRANSMITTING;
				} else {
					state_transmit = STATE_IDLE_TRANSMIT;
				}
			} else {
				state_transmit = STATE_IDLE_TRANSMIT;
			}
		} else if (state_transmit == STATE_TRANSMITTING) {
			//polla om den är klar, om klar gå till STATE_START_PAUSE
			if (IrTransceiver_Transmit_Poll() != IR_NOT_FINISHED) {
				state_transmit = STATE_START_PAUSE_TRANSMIT;
			}
		} else if (state_transmit == STATE_START_PAUSE_TRANSMIT) {
			if (repeatcnt_transmit<proto_transmit.repeats) {
				repeatcnt_transmit++;
			}
			timePauseStarted_transmit = Timebase_CurrentTime();
			if (proto_transmit.framecnt != 255) {
				proto_transmit.framecnt++;
			}
			state_transmit = STATE_PAUSING_TRANSMIT;
		} else if (state_transmit == STATE_PAUSING_TRANSMIT) {
			//när timeout har gått (timebase) så gå till STATE_START_TRANSMIT
			time = Timebase_CurrentTime();
			if (time - timePauseStarted_transmit >= proto_transmit.timeout) {
				state_transmit = STATE_START_TRANSMIT;
			}
			if (stop_transmit == 1 && repeatcnt_transmit >= proto_transmit.repeats) {
				state_transmit = STATE_STOP_TRANSMIT;
			}
		} else if (state_transmit == STATE_STOP_TRANSMIT) {
			stop_transmit = 0;
			proto_transmit.timeout = 0;
			proto_transmit.framecnt = 0;
			repeatcnt_transmit = 0;
			state_transmit = STATE_IDLE_TRANSMIT;
		}
		
		if (rxMsgFull) {
			if ( ((rxMsg.Id & CAN_MASK_CLASS)>>CAN_SHIFT_CLASS) == CAN_ACT) {
				uint16_t acttype =(uint16_t)((rxMsg.Id & CAN_MASK_ACT_TYPE) >> CAN_SHIFT_ACT_TYPE);
				uint8_t actid = (uint8_t)((rxMsg.Id & CAN_MASK_ACT_ID) >> CAN_SHIFT_ACT_ID);
				
				if (rxMsg.DataLength==6) {
					if (acttype == ACT_TYPE_IR) {
						if (state_transmit == STATE_IDLE_TRANSMIT && rxMsg.Data.bytes[0] == IR_BUTTON_DOWN) {
							proto_transmit.protocol = rxMsg.Data.bytes[1];
							proto_transmit.data = rxMsg.Data.bytes[5];
							proto_transmit.data |= (rxMsg.Data.bytes[4]<<8);
							proto_transmit.data |= ((uint32_t)rxMsg.Data.bytes[3]<<16);
							proto_transmit.data |= ((uint32_t)rxMsg.Data.bytes[2]<<24);

							state_transmit = STATE_START_TRANSMIT;
						} else if (state_transmit != STATE_IDLE_TRANSMIT && rxMsg.Data.bytes[0] == IR_BUTTON_UP) {
							stop_transmit = 1;
						}
					}
				}
			}
		
			// Flush the received message
    		rxMsgFull = 0; //  
		}
		
		//Receiver			
		if (state_receive == STATE_IDLE_RECEIVE) {
			IrTransceiver_Receive_Start(rxbuffer);
			state_receive = STATE_START_RECEIVE;
		} else if (state_receive == STATE_START_RECEIVE) {
			state_receive = STATE_RECEIVING;
		} else if (state_receive == STATE_RECEIVING) {
			uint8_t res = IrTransceiver_Receive_Poll(&len_receive);
			if ((res == IR_OK) && (len_receive > 0)) {
				//låt protocols parsa och skicka på can
#if !(SEND_DEBUG)
				uint8_t res2 = parseProtocol(rxbuffer, len_receive, &proto_receive);
				if (res2 == IR_OK && proto_receive.protocol != IR_PROTO_UNKNOWN) {
					txMsg_receive.Data.bytes[0] = IR_BUTTON_DOWN;
					txMsg_receive.Data.bytes[1] = proto_receive.protocol;
					txMsg_receive.Data.bytes[2] = (proto_receive.data>>24)&0xff;
					txMsg_receive.Data.bytes[3] = (proto_receive.data>>16)&0xff;
					txMsg_receive.Data.bytes[4] = (proto_receive.data>>8)&0xff;
					txMsg_receive.Data.bytes[5] = proto_receive.data&0xff;
					txMsg_receive.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_IR << CAN_SHIFT_SNS_TYPE) | (IR_ID_DATA<<CAN_SHIFT_SNS_ID) | (NODE_ID << CAN_SHIFT_SNS_SID));
					txMsg_receive.DataLength = 6;
					BIOS_CanSend(&txMsg_receive);
				} else if (proto_receive.protocol == IR_PROTO_UNKNOWN) {
				}
#endif
#if (SEND_DEBUG)
				send_debug(rxbuffer, len_receive);
				proto_receive.timeout=300;
#endif
				
				state_receive = STATE_START_PAUSE_RECEIVE;
			}
		} else if (state_receive == STATE_START_PAUSE_RECEIVE) {
			IrTransceiver_Receive_Start(rxbuffer);
			timePauseStarted_receive = Timebase_CurrentTime();
			state_receive = STATE_PAUSING_RECEIVE;
		} else if (state_receive == STATE_PAUSING_RECEIVE) {
			//resetta timebase-var om ir finns
			uint8_t res = IrTransceiver_Receive_Poll(&len_receive);
			if (res == IR_NOT_FINISHED || res == IR_OK) {
				timePauseStarted_receive = Timebase_CurrentTime();
			}
			if (res == IR_OK) {
				/* start a new reception */
				IrTransceiver_Receive_Start(rxbuffer);
			}
			
			time = Timebase_CurrentTime();
			if (time - timePauseStarted_receive >= proto_receive.timeout) {
				state_receive = STATE_START_IDLE_RECEIVE;
			}
		} else if (state_receive == STATE_START_IDLE_RECEIVE) {
			if (proto_receive.protocol != IR_PROTO_UNKNOWN) {
				//skicka på can
				txMsg_receive.Data.bytes[0] = IR_BUTTON_UP;
				BIOS_CanSend(&txMsg_receive);
			}
			state_receive = STATE_IDLE_RECEIVE;
		}
	}
	
	return 0;
}

