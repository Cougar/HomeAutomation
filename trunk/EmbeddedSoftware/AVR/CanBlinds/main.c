/**
 * CanBlinds.
 * Takes incomming packages and adjusts PWM output.
 * Input: -128 to +127
 * Position msg: <byte0 BLINDS_CMD> <byte1 position> <byte2 servo number>
 * Turning msg: <byte0 START/STOP> <byte 1 direction> <byte2 servo number>
 *
 * @date    2007-01-01
 * @author  Erik Larsson
 *
 */

//#define TWO_BUTTON_MODE /* If using two (or just one but nothing more) auxiliary buttons */

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
#include <rc_servo.h>

#include <eqlazer_funcdefs.h>

/* defines */
#define GROUP_ID	0x01L // FIXME

#define FAILSAFE_MODE       0x0F
#define FAILSAFE_TRESHOLD   60  /* Degrees celcius when nodeBlinds goes into failsafe mode */

#define BLINDS_CMD_ABS      0x01 /* Absolute position */
#define BLINDS_CMD_REL      0x02 /* Relative position */
#define BLINDS_CMD_START    0x03 /* Start turning RC-servo */
#define BLINDS_CMD_STOP     0x04 /* Stop turning RC-servo */
#define BLINDS_CMD_RESET    0x05
#define BLINDS_CMD_STATUS   0x06

#define BLINDS_TURN_STOP    0x00
#define BLINDS_TURN_LEFT    0x01
#define BLINDS_TURN_RIGHT   0x02

#define STEPS_PER_TURN_PERIOD   10 /* number of steps for each adjustment */
#define TURN_PERIOD             100 /* ms */
#define STATUS_SEND_PERIOD      5000 /* ms */
#define FAILSAFE_SEND_PERIOD    10000 /* ms */
#define BOUNCE_TIME             10 /* ms */
#define SERVO_FEED_TIME			2000 /* ms */
#define BUTTON1                 1
#define BUTTON2                 2

#define SERVO_FEED_PORT			PORTC
#define SERVO_FEED_DDR			DDRC
#define SERVO_FEED_PIN			PC0

#define TRUE 1
#define FALSE !TRUE

/*-------------------------------------------------------------------------
 * Global variables
 * -----------------------------------------------------------------------*/
/*#if defined(TWO_BUTTON_MODE)
Can_Message_t txMsg_Button;
#endif*/

uint8_t msg_received = FALSE;

Can_Message_t txMsg;
Can_Message_t rxMsg;

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
void blindsFailsafe();

