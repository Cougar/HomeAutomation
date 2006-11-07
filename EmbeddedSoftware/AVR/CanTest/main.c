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
	TimebaseInit();
	UartInit();
	sei();
	
	printf("\n------------------------------\n");
	printf("   CAN Monitor\n");
	printf("------------------------------\n");
	
	printf("CanInit...");
	if (CanInit(CAN_BITRATE_1M) != CAN_OK) {
		printf("FAILED!\n");
	}
	else {
		printf("OK!\n");
	}
	
	uint32_t timeStamp = 0;
	
	CanMessage_t txMsg;
	CanMessage_t rxMsg;
	
	txMsg.DataLength = 8;
	txMsg.Data.bytes[0] = 7;
	txMsg.Id = 733;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	
	/* main loop */
	while (1) {
		/* any new CAN messages received? */
		if (CanReceive(&rxMsg) == CAN_OK) {
			ProtocolParseCanMessage(&rxMsg);
		}
		
		/* send CAN message and check for CAN errors once every second */
		if (TimebasePassedTimeMS(timeStamp) >= 1000) {
			/* increase ID and send txMsg */
			txMsg.Id++;
			CanSend(&txMsg);
			/* check for errors */
			timeStamp = TimebaseCurrentTime();
			if (CanCheckError() != CAN_OK) {
				printf("CAN ERROR detected!\n");
			}
		}
	}
	
	return 0;
}
