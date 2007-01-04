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
#include <irreceiver.h>

#define STATE_IDLE		0
#define STATE_IR_REPEAT	1

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
	Timebase_Init();
	Serial_Init();
    #if defined(__AVR_ATmega88__)
    // set the clock speed to 8MHz
    // set the clock prescaler. First write CLKPCE to enable setting of clock the
    // next four instructions.
    CLKPR=(1<<CLKPCE);
    CLKPR=0; // 8 MHZ
    #endif
    
	sei();
    
	IRDDR &= ~(1<<IRBIT);
	
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
	uint32_t irTimeoutTimeStamp = 0;
	
	Can_Message_t txMsg;
	Can_Message_t rxMsg;
	txMsg.Id = 1;
	
	uint8_t ir_protocol, ir_address, ir_command;
	uint8_t state = STATE_IDLE;
	
	/* main loop */
	while (1) {
		/* service the CAN routines */
		Can_Service();
		
		if (state == STATE_IDLE) {
			if (checkIr(&ir_protocol, &ir_address, &ir_command) == IR_OK) {
				txMsg.Data.bytes[0] = 0x00;
				txMsg.Data.bytes[1] = ir_protocol;
				txMsg.Data.bytes[2] = ir_address;
				txMsg.Data.bytes[3] = ir_command;
				txMsg.DataLength = 4;
				Can_Send(&txMsg);
				irTimeoutTimeStamp = Timebase_CurrentTime();
				state = STATE_IR_REPEAT;
			}
		} else if (state == STATE_IR_REPEAT) {
			//kolla efter ny ir, om ingen ny ir och timeout så sätt state till IDLE
			if (Timebase_PassedTimeMillis(irTimeoutTimeStamp) >= 105) {		//getLastProtoTimeout()
				state = STATE_IDLE;
				txMsg.Data.bytes[0] = 0x0f;
				Can_Send(&txMsg);
			}
			if (checkIrIdle() == IR_OK) {		//funktion som borde finnas i irreceiver.c
				irTimeoutTimeStamp = Timebase_CurrentTime();
			}
		}
		
	
		/* check if any messages have been received */
		while (Can_Receive(&rxMsg) == CAN_OK) {
			/*printf("MSG Received: ID=%lx, DLC=%u, EXT=%u, RTR=%u, ", rxMsg.Id, (uint16_t)(rxMsg.DataLength), (uint16_t)(rxMsg.ExtendedFlag), (uint16_t)(rxMsg.RemoteFlag));
    		printf("data={ ");
    		for (uint8_t i=0; i<rxMsg.DataLength; i++) {
    			printf("%x ", rxMsg.Data.bytes[i]);
    		}
    		printf("}\n");*/
		}
	}
	
	return 0;
}
