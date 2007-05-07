/**
 * CAN Test. This program sends a CAN message once every second. The ID of the
 * message is increased each time for testing purposes.
 * 
 * @date	2006-11-21
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
#include <serial.h>
#include <timebase.h>


/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
	Mcu_Init();
	Timebase_Init();
	Serial_Init();
	
	sei();
	
	printf("\n------------------------------------------------------------\n");
	printf(  "   CAN Test: Periodic Transmission\n");
	printf(  "------------------------------------------------------------\n");
	
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
	//The databytes are just what happens to be in the memory. They are never set.
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1; 
	txMsg.Id = 1600000;
	txMsg.DataLength = 2;
	
	/* main loop */
	while (1) {
		/* service the CAN routines */
		Can_Service();
		
		/* send CAN message and check for CAN errors once every second */
		if (Timebase_PassedTimeMillis(timeStamp) >= 1000) {
			timeStamp = Timebase_CurrentTime();
			/* send txMsg */
			txMsg.Id++;
			Can_Send(&txMsg);
		}
		
		/* check if any messages have been received */
		while (Can_Receive(&rxMsg) == CAN_OK) {
			printf("MSG Received: ID=%lx, DLC=%u, EXT=%u, RTR=%u, ", rxMsg.Id, (uint16_t)(rxMsg.DataLength), (uint16_t)(rxMsg.ExtendedFlag), (uint16_t)(rxMsg.RemoteFlag));
    		printf("data={ ");
    		for (uint8_t i=0; i<rxMsg.DataLength; i++) {
    			printf("%x ", rxMsg.Data.bytes[i]);
    		}
    		printf("}\n");
		}
	}
	
	return 0;
}
