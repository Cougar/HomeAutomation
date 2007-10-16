/**
 * CAN Test. This program is for testing purpose.
 * Replaces CanTestPeriodic and CanTestEcho.
 *
 * CanTest sends messages periodicly, dumps incomming messages on uart
 * and echoes on packages with ID== ECHO_RECEIVE_ID;
 *
 * 
 * @date	2006-11-21
 * @author	Jimmy Myhrman, Erik Larsson (2007-05-07)
 *   
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdio.h>
#include <string.h>
/* lib files */
#include <config.h>
#include <mcp2515.h>
#include <serial.h>
#include <timebase.h>
#include <mcu.h>

/* LED 1 blinks
#define LED1_PORT	PORTC
#define LED1_DDR	DDRC
#define LED2_PORT	PORTC
#define LED2_DDR	PORTC
*/

volatile Can_Message_t rxMsg; // Message storage
volatile uint8_t rxMsgFull;   // Synchronization flag

void Can_Process(Can_Message_t* msg) {
	if (!(msg->ExtendedFlag)) return; // We don't care about standard CAN frames.

	if (!rxMsgFull) {
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
	Mcu_Init();
	Timebase_Init();
	sei();
#if defined(UART_OUTPUT)
	Serial_Init();

//	printf("\n------------------------------------------------------------\n");
//	printf(  "   CAN Test:\n");
//	printf(  "   Periodic Transmission on CAN\n");
//	printf(  "   CAN Dump\n");
//	printf(  "   CAN Echo\n");
//	printf(  "------------------------------------------------------------\n");
	
	printf("CanInit...");
	if (Can_Init() != CAN_OK) {
		printf("FAILED!\n");
//		printf("Check wires between AVR and MCP2515 and the MCP speed (xtal).");
	}
	else {
		printf("OK!\n");
		printf("MCP2515 working fine\n");
	}
#elif defined(LED_OUTPUT)
	Can_Init();
#else
	Can_Init();
#endif
	
	uint32_t timeStamp = 0;
	
	Can_Message_t txMsg;
	Can_Message_t rxMsg;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.Id = SENDING_ID;
	txMsg.DataLength = 2;
	txMsg.Data.bytes[0] = 0x12;
	txMsg.Data.bytes[1] = 0x34;

	/* main loop */
	while (1) {
		/* send CAN message and check for CAN errors once every second */
		if (Timebase_PassedTimeMillis(timeStamp) >= 1000) {
			timeStamp = Timebase_CurrentTime();
			/* send txMsg */
			txMsg.Id = SENDING_ID;
			Can_Send(&txMsg);
		}
		
		/* check if any messages have been received */
		if (rxMsgFull) {
#if defined(UART_OUTPUT)
			/* Dump message data on uart */
			printf("\nPKT %#lx %u %u", rxMsg.Id, (uint16_t)(rxMsg.ExtendedFlag), (uint16_t)(rxMsg.RemoteFlag));
			for (uint8_t i=0; i<rxMsg.DataLength; i++) {
				printf(" %#x", rxMsg.Data.bytes[i]);
			}
#endif
			/* Echo function */
			if(rxMsg.Id == ECHO_RECEIVE_ID){
#if defined(UART_OUTPUT)
				printf("\n\"ping\" received");
				txMsg.Id = ECHO_SENDING_ID;
				/* Send reply */
				Can_Send(&txMsg);
				printf("\nreply sent");
#else
				txMsg.Id = ECHO_SENDING_ID;
				/* Send reply */
				Can_Send(&txMsg);
#endif
			}
			rxMsgFull = 0;
		}
	}
	
	return 0;
}
