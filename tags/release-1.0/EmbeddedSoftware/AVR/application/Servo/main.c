/**
 * CAN Servo.
 * 
 * Input: 0 to 255
 * Position msg: <byte0 BLINDS_CMD> <byte1 position> <byte2 servo number>
 * Turning msg: <byte0 START/STOP> <byte 1 direction> <byte2 servo number>
 *
 * @date    2007-01-01
 * @author  Erik Larsson
 *
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

/* lib files */
#include <bios.h>
#include <config.h>
//#include <settings.h> //Migrated to config.inc

#include <drivers/timer/timebase.h>
#include <drivers/sensor/tc1047/tc1047.h>
#include <drivers/actuator/rc-servo/rc_servo.h>

/*-----------------------------------------------------------------------------
 * Defines 
 *---------------------------------------------------------------------------*/
#define FAILSAFE_MODE       0x0F
#define BLINDS_CMD_ABS      0x01 // Absolute position
#define BLINDS_CMD_REL      0x02 // Relative position
#define BLINDS_CMD_START    0x03 // Start turning RC-servo
#define BLINDS_CMD_STOP     0x04 // Stop turning RC-servo
#define BLINDS_CMD_RESET    0x05
#define BLINDS_CMD_STATUS   0x06

#define BLINDS_TURN_STOP	0x00
#define BLINDS_TURN_CCW		0x01
#define BLINDS_TURN_CW		0x02

#define STATUS_SEND_PERIOD      5000 // ms
#define FAILSAFE_SEND_PERIOD    10000 // ms
#define BOUNCE_TIME             10 // ms
#define BUTTON1                 1
#define BUTTON2                 2

#define SERVO_FEED_PORT			PORTC
#define SERVO_FEED_DDR			DDRC
#define SERVO_FEED_PIN			PC0

#define APP_TYPE	CAN_APPTYPES_SERVO
#define APP_VERSION	0x0001


/*-------------------------------------------------------------------------
 * Global variables
 * -----------------------------------------------------------------------*/
/*#if defined(TWO_BUTTON_MODE)
Can_Message_t txMsg_Button;
#endif*/

volatile uint8_t rxMsgFull;

volatile Can_Message_t rxMsg;

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
void blindsFailsafe(void);

