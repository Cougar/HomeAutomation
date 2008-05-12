/**
 * CAN Fancontroller. This application is used to control fans via PWM.
 * Might also add some sort of display if the pincount is high enough
 * Or temperature sensors. who knows :o
 * 
 * @date	2008-01-10
 * @author	Martin Nordén
 *
 * TODO: Everything.
 */


/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>
#include <avr/pgmspace.h> //To allow read/write from flash

#include <math.h>

#include <drivers/timer/timebase.h>

#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.

#include <string.h> //for memcpy
/* lib files */
#include <bios.h>
//#include <funcdefs/funcdefs.h>

//#include <settings.h>

/*----------------------------------------------------------------------------
 * Defines
 * -------------------------------------------------------------------------*/

//Working on implementing all of these...
#define FANCONT_CMD_STATUS   		0x00	/* Get status */
#define FANCONT_CMD_SETSPEED   		0x01	/* Set Fan Speed */
#define FANCONT_CMD_SETFREQENCY		0x02	/* Get PWM Frequency */



#define APP_TYPE    CAN_APPTYPES_FANCONTROLLER
#define APP_VERSION 0x0001

#define TRUE 1
#define FALSE !TRUE


/*-----------------------------------------------------------------------------
 * Global variables
 *---------------------------------------------------------------------------*/
uint8_t DutyCycle;	//Duty cycle	
uint32_t timeStamp;

Can_Message_t txMsg;	//Transmit message storage

volatile Can_Message_t rxMsg; // Message storage
volatile uint8_t rxMsgFull;   // Synchronization flag

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/

/* Takes care of incoming messages and parses them if this node is adressed */
void can_receive(Can_Message_t *msg)
{ // CAN callback function
	if (!rxMsgFull) {
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}


/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {

//Setting up OC1A, 16 bit counter used as 8 bit.
	TCCR1A |= (1<<COM1A1) | (1<<WGM11); /* Set OC1A at TOP, mode 14 */
	TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS12) | (0<<CS11)| (1<<CS10); /* Timer uses main system clock with ? prescale */
	ICR1 = 256; //8 bit precision to match the other two counters.
	OCR1A = 0;  //0% as standard
	DDRB |= (1<<PB1); /* Enable pin as output */
//Setting up OC0A
	TCCR0A |= (1<<COM0A1)|(1<<WGM01)|(1<<WGM00); /* Set OC0A at TOP, Mode 7 */
	TCCR0B |= (1<<CS01)|  (1<<CS00); /* Prescaler updating now */
	OCR0A = 0;	//0% standard
	DDRD |= (1<<PD6);
//Setting up OC0B
	TCCR0A |= (1<<COM0B1);
	OCR0B = 0;
	DDRD |= (1<<PD5);

//Initialize variables
	timeStamp = Timebase_CurrentTime();
	sei();

	BIOS_CanCallback = &can_receive;


//Send a message that we are alive!
	txMsg.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) | (NODE_ID << CAN_SHIFT_NMT_SID);
	txMsg.DataLength = 4;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.Data.words[0] = APP_TYPE;
	txMsg.Data.words[1] = APP_VERSION;
	BIOS_CanSend(&txMsg);

//Lets get timebase up and running
	Timebase_Init();


	/* main loop */
	while (1) {
		
		/* Time to fade color? */
		if ((Timebase_CurrentTime() - timeStamp) >= 100) {
			//OCR1A++;
			//if (OCR1A>150) OCR1A = 50;
			timeStamp = Timebase_CurrentTime();
		}


		/* Check if any messages has been recieved */
		if (rxMsgFull) {
			uint16_t acttype;
			uint8_t fancontid;			
		
			if ( ((rxMsg.Id & CAN_MASK_CLASS)>>CAN_SHIFT_CLASS) == CAN_ACT){
				// Actuator package
				acttype =(uint16_t)((rxMsg.Id & CAN_MASK_ACT_TYPE) >> CAN_SHIFT_ACT_TYPE);
				fancontid = (uint8_t)((rxMsg.Id & CAN_MASK_ACT_ID) >> CAN_SHIFT_ACT_ID);
				
				if ((acttype == ACT_TYPE_FAN) && (fancontid == 1)) {															
					// This is a message for this fancontroller
						
					
					if( rxMsg.Data.bytes[0] == FANCONT_CMD_SETSPEED ){
						/* Set duty cycle */
						OCR1A =  rxMsg.Data.bytes[1];
					}
					
					
				}
			}
    		rxMsgFull = 0; //  
    		rxMsgFull = 0; //  

		}
		/* Done checking incoming packages */

	}
	
	return 0;
}
