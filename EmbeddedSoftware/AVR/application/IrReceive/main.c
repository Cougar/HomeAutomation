#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
//#include <drivers/uart/serial.h>
#include <drivers/timer/timebase.h>
#include <drivers/ir/receiver/irreceiver.h>

#define APP_TYPE    0xf010
#define APP_VERSION 0x0001

#define CAN_IR 0x12

#define STATE_IDLE		0
#define STATE_IR_REPEAT	1


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

	txMsg.Id = CAN_ID_SNS_IRDATA;	// ((CAN_SNS << CAN_SHIFT_CLASS) | (NODE_ID << CAN_SHIFT_SNS_SID))
	
	//printf("AVR Test Application\n");
	//printf("Using AVR BIOS version %x\n", BIOS_VERSION);

	//txMsg.Id = (CAN_SNS << CAN_SHIFT_CLASS) | ((CAN_IR & CAN_MASK_SNS_FUNCC) << CAN_SHIFT_SNS_FUNCC) | (NODE_ID << CAN_SHIFT_SNS_SID);
	//txMsg.Data.dwords[0] = 0x01020304;
	//txMsg.DataLength = 4;

	IRDDR &= ~(1<<IRBIT);

	unsigned long time = Timebase_CurrentTime();
	unsigned long irTimeoutTime;
	
	uint8_t ir_protocol, ir_address, ir_command;
	uint8_t state = STATE_IDLE;

	while (1) {
		time = Timebase_CurrentTime();
		if (state == STATE_IDLE) {
			if (checkIr(&ir_protocol, &ir_address, &ir_command) == IR_OK) {
				txMsg.Data.bytes[0] = 0x00;
				txMsg.Data.bytes[1] = ir_protocol;
				txMsg.Data.bytes[2] = ir_address;
				txMsg.Data.bytes[3] = ir_command;
				txMsg.DataLength = 4;
				BIOS_CanSend(&txMsg);
				irTimeoutTime = Timebase_CurrentTime();
				state = STATE_IR_REPEAT;
			}
		} else if (state == STATE_IR_REPEAT) {
			//kolla efter ny ir, om ingen ny ir och timeout så sätt state till IDLE
			if (time - irTimeoutTime >= 105) {		//getLastProtoTimeout()
				state = STATE_IDLE;
				txMsg.Data.bytes[0] = 0x0f;
				BIOS_CanSend(&txMsg);
			}
			if (checkIrIdle() == IR_OK) {		//funktion som borde finnas i irreceiver.c
				irTimeoutTime = Timebase_CurrentTime();
			}
		}

		
		if (rxMsgFull) {
			// Flush the received message
    		rxMsgFull = 0; //  
		}
	}
	
	return 0;
}