void can_receive(Can_Message_t *msg){
// CAN callback function
	if(!rxMsgFull){
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}
/*----------------------------------------------------------------------------
 * Interrupt routines
 * -------------------------------------------------------------------------*/
#if defined(TWO_BUTTON_MODE)
#error two button mode is still untested
ISR( PCINT2_vect )
{
    /* 
     * Auxiliary button pressed and released
     * Check time between press and release to eliminate bounces
     */
    static uint32_t buttonTime[2];

    txMsg_Button.RemoteFlag = 0;
    txMsg_Button.ExtendedFlag = 1;

    if( (PIND & (1<<PD1)) == 0){
        /* Button pressed */
        buttonTime[0] = Timebase_CurrentTime();
    }else if( (PIND & (1<<PD1)) == 1){
        if( Timebase_PassedTimeMillis( buttonTime[0] ) >= BOUNCE_TIME ){
            /* No bounce, send message */
            txMsg_Button.Id = BUTTON_ID;
            txMsg_Button.DataLength = 1;
            txMsg_Button.Data.bytes[0] = BUTTON1;
            return;
        }else{
            /* Just bounces, ignore it */
            return;
        }
    }
    if( (PIND & (1<<PD2)) == 0){
        /* Button pressed */
        buttonTime[1] = Timebase_CurrentTime();
    }else if( (PIND & (1<<PD2)) == 1){
        if( Timebase_PassedTimeMillis( buttonTime[1] ) >= BOUNCE_TIME ){
            /* No bounce, send message */
            txMsg_Button.Id = BUTTON_ID;
            txMsg_Button.DataLength = 1;
            txMsg_Button.Data.bytes[0] = BUTTON2;
            return;
        }else{
            /* Just bounces, ignore it */
            return;
        }
    }
}
#endif

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
	uint8_t temp_id, servoid;
	uint16_t acttype;

    uint8_t //blindsStatus, /* Position (-128 to 127) */
            servoToTurn,
            turnDirection = BLINDS_TURN_STOP; /* Specifies direction to turn or stopped */
    uint32_t boardTemperature = 0;
    uint32_t timeStamp = 0, timeStampTurn = 0, servoFeed_timeStamp = 0;

	Can_Message_t txMsg;

	// Set up callback for CAN messages
	BIOS_CanCallback = &can_receive;

	sei();

	Timebase_Init();
	adcTemperatureInit();
	rcServoInit();

#if defined(TWO_BUTTON_MODE)
    /*
     * Initialize buttons
     * It is possible to use ie. sensors instead
     * but then change this
     */
    /* Set as input and enable pull-up */
    DDRD &= ~( (1<<DDD1)|(1<<DDD2) );
    PORTD |= (1<<PD1)|(1<<PD2);

    /* Enable IO-pin interrupt */
    PCICR |= (1<<PCIE2);
    /* Unmask PD1 and SERVO_FEED_PIN */
    PCMSK2 |= (1<<PCINT16) | (1<<PCINT17);
#endif
	/* Set output and turn off the servo current feed */
	//SERVO_FEED_DDR |= (1<<SERVO_FEED_PIN);
	//SERVO_FEED_PORT &= ~(1<<SERVO_FEED_PIN);

	txMsg.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) | (NODE_ID << CAN_SHIFT_NMT_SID);
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.DataLength = 4;
	txMsg.Data.words[0] = APP_TYPE;
	txMsg.Data.words[1] = APP_VERSION;

	BIOS_CanSend(&txMsg); // Send startup message
	
	//set up status message
	txMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_STATUS << CAN_SHIFT_SNS_TYPE) | (SNS_ID_SERVO_STATUS << CAN_SHIFT_SNS_ID) | (NODE_ID << CAN_SHIFT_SNS_SID));

    // Main loop
	while (1) {
		// check if any messages have been received
		if(rxMsgFull){
			if( ((rxMsg.Id & CAN_MASK_CLASS)>>CAN_SHIFT_CLASS) == CAN_ACT ){
				// Actuator package
				acttype = (uint16_t)((rxMsg.Id & CAN_MASK_ACT_TYPE) >> CAN_SHIFT_ACT_TYPE);
				servoid = (uint8_t)((rxMsg.Id & CAN_MASK_ACT_ID) >> CAN_SHIFT_ACT_ID);

		// Check if it is command to control this servo
				if(acttype == ACT_TYPE_SERVO && (servoid == SERVOID_1 || servoid == SERVOID_2 || servoid == SERVOID_3)){
					// This node that control a servo is adressed
					if(servoid == SERVOID_1){
						temp_id = 1; // First servo
					}else if(servoid == SERVOID_2){
						temp_id = 2; // and so on
					}else if(servoid == SERVOID_3){
						temp_id = 3;
					}else{
						temp_id = 0;
					}

					if(rxMsg.DataLength == 1 || (rxMsg.DataLength == 2 && rxMsg.Data.bytes[1] == BLINDS_CMD_ABS)){
						// Set absolute position to servo

						setPosition( rxMsg.Data.bytes[0], temp_id );
						servoFeed_timeStamp = Timebase_CurrentTime();

					}else if(rxMsg.DataLength == 2){
						if(rxMsg.Data.bytes[1] == BLINDS_CMD_REL){
							// Set relative position to servo

							alterPosition( rxMsg.Data.bytes[0], temp_id );
							servoFeed_timeStamp = Timebase_CurrentTime();

						}else if(rxMsg.Data.bytes[1] == BLINDS_CMD_START ){
							// Start turning the servo

							turnDirection = (rxMsg.Data.bytes[0]&0x80)?BLINDS_TURN_CW:BLINDS_TURN_CCW; // FIXME riktning beroende på positivt eller negativt tal?
							servoToTurn = temp_id;
							servoFeed_timeStamp = Timebase_CurrentTime();

						}else if(rxMsg.Data.bytes[1] == BLINDS_CMD_STOP ){
							// Stop turning

							turnDirection = BLINDS_TURN_STOP;

						}/*else if(rxMsg.Data.bytes[0] == BLINDS_CMD_STATUS){
							// Send blinds status to CAN
							boardTemperature = getTC1047temperature();

							if( (int32_t)boardTemperature >= FAILSAFE_TRESHOLD ){
								blindsFailsafe();
							}else{
							txMsg.Data.bytes[0] = boardTemperature & 0x00FF;
							txMsg.Data.bytes[1] = (boardTemperature & 0xFF00)>>8;
							txMsg.Data.bytes[2] = 0x00;
							txMsg.Data.bytes[3] = getPosition(1);
							txMsg.Data.bytes[4] = getPosition(2);
							txMsg.Data.bytes[5] = getPosition(3);
							txMsg.DataLength = 6;
							BIOS_CanSend(&txMsg);
						}*/
					}
				}
			}

			rxMsgFull = 0;
		}

		if( (Timebase_CurrentTime() - timeStamp)  >= STATUS_SEND_PERIOD ){
			timeStamp = Timebase_CurrentTime();
            // Send blinds status to CAN periodically

			boardTemperature = getTC1047temperature();

			if( (int32_t)boardTemperature >= FAILSAFE_TRESHOLD ){
				blindsFailsafe();
				}else{
				txMsg.Data.bytes[0] = boardTemperature & 0x00FF;
				txMsg.Data.bytes[1] = (boardTemperature & 0xFF00)>>8;
				txMsg.Data.bytes[2] = 0x00;
				txMsg.Data.bytes[3] = getPosition(1);
				txMsg.Data.bytes[4] = getPosition(2);
				txMsg.Data.bytes[5] = getPosition(3);
				txMsg.DataLength = 6;

				BIOS_CanSend(&txMsg);
			}
		}
        // If start turning servo
		if( turnDirection ){
			if( Timebase_CurrentTime() - timeStampTurn >= TURN_PERIOD ){
				timeStampTurn = Timebase_CurrentTime();
				// Clockwise or counterclockwise?
				if( turnDirection == BLINDS_TURN_CCW ){
					alterPosition( -STEPS_PER_TURN_PERIOD , servoToTurn);
				}else if( turnDirection == BLINDS_TURN_CW ){
					alterPosition( STEPS_PER_TURN_PERIOD , servoToTurn);
				}
				servoFeed_timeStamp = Timebase_CurrentTime();
			}
		}

		// Turn off the servo current feed after timeout
		if( Timebase_CurrentTime() - servoFeed_timeStamp >= SERVO_FEED_TIME ){
			SERVO_FEED_PORT &= ~(1<<SERVO_FEED_PIN);
			servoDisable();
		}else{
			SERVO_FEED_PORT |= (1<<SERVO_FEED_PIN);
			servoEnable();
		}
	}
	return 0;
}

