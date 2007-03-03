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
#include <avr/wdt.h>
/* lib files */
#include <bios.h>
#include <tc1047.h>
#include <funcdefs.h>

/* defines */
#define GROUP_ID		0x01UL

#define RELAY_OFF			0x01UL
#define RELAY_ON			0x02UL
#define FAILSAFE_MODE		0x0FUL
#define FAILSAFE_TRESHOLD	60  /* Degrees when nodeRelay goes into failsafe mode */

#define RELAY_CMD_ON		0x01UL
#define RELAY_CMD_OFF		0x02UL
#define RELAY_CMD_TOGGLE	0x03UL
#define RELAY_CMD_STATUS	0x04UL
#define RELAY_CMD_RESET		0x05UL
#define STATUS_SEND_PERIOD	10000 /* milliseconds */
#define FAILSAFE_SEND_PERIOD	10000 /* milliseconds */
#define BOUNCE_TIME			10 /* milliseconds */
#define BUTTON1				1
#define BUTTON2				2

/*-------------------------------------------------------------------------
 * Global variables
 * -----------------------------------------------------------------------*/
#if TWO_BUTTON_MODE
Can_Message_t txMsg_Button;
#endif


uint8_t relayStatus, failsafe_flag = 0;
uint32_t boardTemperature = 0;
Can_Message_t txMsg;

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
uint8_t relayOff();
uint8_t relayOn();
void relayFailsafe();

void can_receive(Can_Message_t *msg){
	uint32_t act;
	uint32_t m, act_type, group, node;

	if( ((msg->Id & CLASS_MASK)>>CLASS_MASK_BITS) == CLASS_ACT){

		act = (msg->Id & TYPE_MASK) >> TYPE_MASK_BITS;
		act_type = (act & ACT_TYPE_MASK) >> ACT_TYPE_BITS;
		group = (act & ACT_GROUP_MASK) >> ACT_GROUP_BITS;
		node = (act & ACT_NODE_MASK) >> ACT_NODE_BITS;

		if( act_type == ACT_TYPE_RELAY && (group == GROUP_ID || node == NODE_ID) ){
			if(msg->Data.bytes[0] == RELAY_CMD_ON && !failsafe_flag){
				// Turn relay on
				relayStatus = relayOn();

			}else if(msg->Data.bytes[0] == RELAY_CMD_OFF && !failsafe_flag){
				// Turn relay off
				relayStatus = relayOff();
				txMsg.Id = 0x8000111UL; // FIXME
				bios->can_send(&txMsg);

			}else if(msg->Data.bytes[0] == RELAY_CMD_TOGGLE && !failsafe_flag){
					// Toggle relay
					if(relayStatus == RELAY_OFF){
						relayStatus = relayOn();
					}else{
						relayStatus = relayOff();
						txMsg.Id = 0x8000113UL; // FIXME
						bios->can_send(&txMsg);
					}

			}else if(msg->Data.bytes[0] == RELAY_CMD_STATUS && !failsafe_flag){
				// Send relay status to CAN
				boardTemperature = getTC1047temperature();

				if( (int32_t)boardTemperature >= FAILSAFE_TRESHOLD ){
					// Goto failsafe mode
					relayFailsafe();
				}else{
					// Transmitt node status
					txMsg.Data.bytes[0] = boardTemperature & 0x00FF;
					txMsg.Data.bytes[1] = (boardTemperature & 0xFF00)>>8;
					txMsg.Data.bytes[2] = relayStatus;
					txMsg.Data.bytes[3] = 0;
					txMsg.Id = ( CLASS_SNS<<CLASS_MASK_BITS )|( SNS_ACT_STATUS_RELAY<<TYPE_MASK_BITS )|(NODE_ID);
					bios->can_send( &txMsg );
				}
			}else if(msg->Data.bytes[0] == RELAY_CMD_RESET){
				// Return to normal operation
				failsafe_flag = 0;
			}
		}
	}
}

/*----------------------------------------------------------------------------
 * Interrupt routines // FIXME ordna och testa tryckknappar/rotary encoder
 * -------------------------------------------------------------------------*/
