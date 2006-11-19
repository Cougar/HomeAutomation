/**
 * CAN test program.
 * 
 * @target	AVR
 * @date	2006-10-29
 * @author	Jimmy Myhrman
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
#include <uart.h>
#include <timebase.h>
#include <protocol.h>


/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
	Timebase_Init();
	Uart_Init();
	sei();
	
	printf("\n------------------------------\n");
	printf("   CAN Monitor\n");
	printf("------------------------------\n");
	
	printf("CanInit...");
	if (Can_Init() != CAN_OK) {
		printf("FAILED!\n");
	}
	else {
		printf("OK!\n");
	}
	
	uint32_t timeStamp = 0;
	
	Can_Message_t txMsg;
	Can_Message_t rxMsg;
	
	txMsg.DataLength = 8;
	txMsg.Data.bytes[0] = 7;
	txMsg.Id = 0;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	
	/* main loop */
	while (1) {
		/* any new CAN messages received? */
		if (Can_Receive(&rxMsg) == CAN_OK) {
			ProtocolParseCanMessage(&rxMsg);
		}
		
		/* send CAN message and check for CAN errors once every second */
		if (Timebase_PassedTimeMillis(timeStamp) >= 1000) {
			/* send txMsg */
			txMsg.Id++;
			Can_Send(&txMsg);
			/* check for errors */
			timeStamp = Timebase_CurrentTime();
			if (Can_CheckError() != CAN_OK) {
				printf("CAN ERROR detected!\n");
			}
		}
	}
	
	return 0;
}