void blindsFailsafe()
{
/*	uint32_t boardTemperature = 0, timeStamp = 0;

	// Turn of servo current feed
	SERVO_FEED_PORT &= ~(1<<SERVO_FEED_PIN);


	while(1){
		// check if any messages have been received
		while(rxMsgFull) {
			// This relay is adressed
			if(rxMsg.Data.bytes[0] == BLINDS_CMD_RESET ){
				// Return from failsafe mode
				return;
			}
		}

        if( Timebase_CurrentTime() - timeStamp >= FAILSAFE_SEND_PERIOD ){
            timeStamp = Timebase_CurrentTime();
            // Send relay status to CAN
            boardTemperature = getTC1047temperature();

            txMsg.Data.bytes[0] = boardTemperature & 0x00FF;
            txMsg.Data.bytes[1] = (boardTemperature & 0xFF00)>>8;
            txMsg.Data.bytes[2] = FAILSAFE_MODE;
#if NUMBER_OF_RCS >0
            txMsg.Data.bytes[3] = getPosition(1);
#endif
#if NUMBER_OF_RCS >1
            txMsg.Data.bytes[4] = getPosition(2);
#endif
#if NUMBER_OF_RCS >2
            txMsg.Data.bytes[5] = getPosition(3);
#endif
            txMsg.DataLength = NUMBER_OF_RCS+3;

			BIOS_CanSend(&txMsg);
        }
    }*/
}

