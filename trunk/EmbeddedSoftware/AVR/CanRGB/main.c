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
uint32_t timeStamp;	//TimeStamp to keep track of time
uint32_t dimStamp;	//Timestamp to keep track of dimmer changes

uint8_t currR;	//Current Red 0->255
uint8_t currG;	//Current Green 0->255
uint8_t currB;	//Current Blue 0->255

uint8_t destR;	//Destination Red
uint8_t destG;	//Destination Green
uint8_t destB;	//Destination Blue

uint8_t currIntens;	//Current intensity. 0->255
uint8_t destIntens;	//Destination intensity. 0->255

uint16_t fadeSpeed;	//Fadingspeed. 16bit for reaaaally slow fading. Sets the speed of colorchange.
uint8_t dimSpeed;		//Dimmer speed. Sets the speed of intensitychange.

uint8_t program;		//This variable keeps track of what program is currently running. 
uint8_t programstate;	//This variable keeps track of where in a program we currently are. Will probably be removed later on.

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
void updateLED();
void reformatRGB(uint8_t R, uint8_t G, uint8_t B);
uint8_t fade(uint8_t current, uint8_t destination);


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
	OCR1A = 1;  //50% as standard
	DDRB |= (1<<PB1); /* Enable pin as output */

/* Use OC0A */
    TCCR0A |= (1<<COM0A1)|(1<<WGM01)|(1<<WGM00); /* Set OC0A at TOP, Mode 7 */
    TCCR0B |= (1<<CS02);//(1<<CS02)|(1<<CS00); /* Prescaler 1/1024 */
    OCR0A = 40;
    DDRD |= (1<<PD6);

/* Use OC0B */
    TCCR0A |= (1<<COM0B1);
    OCR0B = 40;
    DDRD |= (1<<PD5);

//Initialize variables
   	fadeSpeed = 250;
	dimSpeed = 255;
	timeStamp = 0;
	dimStamp = 0;

	Can_Message_t rxMsg;
	currR = 0x64;
	currG = 0x15;
	currB = 0x63;

	sei();	

	reformatRGB(currR,currG,currB);

	currIntens = 20;
	destIntens = 200;

	/* main loop */
	while (1) {
		/* service the CAN routines */
		Can_Service();

		/* Time to fade color? */
		if (Timebase_PassedTimeMillis(timeStamp) >= fadeSpeed) {
			currR = fade(currR, destR);
			currG = fade(currG, destG);
			currB = fade(currB, destB);

			timeStamp = Timebase_CurrentTime();
			updateLED();
		}


		/* Time to fade intensity? */
		if (Timebase_PassedTimeMillis(dimStamp) >= dimSpeed) {
			currIntens = fade(currIntens, destIntens);
			dimStamp = Timebase_CurrentTime();
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

//	OCR1A = currR * currIntens / 255;
	//TODO: Här ska givetvis de andra två färgerna också uppdateras.

	
	//Bara för debugcrap
	Can_Message_t txMsg;
	txMsg.Id = 0x55;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.DataLength = 4;
	
	txMsg.Data.bytes[0] = currR * currIntens / 255;
	txMsg.Data.bytes[1] = currG * currIntens / 255;
	txMsg.Data.bytes[2] = currB * currIntens / 255;
	txMsg.Data.bytes[3] = currIntens;

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

	currIntens = maxvalue;
}

uint8_t fade(uint8_t current, uint8_t destination){
	if (current > destination){
		return(current - 1);
	} else if (current < destination){
		return(current + 1);
	}
	return(current);
}