/* Takes care about incoming messages and parses them if this node is adressed */
void can_receive(Can_Message_t *msg){
	uint32_t act;
	uint8_t m, act_type, group, node;
	if( ((msg->Id & CLASS_MASK) >> CLASS_MASK_BITS) == CLASS_ACT ){
		act = (msg->Id & TYPE_MASK) >> TYPE_MASK_BITS;
		act_type = (act & ACT_TYPE_MASK) >> ACT_TYPE_BITS;
		group = (act & ACT_GROUP_MASK) >> ACT_GROUP_BITS;
		node = (act & ACT_NODE_MASK) >> ACT_NODE_BITS;

		/* Check if it is command to control this servo */
		if(act_type == ACT_TYPE_SERVO){
			if(group == GROUP_ID || node == NODE_ID){
				for(m=0;m<msg->DataLength;m++){
					rxMsg.Data.bytes[m] = msg->Data.bytes[m];
				}
				msg_received = TRUE;
			}
		}
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

    uint8_t //blindsStatus, /* Position (-128 to 127) */
            servoToTurn,
            turnDirection = BLINDS_TURN_STOP; /* Specifies direction to turn or stopped */
    uint32_t boardTemperature = 0;
    uint32_t timeStamp = 0, timeStampTurn = 0, servoFeed_timeStamp = 0;

	sei();
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
	SERVO_FEED_DDR |= (1<<SERVO_FEED_PIN);
	SERVO_FEED_PORT &= ~(1<<SERVO_FEED_PIN);

	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.Id = (CLASS_SNS << CLASS_MASK_BITS)|(SNS_ACT_STATUS_SERVO << TYPE_MASK_BITS)|(NODE_ID << SID_MASK_BITS);

	/* Set up callback for CAN messages */
	bios->can_callback = &can_receive;

    /* main loop */
	while (1) {
		/* check if any messages have been received */
		if(msg_received){
			/* This node that control a servo is adressed */
			if( rxMsg.Data.bytes[0] == BLINDS_CMD_ABS ){
				/* Set absolute position to servo */
				setPosition( rxMsg.Data.bytes[1], rxMsg.Data.bytes[2] );
				servoFeed_timeStamp = bios->timebase_get();

			}else if(rxMsg.Data.bytes[0] == BLINDS_CMD_REL){
                /* Set relative position to servo */
                alterPosition( rxMsg.Data.bytes[1], rxMsg.Data.bytes[2] );
				servoFeed_timeStamp = bios->timebase_get();

            }else if(rxMsg.Data.bytes[0] == BLINDS_CMD_START ){
                /* Start turning the servo */
                turnDirection = rxMsg.Data.bytes[1];
                servoToTurn = rxMsg.Data.bytes[2];
				servoFeed_timeStamp = bios->timebase_get();

            }else if(rxMsg.Data.bytes[0] == BLINDS_CMD_STOP ){
                /* Stop turning */
                turnDirection = BLINDS_TURN_STOP;

            }else if(rxMsg.Data.bytes[0] == BLINDS_CMD_STATUS){
                /* Send blinds status to CAN */
                boardTemperature = getTC1047temperature();

                if( (int32_t)boardTemperature >= FAILSAFE_TRESHOLD ){
                    blindsFailsafe();
                }else{
                    txMsg.Data.bytes[0] = boardTemperature & 0x00FF;
                    txMsg.Data.bytes[1] = (boardTemperature & 0xFF00)>>8;
                    txMsg.Data.bytes[2] = 0x00;
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
					bios->can_send(&txMsg);
                    }
                }
				msg_received = FALSE;
        }

		if( (bios->timebase_get() - timeStamp)  >= STATUS_SEND_PERIOD ){
			timeStamp = bios->timebase_get();
            /* Send blinds status to CAN */

            boardTemperature = getTC1047temperature();

            if( (int32_t)boardTemperature >= FAILSAFE_TRESHOLD ){
                blindsFailsafe();
            }else{
                txMsg.Data.bytes[0] = boardTemperature & 0x00FF;
                txMsg.Data.bytes[1] = (boardTemperature & 0xFF00)>>8;
                txMsg.Data.bytes[2] = 0x00;
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

				bios->can_send(&txMsg);
            }
        }
        /* If start turning servo */
		if( turnDirection ){
            if( bios->timebase_get() - timeStampTurn >= TURN_PERIOD ){
				timeStampTurn = bios->timebase_get();
                /* Clockwise or counterclockwise? */
                if( turnDirection == BLINDS_TURN_LEFT ){
                    alterPosition( -STEPS_PER_TURN_PERIOD , servoToTurn);
                }else if( turnDirection == BLINDS_TURN_RIGHT ){
                    alterPosition( STEPS_PER_TURN_PERIOD , servoToTurn);
                }
            }
        }
		/* Turn off the servo current feed */
		if( bios->timebase_get() - servoFeed_timeStamp >= SERVO_FEED_TIME ){
			SERVO_FEED_PORT &= ~(1<<SERVO_FEED_PIN);
		}else{
			SERVO_FEED_PORT |= (1<<SERVO_FEED_PIN);
		}
    }
    return 0;
}

void blindsFailsafe()
{
	uint8_t relayStatus;
	uint32_t boardTemperature = 0, timeStamp = 0;

	/* Turn of servo current feed */
	SERVO_FEED_PORT &= ~(1<<SERVO_FEED_PIN);


	while(1){
		/* check if any messages have been received */
		while(msg_received) {
			/* This relay is adressed*/
			if(rxMsg.Data.bytes[0] == BLINDS_CMD_RESET ){
				/* Return from failsafe mode */
				return;
			}
		}

        if( bios->timebase_get() - timeStamp >= FAILSAFE_SEND_PERIOD ){
            timeStamp = bios->timebase_get();
            /* Send relay status to CAN */
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

			bios->can_send(&txMsg);
        }
    }
}

