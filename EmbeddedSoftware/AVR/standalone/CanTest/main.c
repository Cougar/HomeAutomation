/**
 * CAN Test. This program is for testing purpose.
 * Replaces CanTestPeriodic and CanTestEcho.
 *
 * CanTest sends messages periodicly, dumps incomming messages on uart
 * and echoes on packages with ID== ECHO_RECEIVE_ID;
 *
 * This version uses drivers that polls the mcp2515. That means that if this program works fine,
 * but bios+app doesnt. Something is wrong with the interruppt.
 * Probably the wire between AVR and MCP, so check it twice.
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
/* lib files */
#include <can.h>
#include <serial.h>
#include <timebase.h>

#define SENDING_ID	0x1600000UL // FIXME vilka idn kan vara lämpliga?
#define ECHO_RECEIVE_ID		0x1700000UL
#define ECHO_SENDING_ID		0x17000FFUL



/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
	Mcu_Init();
	Timebase_Init();
	Serial_Init();

	sei();

	printf("\n------------------------------------------------------------\n");
	printf(  "   CAN Test:\n");
	printf(  "   Periodic Transmission on CAN\n");
	printf(  "   CAN Dump\n");
	printf(  "   CAN Echo\n");
	printf(  "------------------------------------------------------------\n");
	
	printf("CanInit...");
	if (Can_Init() != CAN_OK) {
		printf("FAILED!\n");
		printf("Check wires between AVR and MCP2515 and the MCP speed (xtal).");
	}
	else {
		printf("OK!\n");
		printf("MCP2515 working fine\n");
	}
	
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
		/* service the CAN routines */
		Can_Service();
		
		/* send CAN message and check for CAN errors once every second */
		if (Timebase_PassedTimeMillis(timeStamp) >= 1000) {
			timeStamp = Timebase_CurrentTime();
			/* send txMsg */
			txMsg.Id = SENDING_ID;
			Can_Send(&txMsg);
		}
		
		/* check if any messages have been received */
		while (Can_Receive(&rxMsg) == CAN_OK) {
			/* Dump message data on uart */
			printf("MSG Received: ID=%lx, DLC=%u, EXT=%u, RTR=%u, ", rxMsg.Id, (uint16_t)(rxMsg.DataLength), (uint16_t)(rxMsg.ExtendedFlag), (uint16_t)(rxMsg.RemoteFlag));
			printf("data={ ");
			for (uint8_t i=0; i<rxMsg.DataLength; i++) {
				printf("%x ", rxMsg.Data.bytes[i]);
			}
			printf("}\n");

			/* Echo function */
			if(rxMsg.Id == ECHO_RECEIVE_ID){
				printf("\"ping\" received\n");
				txMsg.Id = ECHO_SENDING_ID;
				/* Send reply */
				Can_Send(&txMsg);
				printf("reply sent\n");
			}
		}
	}
	
	return 0;
}
