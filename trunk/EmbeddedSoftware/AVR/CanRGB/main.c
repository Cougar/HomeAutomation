/**
 * CAN RGB LED. This application is used to control a RGB LED.
 * Still under heavy development
 * 
 * @date	2007-02-04
 * @author	Martin Nordén
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
#include <math.h>

/*-----------------------------------------------------------------------------
 * Variables
 *---------------------------------------------------------------------------*/
uint32_t timeStamp;

uint8_t currR;	//Current Red
uint8_t currG;	//Current Green
uint8_t currB;	//Current Blue

uint8_t destR;	//Destination Red
uint8_t destG;	//Destination Green
uint8_t destB;	//Destination Blue

uint8_t intensity;	//Current intensity.

uint16_t fadeSpeed;	//Fadingspeed. 16bit for reaaaally slow fading
uint8_t program;		//This variable keeps track of what program is currently running. 0 means no program.
uint8_t programstate;	//This variable keeps track of where in a program we currently are.

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
void updateLED();
void reformatRGB(uint8_t R, uint8_t G, uint8_t B);


/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
	Timebase_Init();
	Serial_Init();
	Can_Init();

//Setting up OC1A, 16 bit counter used as 8 bit.
	/* Use OC1A */
	TCCR1A |= (1<<COM1A1) | (1<<WGM11); /* Set OC1A at TOP, mode 14 */
	TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS11); /* Timer uses main system clock with 1/8 prescale */
	ICR1 = 256; //8 bit precision to match the other two counters.
	OCR1A = 128;  //50% as standard
	DDRB |= (1<<PB1); /* Enable pin as output */

//Initialize variables
   	fadeSpeed = 250;
	timeStamp = 0;

	Can_Message_t rxMsg;
	currR = 0x53;
	currG = 0x15;
	currB = 0xa0;

	sei();	

	reformatRGB(currR,currG,currB);

	/* main loop */
	while (1) {
		/* service the CAN routines */
		Can_Service();

		/* Time to see if we want to update our values */
		if (Timebase_PassedTimeMillis(timeStamp) >= fadeSpeed) {
			intensity = intensity -1;
			timeStamp = Timebase_CurrentTime();
			updateLED();
		}


		/* check if any messages have been received */
		/* A lot of remote control crap going on here now for testing */
		while (Can_Receive(&rxMsg) == CAN_OK) {
			if ((rxMsg.Data.bytes[0] == 0x0)&(rxMsg.Data.bytes[3] == 0x20)){
				program = 1;
				fadeSpeed = 0;
			}

			if ((rxMsg.Data.bytes[0] == 0x0)&(rxMsg.Data.bytes[3] == 0x21)){
				program = 2;
				fadeSpeed = 0;
			}
		}


	}
	
	return 0;
}





void updateLED(){

OCR1A = currR * intensity / 255;

	Can_Message_t txMsg;
	txMsg.Id = 0x55;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.DataLength = 4;
	
	txMsg.Data.bytes[0] = currR * intensity / 255;
	txMsg.Data.bytes[1] = currG * intensity / 255;
	txMsg.Data.bytes[2] = currB * intensity / 255;
	txMsg.Data.bytes[3] = intensity;

	Can_Send(&txMsg);



//Red
//Green
//Blue
}


void reformatRGB(uint8_t R, uint8_t G, uint8_t B){
	uint8_t maxvalue;
	uint16_t tempcontainer;

	if ((R>G)&(R>B)){
		maxvalue = R;
	} else if ((G>R)&(G>B)){
		maxvalue = G;
	} else {
		maxvalue = B;
	}

	tempcontainer = R * 255;
	currR = tempcontainer/maxvalue;
	tempcontainer = G * 255;
	currG = tempcontainer/maxvalue;
	tempcontainer = B * 255;
	currB = tempcontainer/maxvalue;

	intensity = maxvalue;

}
