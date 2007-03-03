/**
 * CanIRreceive
 * Receives IR data and sends packages to CAN
 *
 * @date	2006-11-21
 * @author	Jimmy Myhrman, Anders Runeson
 * Converted to BIOS version by Erik Larsson 2007-02-12
 *
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
/* lib files */
#include <bios.h>
#include <irreceiver.h>
#include <funcdefs.h>

#define STATE_IDLE		0
#define STATE_IR_REPEAT	1

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {

	sei();

	IRDDR &= ~(1<<IRBIT);

	uint32_t timeStamp = 0;
	uint32_t irTimeoutTimeStamp = 0;

	Can_Message_t txMsg;
	Can_Message_t rxMsg;
	txMsg.Id = (CLASS_SNS << CLASS_MASK_BITS)|(SNS_IR_RECEIVE << TYPE_MASK_BITS)|(NODE_ID);

	uint8_t ir_protocol, ir_address, ir_command;
	uint8_t state = STATE_IDLE;

	/* main loop */
	while (1) {
		if (state == STATE_IDLE) {
			if (checkIr(&ir_protocol, &ir_address, &ir_command) == IR_OK) {
				txMsg.Data.bytes[0] = 0x00;
				txMsg.Data.bytes[1] = ir_protocol;
				txMsg.Data.bytes[2] = ir_address;
				txMsg.Data.bytes[3] = ir_command;
				txMsg.DataLength = 4;
				bios->can_send(&txMsg);
				irTimeoutTimeStamp = bios->timebase_get();
				state = STATE_IR_REPEAT;
			}
		} else if (state == STATE_IR_REPEAT) {
			//kolla efter ny ir, om ingen ny ir och timeout så sätt state till IDLE
			if (bios->timebase_get() - irTimeoutTimeStamp >= 105) {		//getLastProtoTimeout()
				state = STATE_IDLE;
				txMsg.Data.bytes[0] = 0x0f;
				bios->can_send(&txMsg);
			}
			if (checkIrIdle() == IR_OK) {		//funktion som borde finnas i irreceiver.c
				irTimeoutTimeStamp = bios->timebase_get();
			}
		}
	}

	return 0;
}
