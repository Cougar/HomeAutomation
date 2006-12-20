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
#include <avr\pgmspace.h> //F�r pgm_Read_byte
#include <avr/wdt.h>
#include <stdio.h>
#include <font.h>
/* lib files */
#include <can.h>
#include <timebase.h>
#include <ks0108.h>


//En variabel f�r att h�lla tillst�ndet

//Interrupttest


uint8_t encoderPosition; //Bara f�r interrupttest;

SIGNAL (SIG_INTERRUPT1)
{
	if ((PINB&(0b10000000))==0){
		encoderPosition = encoderPosition - 1;
	} else {
		encoderPosition = encoderPosition + 1;
	}
	

	if (encoderPosition > 250) encoderPosition = 0;
	if (encoderPosition > 99) encoderPosition = 99;
}

// /Interrupttest


/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
//INterrupttest
GICR|=0x80;
MCUCR=0b00001000;
GIFR=0x80;


// /Interrupttest

	Timebase_Init();
	sei();

	uint32_t timeStamp = 0;


	//H�r leker vi med displayen woohoo

//S�tt rubbet till utg�ngar.. kanske inte snygg l�sning, men det f�r bli s� s�l�nge.
PORTC=0x00;

DDRC |= (1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5);

PORTD=0x00;
DDRD |= (1<<0)|(1<<1)|(1<<2)|(1<<4)|(1<<5)|(1<<6)|(1<<7);
PORTB=0x00;
DDRB |= (1<<0)|(1<<1);

DDRD &= ~(1<<3);
DDRB &= ~(1<<7);

	//Initiera tjohej





//Ny och snyggare kod :)


//Sl� p�
glcdPowerOn();




//Test med lite CAN



	Can_Message_t rxMsg;
	char buffert[21];


	glcdSetXY(0,0);
	glcdPutStr("CanInit...");
	if (Can_Init() != CAN_OK) {
		glcdPutStr("FAILED!");
	}
	else {
		glcdPutStr("OK!");
	}


	//Can-loopen
	while (1) {
		/* service the CAN routines */
		Can_Service();
		
		/* check if any messages have been received */
		while (Can_Receive(&rxMsg) == CAN_OK) {
			glcdSetXY(0,1);
			glcdPutStr("MSG Recieved!");
			glcdSetXY(0,2);
			snprintf(buffert,21,"ID=%lx L=%u E=%u RT=%u", rxMsg.Id, (uint16_t)(rxMsg.DataLength), (uint16_t)(rxMsg.ExtendedFlag), (uint16_t)(rxMsg.RemoteFlag));
			glcdPutStr(buffert);

			glcdSetXY(0,3);

  			for (uint8_t i=0; i<rxMsg.DataLength; i++) {
    				snprintf(buffert,21,"%x ", rxMsg.Data.bytes[i]);
				glcdPutStr(buffert);
    			}
		}
		
		/* H�r kollar vi klickvriden */		
		glcdSetXY(116,0);
		snprintf(buffert,21,"%u ",encoderPosition);
		glcdPutStr(buffert);


	}


	while (1) {

	}
	
	return 0;
}
