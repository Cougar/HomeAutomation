/**
 * CAN Test.
 * Tests if sending CAN messages is ok. 
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
#include <timebase.h>

#define INTERVAL 1000
#define SENDER_ID 0xff
/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
	Timebase_Init();
	
	sei();
	
	Can_Init();
	
	uint32_t timeStamp = 0;
	
	Can_Message_t txMsg;
	
	txMsg.Id = SENDER_ID;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.DataLength = 1;
	txMsg.Data.bytes[0] = 0xff; 
	
	/* main loop */
	while (1) {
		/* service the CAN routines */
		Can_Service();
		
		/* send CAN message */
		if (Timebase_PassedTimeMillis(timeStamp) >= INTERVAL) {
			timeStamp = Timebase_CurrentTime();
			/* send txMsg */
			Can_Send(&txMsg);
		}
	}
	
	return 0;
}
