/**
 * CAN RGB LED. This version is not using BIOS and is therefor obselete. New version with bios is
 * quite a lot better.
 * 
 * @date	2007-02-10
 * @author	Martin Nordén
 *
 * Set RGB: <byte0 RGBLED_CMD_SETRGB> <byte 1 Set/Fade> <byte2 R> <byte3 G> <byte4 B>
 * Change RGB: <byte 0 RGBLED_CMD_CHANGERGB> <byte 1 int R> <byte 2 int G> <byte 3 int B>  (Note that they are all signed)
 * Set Intens: <byte0 RGBLED_CMD_SETINTENS> <byte1 Set/Fade> <byte2 Intens> (<byte3 dimSpeed>)
 *
 * TODO: Better CAN Integration, Failsafe mode, Tempsensors etc.
 */


/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdio.h>
#include <avr/pgmspace.h> //To allow read/write from flash
/* lib files */
#include <can.h>
#include <serial.h>
#include <timebase.h>
#include <math.h> //Not sure this is needed anymore..

/*----------------------------------------------------------------------------
 * Defines
 * -------------------------------------------------------------------------*/

//Not implemented yet. Will be implementet when I build a canUSB :)

#define RGBLED_CMD_GET      0x1400
#define RGBLED_CMD_SEND     0x1401

#define RGBLED_CMD_STATUS   	0x00	/* Get status */
#define RGBLED_CMD_SETRGB   	0x01	/* Set RGB Triplet */
#define RGBLED_CMD_CHANGERGB  0x02	/* Change RGB +/- */
#define RGBLED_CMD_SETINTENS  0x10  /* Set intensity */

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
 * Programs! There will be a better way to add programs here.. some day.
 *---------------------------------------------------------------------------*/

//Special combinations that means something special
//The combinations are useless for lighting anyway
//So we might as well use them!
//0x00, 0x00, 0x00, 0x00 means end of program, keep looping it
//0x00, 0x00, 0x00, 0x01 means end of program, halt 

Color pgm1[] PROGMEM = {
{0xff,0x00,0x00,0x64},
{0xff,0xff,0x00,0x10},
{0x00,0xff,0x00,0xa0},
{0x00,0xff,0xff,0x7f},
{0x00,0x00,0xff,0x7f},
{0xff,0x00,0xff,0x40},
{0x00,0x00,0x00,0x00}
};

Color pgm2[] PROGMEM = {
{0xff,0x00,0x00,0x00},
{0xff,0xff,0x00,0x00},
{0xff,0xff,0xff,0x00},
{0xa0,0x00,0xa0,0x00},
{0x00,0x00,0xff,0x00},
{0x00,0x00,0x00,0x00}
};

Color pgm3[] PROGMEM = {
{0xff,0xff,0xff,0x00},
{0x00,0x00,0x00,0x01}
};

Color* programs[] PROGMEM = {
(Color*)pgm1,
(Color*)pgm2,
(Color*)pgm3
};



/*-----------------------------------------------------------------------------
 * Variables
 *---------------------------------------------------------------------------*/
uint32_t timeStamp;	//TimeStamp to keep track of time
uint32_t dimStamp;	//Timestamp to keep track of dimmer changes

Color currColor;  	//Current color
Color destColor;  	//Destination color

uint16_t fadeSpeed;	//Fadingspeed. 16bit for reaaaally slow fading. Sets the speed of colorchange.
uint8_t dimSpeed;		//Dimmer speed. Sets the speed of intensitychange.

uint8_t currProgram;	//This variable keeps track of what program is currently running. 
uint8_t programMode;	//This keeps track of what mode we are currently using. 0: the program obeyes the current brightness, 1: the program is allowed to set it's own brightness

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
	dimSpeed = 10;
	timeStamp = 0;
	dimStamp = 0;
	Can_Message_t rxMsg;	

	reformatRGB(0x00,0xff,0x00,1);
	destColor.Intens = 250;
	currColor = destColor;

//Just temporary for now, they keep track of some kind of demo-program.
	currProgram = 1;
	programMode = 0;
	uint16_t temp_adress = pgm_read_word(&programs[currProgram-1]); //Get adress for program 0

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
		if (currProgram != 0){ //Check if a program is currently running
			if ((currColor.R == destColor.R)&(currColor.G == destColor.G)&(currColor.B == destColor.B)&(currColor.Intens == destColor.Intens)){//Is the current sequence finished?
				//Now we have to check if the program is finished and if we should loop
				if ((pgm_read_byte(temp_adress)==0)&(pgm_read_byte(temp_adress+1)==0)&(pgm_read_byte(temp_adress+2)==0)){
					//The program has ended! Shall we loop it?
					if (pgm_read_byte(temp_adress+3)==0){
						//Yes, let's loop it
						temp_adress = pgm_read_word(&programs[currProgram-1]);
					} else {
						//No, halt!
						currProgram = 0;
					}
				} else {
					//The program has not ended, let's get the next RGB-triplet!
					destColor.R = pgm_read_byte(temp_adress);
					destColor.G = pgm_read_byte(temp_adress + 1);
					destColor.B = pgm_read_byte(temp_adress + 2);
					if (programMode == 1){
						destColor.Intens = pgm_read_byte(temp_adress + 3);
					}

					temp_adress = temp_adress + 4;				
				}
			}
		}

		/* check if any messages have been received */
		/* A lot of remote control crap going on here now for testing */
		while (Can_Receive(&rxMsg) == CAN_OK) {
			if ((rxMsg.Data.bytes[0] == 0xf)&(rxMsg.Data.bytes[3] == 0x01)){
				currProgram = 1;
				temp_adress = pgm_read_word(&programs[currProgram-1]);
			}

			if ((rxMsg.Data.bytes[0] == 0xf)&(rxMsg.Data.bytes[3] == 0x02)){
				currProgram = 2;
				temp_adress = pgm_read_word(&programs[currProgram-1]);
			}

			if ((rxMsg.Data.bytes[0] == 0xf)&(rxMsg.Data.bytes[3] == 0x03)){
				currProgram = 3;
				temp_adress = pgm_read_word(&programs[currProgram-1]);
			}

			if ((rxMsg.Data.bytes[0] == 0xf)&(rxMsg.Data.bytes[3] == 0x00)){
				currProgram = 0;
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

/*		
	//Bara för debugcrap
	Can_Message_t txMsg;
	txMsg.Id = 0x55;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.DataLength = 8;
	txMsg.Data.bytes[0] = currColor.R;
	txMsg.Data.bytes[1] = currColor.G;
	txMsg.Data.bytes[2] = currColor.B;
	txMsg.Data.bytes[3] = currColor.Intens;

	Can_Send(&txMsg);
*/
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
 *     fade takes two pointers, current and destination and fades current				*
 *     one tick closer to destination.	                                                      *
 ************************************************************************************************/
void fade(uint8_t* current, uint8_t* destination){
	if (*current > *destination){
		*current = *current - 1;
	} else if (*current < *destination){
		*current = *current + 1;
	}
}


/************************************************************************************************
 *     changeColor changes a color accord to amount. -127>127.                                  *
 *     color is either dest och curr R,G,B.                                                     *
 *     Maybe TODO: If red is already 255 and we want to increase it, decrease the other ones.	*
 ************************************************************************************************/
void changeColor(int8_t amount, uint8_t* color){
	if ( ((amount > 0)&(*color < 255)) || ((amount < 0)&(*color > 0)) ) //Se till att vi inte slår över *color
	*color = *color + amount;
}

