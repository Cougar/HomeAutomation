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

uint32_t tempStamp;	//Timestamp to keep track of temp things during development
uint8_t temp;

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
void setDestRGB(uint8_t R, uint8_t G, uint8_t B);

void changeColor(int8_t amount, uint8_t* color);

uint8_t fade(uint8_t current, uint8_t destination);


/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
	Timebase_Init();
	Serial_Init();
	Can_Init();

//Setting up OC1A, 16 bit counter used as 8 bit.
	TCCR1A |= (1<<COM1A1) | (1<<WGM11); /* Set OC1A at TOP, mode 14 */
	TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS11); /* Timer uses main system clock with 1/8 prescale */
	ICR1 = 256; //8 bit precision to match the other two counters.
	OCR1A = 0;  //0% as standard
	DDRB |= (1<<PB1); /* Enable pin as output */

//Setting up OC0A
    TCCR0A |= (1<<COM0A1)|(1<<WGM01)|(1<<WGM00); /* Set OC0A at TOP, Mode 7 */
    TCCR0B |= (1<<CS01)|(1<<CS00); /* Prescaler ???? Check datasheet */
    OCR0A = 0;	//0% standard
    DDRD |= (1<<PD6);

//Setting up OC0B
    TCCR0A |= (1<<COM0B1);
    OCR0B = 0;
    DDRD |= (1<<PD5);


//Initialize variables
   	fadeSpeed = 10;
	dimSpeed = 255;
	timeStamp = 0;
	dimStamp = 0;
	Can_Message_t rxMsg;

	temp = 0;
	
	sei();	

	reformatRGB(0x00,0xff,0x00);
	currR = destR;
	currG = destG;
	currB = destB;
	currIntens = 100;
	destIntens = 100;

	program = 0;


	/* main loop */
	while (1) {
		/* service the CAN routines */
		Can_Service();

		/* Time to fade color? */
		if (Timebase_PassedTimeMillis(timeStamp) >= fadeSpeed) {			

			changeColor(1, &destB);
			changeColor(-1, &destG);

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


		/* Just a nice fade-demo */
		if ((Timebase_PassedTimeMillis(tempStamp) >= 2500)&(program == 1)) {
			tempStamp = Timebase_CurrentTime();
			
			switch(temp){
			case 1:
				setDestRGB(0xff,0x00,0x00);
				break;
			case 2:
				setDestRGB(0xff,0xff,0x00);
				break;
			case 3:
				setDestRGB(0x00,0xff,0x00);
				break;
			case 4:
				setDestRGB(0x00,0xff,0xff);
				break;
			case 5:
				setDestRGB(0x00,0x00,0xff);
				break;
			case 6:
				setDestRGB(0xff,0x00,0xff);
				temp = 255;
				break;
			}
			temp++;
				
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
	uint16_t tempcontainer;

	tempcontainer = currR * currIntens / 255;	
	OCR1A = tempcontainer;
	tempcontainer = currG * currIntens / 255;	
      OCR0A = tempcontainer;
	tempcontainer = currB * currIntens / 255;		
	OCR0B = tempcontainer;

	
	//Bara för debugcrap
	Can_Message_t txMsg;
	txMsg.Id = 0x55;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.DataLength = 4;
	
	txMsg.Data.bytes[0] = currR;// * currIntens / 255;
	txMsg.Data.bytes[1] = currG;// * currIntens / 255;
	txMsg.Data.bytes[2] = currB;// * currIntens / 255;
	txMsg.Data.bytes[3] = currIntens;
	Can_Send(&txMsg);
}


/* setDestRGB sets a new target RGB triplet to fade to. Note that the brightness is kept at approximatly the same level
 * even though the color changes */

void setDestRGB(uint8_t R, uint8_t G, uint8_t B){
	uint8_t maxvalue;
	uint16_t tempcontainer;

	if ((R>=G)&(R>=B)){
		maxvalue = R;
	} else if ((G>=R)&(G>=B)){
		maxvalue = G;
	} else {
		maxvalue = B;
	}

	tempcontainer = R * 255;
	destR = tempcontainer/maxvalue;
	tempcontainer = G * 255;
	destG = tempcontainer/maxvalue;
	tempcontainer = B * 255;
	destB = tempcontainer/maxvalue;

}

/* reformatRGB sets a new target RGB triplet to fade to. Note that the brightness is recalculated. */

void reformatRGB(uint8_t R, uint8_t G, uint8_t B){
	uint8_t maxvalue;
	uint16_t tempcontainer;

	if ((R>=G)&(R>=B)){
		maxvalue = R;
	} else if ((G>=R)&(G>=B)){
		maxvalue = G;
	} else {
		maxvalue = B;
	}

	tempcontainer = R * 255;
	destR = tempcontainer/maxvalue;
	tempcontainer = G * 255;
	destG = tempcontainer/maxvalue;
	tempcontainer = B * 255;
	destB = tempcontainer/maxvalue;

	destIntens = maxvalue;
}

uint8_t fade(uint8_t current, uint8_t destination){
	if (current > destination){
		return(current - 1);
	} else if (current < destination){
		return(current + 1);
	}
	return(current);
}

/*
 * changeColor ändrar en färg mängden amount. -127>127.
 * color kan vara dest eller curr R,G,B.
 * Ev. TODO: om t.ex. röd är på max och man vill öka röd ska de andra färgerna minskas.	*/
void changeColor(int8_t amount, uint8_t* color){
	if ( ((amount > 0)&(*color < 255)) || ((amount < 0)&(*color > 0)) ) //Se till att vi inte slår över *color
	*color = *color + amount;
}
