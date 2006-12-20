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


uint8_t encoderPosition; //Bara f�r interrupttest. +/-1 beroende p� riktning.;

//variabler f�r visning
	Can_Message_t rxMsg;

void draw_lcd_screen(void){

	switch (encoderPosition) {
		case 0:
			glcdSetXY(0,0);
			char buffert[21];
			glcdPutStr("Latest CAN-message:");
			glcdSetXY(0,1);
			snprintf(buffert,21,"ID=%lx L=%u E=%u RT=%u", rxMsg.Id, (uint16_t)(rxMsg.DataLength), (uint16_t)(rxMsg.ExtendedFlag), (uint16_t)(rxMsg.RemoteFlag));
			glcdPutStr(buffert);

			glcdSetXY(0,2);

  			for (uint8_t i=0; i<rxMsg.DataLength; i++) {
    				snprintf(buffert,21,"%x ", rxMsg.Data.bytes[i]);
				glcdPutStr(buffert);
    			}
		return;	

		case 1: 
			glcdSetXY(0,0);
			glcdPutStr("Muahaha, Har har vi nummer tva. Och detta kanske funkar!");
		return;
		case 2: 
			glcdSetXY(0,0);
			glcdPutStr("cparune :P");
		return;

	}

}



SIGNAL (SIG_INTERRUPT1)
{
	if ((PINB&(0b10000000))==0){
		encoderPosition = encoderPosition - 1;
	} else {
		encoderPosition = encoderPosition + 1;
	}
	

	if (encoderPosition > 250) encoderPosition = 0;
	if (encoderPosition > 99) encoderPosition = 99;
	glcdClear(); //Nolla displayen

	draw_lcd_screen();
}


//St�d f�r en massa olika sidor




/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
	GICR|=0x80;		//Interrupt p�
	MCUCR=0b00001000;	//Rising Edge
	GIFR=0x80;		//N�got interruptigt :)


	Timebase_Init();
	sei();
	uint32_t timeStamp = 0;


	//S�tt en massa ut/ing�ngar.. kanske inte snygg l�sning, men det f�r bli s� s�l�nge.
	PORTC=0x00;
	DDRC |= (1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5);
	PORTD=0x00;
	DDRD |= (1<<0)|(1<<1)|(1<<2)|(1<<4)|(1<<5)|(1<<6)|(1<<7);
	PORTB=0x00;
	DDRB |= (1<<0)|(1<<1);
	DDRD &= ~(1<<3);
	DDRB &= ~(1<<7);

	
	glcdPowerOn();

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
			if (encoderPosition == 0){
				draw_lcd_screen();
			}
		}
		
		/* H�r kollar vi klickvriden */		
//		glcdSetXY(115,0);
//		snprintf(buffert,21,"%u ",encoderPosition);
//		glcdPutStr(buffert);
	}

	return 0;
}
