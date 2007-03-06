#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
#include <drivers/uart/serial.h>
#include <drivers/timer/timebase.h>

#define APP_TYPE    0xf001
#define APP_VERSION 0x0001

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
	Serial_Init();
	
	unsigned long time, time1 = Timebase_CurrentTime();
	
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
	
	printf("AVR Test Application\n");
	printf("Using AVR BIOS version %x\n", BIOS_VERSION);
	
	txMsg.Id = (CAN_TST << CAN_SHIFT_CLASS) | NODE_ID;
	txMsg.Data.dwords[0] = 0x01020304;
	txMsg.DataLength = 8;

	while (1) {
		time = Timebase_CurrentTime();
		if ((time - time1) >= 2000) {
			time1 = time;
			// Send a CAN message
			txMsg.Data.dwords[1] = time;
			BIOS_CanSend(&txMsg);
			printf("Two seconds passed, time is now %ld.\n", time);
		}
		
		if (rxMsgFull) {
			// Print the received message
			printf("RX: ID=%08lx, DLC=%u, EXT=%u, RTR=%u, data={ ", 
			        rxMsg.Id,
			        (uint16_t)rxMsg.DataLength, 
			        (uint16_t)rxMsg.ExtendedFlag, 
			        (uint16_t)rxMsg.RemoteFlag);
    		for (uint8_t i=0; i<rxMsg.DataLength; i++) {
    			printf("%02x ", rxMsg.Data.bytes[i]);
    		}
    		printf("}\n");
    		rxMsgFull = 0; //  
		}
	}
	
	return 0;
}
