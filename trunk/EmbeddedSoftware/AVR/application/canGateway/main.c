#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
#include <drivers/uart/serial.h>
#include <drivers/timer/timer.h>
#include <drivers/can/can.h>
#include <drivers/can/mcp2515/mcp2515.h>

#define APP_TYPE    CAN_APPTYPES_CANGW
#define APP_VERSION 0x0001

// A simple message "queue", with space for one message only.
// These are declared volatile to tell the compiler not to optimize away accesses.
volatile Can_Message_t rxMsg; // Message storage
volatile uint8_t rxMsgFull;   // Synchronization flag
volatile Can_Message_t extRxMsg;
volatile uint8_t extRxMsgFull;   // Synchronization flag

volatile Can_Message_t txMsg; // Message storage
volatile Can_Message_t extTxMsg;


// external CAN message reception callback
// This function runs with interrupts disabled, keep it as short as possible.
void Can_Process_USART(Can_Message_t* msg) {
	if (!extRxMsgFull) {
		memcpy((void*)&extRxMsg, msg, sizeof(extRxMsg));
		extRxMsgFull = 1;
	}	
}

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
	Serial_Init();
	uint8_t returnval = Can_Init();
	if (returnval != CAN_OK) {
	}
	
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
	
	while (1) {
		
		if (extRxMsgFull) {
			txMsg.Id = extRxMsg.Id;
			txMsg.DataLength = extRxMsg.DataLength;
			txMsg.RemoteFlag = extRxMsg.RemoteFlag;
			txMsg.ExtendedFlag = extRxMsg.ExtendedFlag;
			for (uint8_t i = 0; i < txMsg.DataLength; i++) {
				txMsg.Data.bytes[i] = extRxMsg.Data.bytes[i];
			}
			BIOS_CanSend(&txMsg);

    		extRxMsgFull = 0; //  
		}
		if (rxMsgFull) {
			extTxMsg.Id = rxMsg.Id;
			extTxMsg.DataLength = rxMsg.DataLength;
			extTxMsg.RemoteFlag = rxMsg.RemoteFlag;
			extTxMsg.ExtendedFlag = rxMsg.ExtendedFlag;
			for (uint8_t i = 0; i < extTxMsg.DataLength; i++) {
				extTxMsg.Data.bytes[i] = rxMsg.Data.bytes[i];
			}
			Can_Send(&extTxMsg);

    		rxMsgFull = 0; //  
		}
	}
	
	return 0;
}

