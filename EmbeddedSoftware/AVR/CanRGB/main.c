/**
 * CAN Test. This program sends a CAN message once every second. The ID of the
 * message is increased each time for testing purposes.
 * 
 * @date	2006-11-21
 * @author	Jimmy Myhrman
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

uint8_t currRed;		//Current Red PWM dutycycle
uint8_t currGreen;	//Current Green PWM dutycycle
uint8_t currBlue;		//Current Blue PWM dutycycle

uint8_t destRed;		//Destination Red PWM dutycycle
uint8_t destGreen;	//Destination Green PWM dutycycle
uint8_t destBlue;		//Destination Blue PWM dutycycle

uint16_t fadeSpeed;	//Fadingspeed. 16bit for reaaaally slow fading
uint8_t program;		//This variable keeps track of what program is currently running. 0 means no program.
uint8_t programstate;	//This variable keeps track of where in a program we currently are.

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
void updateLED();


/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
	Timebase_Init();
	Serial_Init();

//Setting up OC1A, 16 bit counter used as 8 bit.
	/* Use OC1A */
	TCCR1A |= (1<<COM1A1) | (1<<WGM11); /* Set OC1A at TOP, mode 14 */
	TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS11); /* Timer uses main system clock with 1/8 prescale */
	ICR1 = 256; //8 bit precision to match the other two counters.
	OCR1A = 128;  //50% as standard
	DDRB |= (1<<PB1); /* Enable pin as output */

//Initialize variables
	destRed = 128;
   	fadeSpeed = 10;
	uint32_t timeStamp = 0;

	Can_Message_t rxMsg;

	sei();	

	/* main loop */
	while (1) {
		/* service the CAN routines */
		Can_Service();
		
		/* Time to see if we want to update our values */
		if (Timebase_PassedTimeMillis(timeStamp) >= fadeSpeed) {
			timeStamp = Timebase_CurrentTime();
			updateLED();
		}


		/* This code really needs cleaning up. Support for fade-programs */
		/* Maybe create a struct for one program-step? TargetRED, TargetGREEN, TargetBLUE, Speed, NextAction? */
		if (program != 0){
			switch(program){
				case 1:
					if (destRed != 255){
						destRed = destRed + 1;
					}
					break;
				case 2:
					if (destRed != 0){
						destRed = destRed - 1;
					}
					break;
				case 3:
					switch(programstate){
						case 0:
							destRed = 0;
							fadeSpeed = 10;
							if (currRed == 0){
								programstate = 1;
							}
						break;
						case 1:
							destRed = 255;
							fadeSpeed = 100;
							if (currRed == 255){
								programstate = 2;
							}
						break;
						case 2:
							destRed = 60;
							fadeSpeed = 5;
							if (currRed == 60){
								programstate = 3;
							}
						break;
						case 3:
							destRed = 170;
							fadeSpeed = 20;
							if (currRed == 170){
								programstate = 4;
							}
						break;
						case 4:
							destRed = 0;
							fadeSpeed = 40;
							if (currRed == 40){
								programstate = 0;
								program = 0;
							}
						break;
					}
					break;
			}
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

			if ((rxMsg.Data.bytes[0] == 0xf)&(rxMsg.Data.bytes[3] == 0x20)){
				program = 0;

			}

			if ((rxMsg.Data.bytes[0] == 0xf)&(rxMsg.Data.bytes[3] == 0x21)){
				program = 0;
			}


			//Ready-made programs!
			if ((rxMsg.Data.bytes[0] == 0xf)&(rxMsg.Data.bytes[3] == 0x01)){
				destRed = 255;
			}

			if ((rxMsg.Data.bytes[0] == 0xf)&(rxMsg.Data.bytes[3] == 0x02)){
				destRed = 0;
			}

			if ((rxMsg.Data.bytes[0] == 0xf)&(rxMsg.Data.bytes[3] == 0x04)){
				destRed = 0;
				fadeSpeed = 0;
			}

			if ((rxMsg.Data.bytes[0] == 0xf)&(rxMsg.Data.bytes[3] == 0x05)){
				destRed = 255;
				fadeSpeed = 10;
			}

			if ((rxMsg.Data.bytes[0] == 0xf)&(rxMsg.Data.bytes[3] == 0x07)){
				program = 3;
				programstate = 0;
			}

		}


	}
	
	return 0;
}





void updateLED(){
//Red
	if (currRed < destRed){
		currRed = currRed + 1;
		OCR1A = currRed;
	} else if (currRed > destRed){
		currRed = currRed - 1;
		OCR1A = currRed;
	}
//Green
//Blue
}