#if TWO_BUTTON_MODE
ISR( PCINT2_vect )
{
    /* 
     * Auxiliary button pressed and released
     * Check time between press and release to eliminate bounces
     */
    static uint32_t buttonTime[2];

    txMsg_Button.RemoteFlag = 0;
    txMsg_Button.ExtendedFlag = 1;

    if( (PIND & (1<<PD6)) == 0){
        /* Button pressed */
        buttonTime[0] = Timebase_CurrentTime();
    }else if( (PIND & (1<<PD6)) == 1){
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
    if( (PIND & (1<<PD7)) == 0){
        /* Button pressed */
        buttonTime[1] = Timebase_CurrentTime();
    }else if( (PIND & (1<<PD7)) == 1){
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

	adcTemperatureInit();

#if TWO_BUTTON_MODE
#error two button mode not yet tested
    /*
     * Initialize buttons
     * It is possible to use ie. sensors instead
     * but then change this
     */
    // Set as input and enable pull-up
    DDRD &= ~( (1<<DDD6)|(1<<DDD7) );
    PORTD |= (1<<PD6)|(1<<PD7);

    // Enable IO-pin interrupt
    PCICR |= (1<<PCIE1);
    // Unmask PD6 and PD7
    PCMSK2 |= (1<<PCINT22) | (1<<PCINT23);
#endif

	// Control channel for relay
	DDRC |= (1<<DDC1);

	// Set up callback for CAN messages
//	bios->can_callback = &can_receive;

	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.DataLength = 4;

	sei();

	// Turn relay off
	relayStatus = relayOff();
	txMsg.Id = 0x8000110UL; // FIXME
	bios->can_send(&txMsg);

	uint32_t timeStamp = bios->timebase_get(), temp;


	// Main loop
	while (1) {

		temp = bios->timebase_get();
		if( temp - timeStamp >= STATUS_SEND_PERIOD ){
			timeStamp = temp;
			// Send relay status to CAN

//			boardTemperature = getTC1047temperature();

//			if( (int32_t)boardTemperature >= FAILSAFE_TRESHOLD ){
//				relayFailsafe();
//			}else{
				txMsg.Data.bytes[0] = timeStamp & 0x000000FF; //boardTemperature & 0x00FF;
				txMsg.Data.bytes[1] = (timeStamp & 0x0000FF00)>>8; //(boardTemperature & 0xFF00)>>8;
				txMsg.Data.bytes[2] = (timeStamp & 0x00FF0000)>>16; //relayStatus;
				txMsg.Data.bytes[3] = (timeStamp & 0xFF000000)>>24; //0;
				txMsg.Id = ( CLASS_SNS<<CLASS_MASK_BITS )|( SNS_ACT_STATUS_RELAY<<TYPE_MASK_BITS )|(NODE_ID);
				bios->can_send( &txMsg );
//			}
		}
	}
	return 0;
}

uint8_t relayOff()
{
	// Turn off relay
	PORTC &= ~(1<<PC1);
	DDRC |= (1<<DDC1); // FIXME

	return RELAY_OFF;
}

uint8_t relayOn()
{
	// Turn on relay
	DDRC &= ~(1<<DDC1); // FIXME
	PORTC |= (1<<PC1);

	return RELAY_ON;
}

void relayFailsafe()
{
	uint32_t timeStamp = 0;

	relayStatus = relayOff();
	failsafe_flag = 1;

	while(1){

		if( !failsafe_flag ){
			// Return from failsafe mode
			return;
		}

		if( bios->timebase_get() - timeStamp >= FAILSAFE_SEND_PERIOD ){
			timeStamp = bios->timebase_get();
			// Send relay status to CAN
			boardTemperature = getTC1047temperature();

			txMsg.Data.bytes[0] = boardTemperature & 0x00FF;
			txMsg.Data.bytes[1] = (boardTemperature & 0xFF00)>>8;
			txMsg.Data.bytes[2] = relayStatus;
			txMsg.Data.bytes[3] = FAILSAFE_MODE;
			txMsg.Id = ( CLASS_SNS<<CLASS_MASK_BITS )|( SNS_ACT_STATUS_RELAY<<TYPE_MASK_BITS )|(NODE_ID);

			bios->can_send( &txMsg );
		}
	}
}

