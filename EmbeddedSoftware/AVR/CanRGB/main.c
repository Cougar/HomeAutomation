/**
 * CAN RGB LED. This application is used to control a RGB LED.
 * It uses the HSL colorspace. Still under heavy development.
 * Still untested with a real RGB-LED, HSV Colorspace might be better suited. We'll see...

 * TODO: Use pointers in the HSL_2_RGB functions etc. It's currently a waste of memory.
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

uint8_t R;		//Current Red PWM dutycycle
uint8_t G;		//Current Green PWM dutycycle
uint8_t B;		//Current Blue PWM dutycycle

double currH;	//HSL går mellan 0 och 1
double currS;
double currL;

double destH;
double destS;
double destL;

uint16_t fadeSpeed;	//Fadingspeed. 16bit for reaaaally slow fading
uint8_t program;		//This variable keeps track of what program is currently running. 0 means no program.
uint8_t programstate;	//This variable keeps track of where in a program we currently are.

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
void updateLED();
uint8_t Hue_2_RGB(double v1, double v2, double v3);


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
   	fadeSpeed = 20;
	timeStamp = 0;

	Can_Message_t rxMsg;
	
	//Init HSL
	currH = 0;
	currS = 1;
	currL = 0.5;

	sei();	

	/* main loop */
	while (1) {
		/* service the CAN routines */
		Can_Service();

		/* Time to see if we want to update our values */
		if (Timebase_PassedTimeMillis(timeStamp) >= fadeSpeed) {
			timeStamp = Timebase_CurrentTime();
			currH = currH + 0.01;
			if (currH > 1) currH = 0;
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


if ( currS == 0 )                       //HSL values = 0 ÷ 1
{
   R = currL * 255;                      //RGB results = 0 ÷ 255
   G = currL * 255;
   B = currL * 255;
}
else
{
   double var_1;
   double var_2;
   if ( currL < 0.5 ) var_2 = currL * ( 1 + currS );
   else           var_2 = ( currL + currS ) - ( currS * currL );

   var_1 = 2 * currL - var_2;

   R = Hue_2_RGB( var_1, var_2, currH + ( 1 / 3 ) );
   G = Hue_2_RGB( var_1, var_2, currH );
   B = Hue_2_RGB( var_1, var_2, currH - ( 1 / 3 ) );
} 



OCR1A = R;

//Red
//Green
//Blue
}


uint8_t Hue_2_RGB(double v1, double v2, double vH )             //Function Hue_2_RGB
{
   if ( vH < 0 ) vH += 1;
   if ( vH > 1 ) vH -= 1;
   if ( ( 6 * vH ) < 1 ) return 255*( v1 + ( v2 - v1 ) * 6 * vH );
   if ( ( 2 * vH ) < 1 ) return 255*( v2 );
   if ( ( 3 * vH ) < 2 ) return 255*( v1 + ( v2 - v1 ) * ( ( 2 / 3 ) - vH ) * 6 );
   return 255*( v1 );
}