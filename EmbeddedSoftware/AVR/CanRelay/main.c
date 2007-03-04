/**
 * CAN Relay.
 *
 *
 * @date    2006-12-17
 * @author  Erik Larsson
 *
 *   ADC=Vin*1024/Vref
 *   Vout=( 10 mV/C )( Temperature C ) + 500 mV
 *
 *  Transmitts: DATA: <temperature low byte> <temperature high byte> <relay status> <failsafe mode>
 *
 *
 */

#define TWO_BUTTON_MODE 0 /* If using two (or just one but nothing more) auxiliary buttons */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
/* lib files */
#include <bios.h>
#include <tc1047.h>
#include <funcdefs.h>

/* defines */
#define GROUP_ID		0x01UL

#define RELAY_OFF			0x01
#define RELAY_ON			0x02
#define FAILSAFE_MODE		0x0F
#define FAILSAFE_TRESHOLD	60  /* Degrees when nodeRelay goes into failsafe mode */

#define RELAY_CMD_ON		0x01
#define RELAY_CMD_OFF		0x02
#define RELAY_CMD_TOGGLE	0x03
#define RELAY_CMD_STATUS	0x04
#define RELAY_CMD_RESET		0x05
#define STATUS_SEND_PERIOD	10000UL /* milliseconds */
#define FAILSAFE_SEND_PERIOD	10000UL /* milliseconds */
#define BOUNCE_TIME			10 /* milliseconds */
#define BUTTON1				1
#define BUTTON2				2

#define TRUE	1
#define FALSE	0

/*-------------------------------------------------------------------------
 * Global variables
 * -----------------------------------------------------------------------*/
uint8_t relayStatus, failsafe_flag = FALSE, rxMsg_Data_bytes[8], msg_received = FALSE;
uint32_t boardTemperature = 0;


/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
uint8_t relayOff();
uint8_t relayOn();
void relayFailsafe();
void sendStatus();

void can_receive(Can_Message_t *msg)
{ // CAN callback function
	uint32_t act;
	uint8_t m, act_type, group, node;

	if( ((msg->Id & CLASS_MASK)>>CLASS_MASK_BITS) == CLASS_ACT){
		// Actuator package
		act = (msg->Id & TYPE_MASK) >> TYPE_MASK_BITS;
		act_type =(uint8_t)((act & ACT_TYPE_MASK) >> ACT_TYPE_BITS);
		group = (uint8_t)((act & ACT_GROUP_MASK) >> ACT_GROUP_BITS);
		node = (uint8_t)((act & ACT_NODE_MASK) >> ACT_NODE_BITS);

		if( act_type == ACT_TYPE_RELAY && (group == GROUP_ID || node == NODE_ID) ){
			// This is a message for this node

			if(rxMsg_Data_bytes[0] == RELAY_CMD_RESET){
				// Return to normal operation
				failsafe_flag = 0;
			}

			for(m=0;m<(msg->DataLength);m++){
				rxMsg_Data_bytes[m] = msg->Data.bytes[m];
			}
			msg_received = TRUE; // Tell application that message has arrived
		}
	}
}

/*----------------------------------------------------------------------------
 * Interrupt routines // FIXME ordna och testa tryckknappar/rotary encoder
 * -------------------------------------------------------------------------*/
#if TWO_BUTTON_MODE
ISR( PCINT2_vect )
{

}
#endif

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {

	uint32_t timeStamp = bios->timebase_get(), temp = timeStamp;

	Can_Message_t txMsg;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.DataLength = 4;

	adcTemperatureInit();

	// Set up callback for CAN messages
	bios->can_callback = &can_receive;

	sei();

	// Turn relay off
	relayStatus = relayOff();
//	txMsg.Id = 0x8000110; // FIXME
//	bios->can_send(&txMsg);

	// Main loop
	while (1) {

		if(msg_received && !failsafe_flag ){

			if( rxMsg_Data_bytes[0] == RELAY_CMD_ON ){
				// Turn relay on
				relayStatus = relayOn();

			}else if(rxMsg_Data_bytes[0] == RELAY_CMD_OFF ){
				// Turn relay off
				relayStatus = relayOff();

			}else if(rxMsg_Data_bytes[0] == RELAY_CMD_TOGGLE ){
					// Toggle relay
					if(relayStatus == RELAY_OFF){
						relayStatus = relayOn();
					}else{
						relayStatus = relayOff();
					}

			}else if(rxMsg_Data_bytes[0] == RELAY_CMD_STATUS ){
				// Send relay status to CAN
				sendStatus();
			}
			msg_received = FALSE;
		}

		temp = bios->timebase_get();
		if( temp - timeStamp >= STATUS_SEND_PERIOD ){
			timeStamp = temp;
/*			txMsg.Id = 0x8000111;
			txMsg.Data.bytes[0] = timeStamp & 0x000000FF;
			txMsg.Data.bytes[0] = (timeStamp & 0x0000FF00)>>8;
			txMsg.Data.bytes[0] = (timeStamp & 0x00FF0000)>>16;
			txMsg.Data.bytes[0] = (timeStamp & 0xFF000000)>>24;
			bios->can_send(&txMsg);
*/			// Send relay status to CAN
			sendStatus();
		}
	}
	return 0;
}

uint8_t relayOff()
{
	// Turn off relay
	PORTC &= ~(1<<PC1);
	DDRC |= (1<<DDC1);

	return RELAY_OFF;
}

uint8_t relayOn()
{
	// Turn on relay
	DDRC &= ~(1<<DDC1);
	PORTC |= (1<<PC1);

	return RELAY_ON;
}

void relayFailsafe()
{
	uint32_t failsafe_timeStamp, temp;
	relayStatus = relayOff();
	failsafe_flag = 1;

	while(1){

		if( !failsafe_flag ){
			// Return from failsafe mode
			return;
		}
// TODO skicka med period
		sendStatus( );
	}
}

void sendStatus()
{
	uint32_t temperature;
	Can_Message_t statusMsg;
	statusMsg.RemoteFlag = 0;
	statusMsg.ExtendedFlag = 1;
	statusMsg.DataLength = 4;
	statusMsg.Id = ( CLASS_SNS<<CLASS_MASK_BITS )|( SNS_ACT_STATUS_RELAY<<TYPE_MASK_BITS )|(NODE_ID);

	temperature = getTC1047temperature();
	if( (int32_t)temperature >= FAILSAFE_TRESHOLD && !failsafe_flag ){
		// Goto failsafe mode
		relayFailsafe();
	}else{
		// Transmitt node status
		statusMsg.Data.bytes[0] = temperature & 0x00FF;
		statusMsg.Data.bytes[1] = (temperature & 0xFF00)>>8;
		statusMsg.Data.bytes[2] = relayStatus;
		statusMsg.Data.bytes[3] = failsafe_flag;

		bios->can_send( &statusMsg );
	}
}
