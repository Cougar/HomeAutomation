/**
 * CAN RGB LED. This application is used to control a RGB LED.
 * Still under heavy development
 * 
 * @date	2007-02-04
 * @author	Martin Nordén
 *   
 * TODO: A lot, currently trying to get some kind of ready-made programs to work.
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
#include <math.h> //Not sure this is needed anymore..
#include <avr/pgmspace.h> //To allow read/write from flash


/*----------------------------------------------------------------------------
 * Ehm, struct?
 * -------------------------------------------------------------------------*/
typedef struct
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
	uint8_t Intens;
} Color;

/*-----------------------------------------------------------------------------
 * Programs! This will most likely be improooved, but I have to start somewhere.
 *---------------------------------------------------------------------------*/

/*
struct Color pgm1[] = {
{0x10,0x20,0x30,0x40},
{0x10,0x20,0x30,0x40},
{0x10,0x20,0x30,0x40}
};

struct Color pgm2[] = {
{0x20,0x20,0x30,0x50},
{0x20,0x20,0x30,0x50},
{0x20,0x20,0x30,0x50}
};

struct Color* programs[] PROGMEM = {
(struct Color*)pgm1,
(struct Color*)pgm2
};


uint16_t temp_address = pgm_read_word(&programs[1]);
*/

/*-----------------------------------------------------------------------------
 * Variables
 *---------------------------------------------------------------------------*/
uint32_t timeStamp;	//TimeStamp to keep track of time
uint32_t dimStamp;	//Timestamp to keep track of dimmer changes

uint32_t tempStamp;	//Timestamp to keep track of temp things during development
uint8_t temp;

Color currColor;  //Current color
Color destColor;  //Destination color

uint16_t fadeSpeed;	//Fadingspeed. 16bit for reaaaally slow fading. Sets the speed of colorchange.
uint8_t dimSpeed;		//Dimmer speed. Sets the speed of intensitychange.

uint8_t program;		//This variable keeps track of what program is currently running. 
uint8_t programstate;	//This variable keeps track of where in a program we currently are. Will probably be removed later on.

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
void updateLED();
void reformatRGB(uint8_t R, uint8_t G, uint8_t B, uint8_t setBright);
void changeColor(int8_t amount, uint8_t* color);
void fade(uint8_t* current, uint8_t* destination);

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
	Timebase_Init();
	Serial_Init();
	Can_Init();

//Setting up OC1A, 16 bit counter used as 8 bit.
	TCCR1A |= (1<<COM1A1) | (1<<WGM11); /* Set OC1A at TOP, mode 14 */
	TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS12); /* Timer uses main system clock with ? prescale */
	ICR1 = 256; //8 bit precision to match the other two counters.
	OCR1A = 0;  //0% as standard
	DDRB |= (1<<PB1); /* Enable pin as output */

//Setting up OC0A
	TCCR0A |= (1<<COM0A1)|(1<<WGM01)|(1<<WGM00); /* Set OC0A at TOP, Mode 7 */
	TCCR0B |= (1<<CS02); /* Prescaler updating now */
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

	reformatRGB(0x00,0xff,0x00,1);
	destColor.Intens = 100;
	currColor = destColor;

//Just temporary for now, they keep track of some kind of demo-program.
	program = 1;
	temp = 1;


	sei();

	/* main loop */
	while (1) {
		/* service the CAN routines */
		Can_Service();

		/* Time to fade color? */
		if (Timebase_PassedTimeMillis(timeStamp) >= fadeSpeed) {
			fade(&currColor.R, &destColor.R);			
			fade(&currColor.G, &destColor.G);
			fade(&currColor.B, &destColor.B);

			timeStamp = Timebase_CurrentTime();
			updateLED();
		}


		/* Time to fade intensity? */
		if (Timebase_PassedTimeMillis(dimStamp) >= dimSpeed) {
			fade(&currColor.Intens, &destColor.Intens);
			dimStamp = Timebase_CurrentTime();
			updateLED();
		}


		/* Program management */
		if (program != 0){ //Check if a program is currently running
			//if (currColor == destColor){ //Is the current sequence finished?
				//Set next state.
			//}
		}

		/* Just a nice fade-demo */
		if ((Timebase_PassedTimeMillis(tempStamp) >= 2500)&(program == 1)) {
			tempStamp = Timebase_CurrentTime();
			
			switch(temp){
			case 1:
				reformatRGB(0xff,0x00,0x00,0);
				break;
			case 2:
				reformatRGB(0xff,0xff,0x00,0);
				break;
			case 3:
				reformatRGB(0x00,0xff,0x00,0);
				break;
			case 4:
				reformatRGB(0x00,0xff,0xff,0);
				break;
			case 5:
				reformatRGB(0x00,0x00,0xff,0);
				break;
			case 6:
				reformatRGB(0xff,0x00,0xff,0);
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




/************************************************************************************************
 *     updateLED updates the PWM counter values and thus changes the color of the RGBLED.       *
 *     It doesn't reculculate anything, except the values to write to the PWM registers.        *
 *     TODO: Get rid of the tempcontainer!									*
 ************************************************************************************************/
void updateLED(){
	uint8_t tempcontainer;

	tempcontainer = currColor.R * currColor.Intens / 255;	//I really want to skip this, don't know how yet though.
	OCR1A = tempcontainer;	
      OCR0A = currColor.G * currColor.Intens / 255;
	OCR0B = currColor.B * currColor.Intens / 255;		
		
	//Bara för debugcrap
	Can_Message_t txMsg;
	txMsg.Id = 0x55;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.DataLength = 4;
	txMsg.Data.bytes[0] = currColor.R * currColor.Intens / 255;
	txMsg.Data.bytes[1] = currColor.G * currColor.Intens / 255;
	txMsg.Data.bytes[2] = currColor.B * currColor.Intens / 255;
	txMsg.Data.bytes[3] = currColor.Intens;
	Can_Send(&txMsg);
}



/************************************************************************************************
 *     reformatRGB sets a new target RGB triplet to fade to. It reformats it so that            *
 *     the highest color always gets 0xff. Brightness is recalculated.                          *
 *     if setBright is set to 0, the brightness will not be updated.					*
 *     TODO: Smarter 16-bit arithmetics would be nice. Also a pointer to struct to be updated.	*
 ************************************************************************************************/
void reformatRGB(uint8_t R, uint8_t G, uint8_t B, uint8_t setBright){
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
	destColor.R = tempcontainer/maxvalue;
	tempcontainer = G * 255;
	destColor.G = tempcontainer/maxvalue;
	tempcontainer = B * 255;
	destColor.B = tempcontainer/maxvalue;

	if (setBright){
		destColor.Intens = maxvalue;
	}
}


/************************************************************************************************
 *     fade tar in två värden, current och destination och för current ett steg                 *
 *     närmare destination.			                                                      *
 ************************************************************************************************/
void fade(uint8_t* current, uint8_t* destination){
	if (*current > *destination){
		*current = *current - 1;
	} else if (*current < *destination){
		*current = *current + 1;
	}
}


/************************************************************************************************
 *     changeColor ändrar en färg mängden amount. -127>127.                                     *
 *     color kan vara dest eller curr R,G,B.                                                    *
 *     Ev. TODO: om t.ex. röd är på max och man vill öka röd ska de andra färgerna minskas.	*
 ************************************************************************************************/
void changeColor(int8_t amount, uint8_t* color){
	if ( ((amount > 0)&(*color < 255)) || ((amount < 0)&(*color > 0)) ) //Se till att vi inte slår över *color
	*color = *color + amount;
}

