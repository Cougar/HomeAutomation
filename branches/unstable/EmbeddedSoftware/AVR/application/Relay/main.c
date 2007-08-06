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


/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
/* lib files */
#include <config.h>
#include <bios.h>
#include <drivers/timer/timebase.h>
#include <drivers/sensor/tc1047/tc1047.h>


/* defines */
#define RELAY_OFF			0x01
#define RELAY_ON			0x02
#define FAILSAFE_MODE		0x0F
#define FAILSAFE_TRESHOLD	0xFF  /* Degrees when nodeRelay goes into failsafe mode */

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

#define APP_TYPE    CAN_APPTYPES_RELAY
#define APP_VERSION 0x0001

/*-------------------------------------------------------------------------
 * Global variables
 * -----------------------------------------------------------------------*/
uint8_t relayStatus, failsafe_flag = 0;
uint32_t boardTemperature = 0;

volatile Can_Message_t rxMsg; // Message storage
volatile uint8_t rxMsgFull;   // Synchronization flag


/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
uint8_t relayOff(void);
uint8_t relayOn(void);
void relayFailsafe(void);
void sendStatus(void);

void can_receive(Can_Message_t *msg)
{ // CAN callback function
	if (!rxMsgFull) {
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
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

	uint32_t timeStamp = Timebase_CurrentTime(), temp = timeStamp;

	adcTemperatureInit();

	// Set up callback for CAN messages
	BIOS_CanCallback = &can_receive;

	sei();
	
	Timebase_Init();

	Can_Message_t txMsg;
	txMsg.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) | (NODE_ID << CAN_SHIFT_NMT_SID);
	txMsg.DataLength = 4;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.Data.words[0] = APP_TYPE;
	txMsg.Data.words[1] = APP_VERSION;
	BIOS_CanSend(&txMsg);

	// Turn relay off
	relayStatus = relayOff();

	// Main loop
	while (1) {

		temp = Timebase_CurrentTime();
		if( temp - timeStamp >= STATUS_SEND_PERIOD ){
			timeStamp = temp;
			// Send relay status to CAN
			sendStatus();
		}

		if (rxMsgFull) {
			uint16_t acttype;
			uint8_t relayid;
		
			if ( ((rxMsg.Id & CAN_MASK_CLASS)>>CAN_SHIFT_CLASS) == CAN_ACT){
				// Actuator package
				acttype =(uint16_t)((rxMsg.Id & CAN_MASK_ACT_TYPE) >> CAN_SHIFT_ACT_TYPE);
				relayid = (uint8_t)((rxMsg.Id & CAN_MASK_ACT_ID) >> CAN_SHIFT_ACT_ID);
				
				if ((acttype == ACT_TYPE_RELAY) && (relayid == THISRELAYID)) {
					// This is a message for this relay
					if (rxMsg.Data.bytes[0] == RELAY_CMD_RESET) {
						// Return to normal operation
						failsafe_flag = 0;
					}
					
					if (!failsafe_flag) {
						if (rxMsg.Data.bytes[0] == RELAY_CMD_ON ){
							// Turn relay on
							relayStatus = relayOn();
			
						} else if (rxMsg.Data.bytes[0] == RELAY_CMD_OFF ){
							// Turn relay off
							relayStatus = relayOff();
			
						} else if (rxMsg.Data.bytes[0] == RELAY_CMD_TOGGLE ){
								// Toggle relay
								if(relayStatus == RELAY_OFF){
									relayStatus = relayOn();
								}else{
									relayStatus = relayOff();
								}
			
						}else if(rxMsg.Data.bytes[0] == RELAY_CMD_STATUS ){
							// Send relay status to CAN
							sendStatus();
						}
					}
				}
			}
    		rxMsgFull = 0; //  

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
	statusMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_STATUS << CAN_SHIFT_SNS_TYPE) | (SNS_ID_RELAY_STATUS << CAN_SHIFT_SNS_ID) | (NODE_ID << CAN_SHIFT_SNS_SID));
	
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

		BIOS_CanSend( &statusMsg );
	}
}
