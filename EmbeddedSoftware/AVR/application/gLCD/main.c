#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
#include <drivers/timer/timer.h>

#include <drivers/lcd/glcd/ks0108.h>
#include <drivers/lcd/glcd/font.h>

#include <avr/pgmspace.h>

#define APP_TYPE    0xf001
#define APP_VERSION 0x0002

/*
static unsigned char Splash_left[] PROGMEM = {
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		  15,231,  3,241,250,171,115,181, 53, 54,110,254,254,254,110,180, 
		  53, 53,113,171,251,242,240,  3, 15,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		   0,255,252,227, 31,191,127,254,254,255,191,119,119,119,191,255, 
		 254,254,255,127, 63,223,227,252,252,129, 63,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,127, 31,135,217, 
		 220,223,  7,  1, 12, 47, 47,118,118,121,125,251,251,251,251,125, 
		 121,118, 54, 15, 12,  9,  7, 15,223,223,216,167,143, 63,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255, 15,240,254,255,255,255, 
		   7,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
		   0,  0,  0,  0,  0,  0,  0,  0,  3,  7,255,255,255,255,240, 15, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,127,131,252,255,255,255,255,159, 
		 224,248,240,240,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
		   0,  0,  0,  0,  0,  0,240,240,248,224, 31,255,255,255,255,252, 
		 131,127,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,  0,255,255,255, 31,225,254,255, 
		 255,127, 31,  7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
		   0,  0,  0,  0,  0,  0,  7, 31,127,255,255,254,225, 31,255,255, 
		 255,  0,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,  7,248,255,255, 15,240,255,255, 15, 
		  19, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 
		  28, 28, 28, 28, 28, 28, 28, 28, 28, 19, 15,255,255,240, 15,255, 
		 255,248,  7,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,191, 95, 95,111,119,121,126,159,127,135,248,255,255,255,128, 
		   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,248,248,  0,  0, 
		   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,255,255,255,248,135, 
		 127,159,126,121,119,111, 95, 95,191,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255, 
		 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255 
};
*/
static unsigned char Splash_left[] PROGMEM = {
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf,0xe7,0x3,0xf1,0xfa,0xab,0x73,0xb5,0x35,0x36,0x6e,0xfe,0xfe,0xfe,0x6e,0xb4,0x35,0x35,0x71,0xab,0xfb,0xf2,0xf0,0x3,0xf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0x7,0xff,//Line 1 
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,0xfc,0xe3,0x1f,0xbf,0x7f,0xfe,0xfe,0xff,0xbf,0x77,0x77,0x77,0xbf,0xff,0xfe,0xfe,0xff,0x7f,0x3f,0xdf,0xe3,0xfc,0xfc,0x81,0x3f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,//Line 2 
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0x1f,0x87,0xd9,0xdc,0xdf,0x7,0x1,0xc,0x2f,0x2f,0x76,0x76,0x79,0x7d,0xfb,0xfb,0xfb,0xfb,0x7d,0x79,0x76,0x36,0xf,0xc,0x9,0x7,0xf,0xdf,0xdf,0xd8,0xa7,0x8f,0x3f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,//Line 3 
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf,0xf0,0xfe,0xff,0xff,0xff,0x7,0x3,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x7,0xff,0xff,0xff,0xff,0xf0,0xf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,//Line 4 
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0x83,0xfc,0xff,0xff,0xff,0xff,0x9f,0xe0,0xf8,0xf0,0xf0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xf0,0xf0,0xf8,0xe0,0x1f,0xff,0xff,0xff,0xff,0xfc,0x83,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,//Line 5 
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,0xff,0xff,0x1f,0xe1,0xfe,0xff,0xff,0x7f,0x1f,0x7,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x7,0x1f,0x7f,0xff,0xff,0xfe,0xe1,0x1f,0xff,0xff,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0x3,0xb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xb,0x3,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfc,0xff,//Line 6 
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7,0xf8,0xff,0xff,0xf,0xf0,0xff,0xff,0xf,0x13,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x13,0xf,0xff,0xff,0xf0,0xf,0xff,0xff,0xf8,0x7,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,//Line 7 
0xff,0xbf,0x5f,0x5f,0x6f,0x77,0x79,0x7e,0x9f,0x7f,0x87,0xf8,0xff,0xff,0xff,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xf8,0xf8,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x80,0xff,0xff,0xff,0xf8,0x87,0x7f,0x9f,0x7e,0x79,0x77,0x6f,0x5f,0x5f,0xbf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0x0,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x0,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff//Line 7 
};
uint16_t ixa = 0;
uint8_t jxa = 0;
uint16_t sensor_Temperature = 12.7*(2<<(SNS_DECPOINT_VOLTAGE-1));


void numtoascii( int16_t num, char **str );
void signedtoascii(int16_t num, uint8_t decimalplace, char *string, uint8_t numberofdecimals);
void unsignedtoascii(uint16_t num, uint8_t decimalplace, char *string, uint8_t numberofdecimals);




// A simple message "queue", with space for one message only.
// These are declared volatile to tell the compiler not to optimize away accesses.
volatile Can_Message_t rxMsg; // Message storage
volatile uint8_t rxMsgFull;   // Synchronization flag

// CAN message reception callback.
// This function runs with interrupts disabled, keep it as short as possible.
void can_receive(Can_Message_t *msg) {
	if (!rxMsgFull) {
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}

// Timer callback function used for some timer tests
void timer_callback(uint8_t timer) {
	Can_Message_t msg;
	
	msg.ExtendedFlag = 1;
	msg.Id = (CAN_TST << CAN_SHIFT_CLASS) | NODE_ID;
	msg.RemoteFlag = 0;
	msg.DataLength = 1;
	msg.Data.bytes[0] = timer;
	
	BIOS_CanSend(&msg);
}

int main(void)
{
    
	
	// Enable interrupts as early as possible
	sei();
	
	unsigned long time;
	
	Can_Message_t txMsg;
	txMsg.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) | (NODE_ID << CAN_SHIFT_NMT_SID);
	txMsg.DataLength = 4;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.Data.words[0] = APP_TYPE;
	txMsg.Data.words[1] = APP_VERSION;
	
	// Set up callback for CAN reception, this is optional if only sending is required.
	BIOS_CanCallback = &can_receive;
	// Send CAN_NMT_APP_START
	BIOS_CanSend(&txMsg);
	
	
	
	
	//Initiera utg�ngar
    DDRB |= (1<<7)|(1<<2)|(1<<1)|(1<<0);
    DDRC |= (1<<5)|(1<<4);   
    DDRD |= (1<<7)|(1<<6)|(1<<5)|(1<<4)|(1<<2)|(1<<1)|(1<<0);
	glcdPowerOn();

	glcdSetXY(0,0);
	glcdSetXY(0,0);
	glcdSetXY(0,0);

	//glcdPutStr("This is a test....... Let us try another row.. And another.. And some more..",1);

	for (jxa = 0; jxa< 8; jxa++){
		glcdSetXY(0,jxa);
		for (ixa = 0; ixa < 128; ixa++){
			glcdWriteData((uint8_t)pgm_read_byte((uint16_t)&Splash_left+ixa+jxa*128),1);
		}
	}
	
	

	//glcdSetXY(0,0);
	//glcdPutStr("I can write text. This is cool. Yeah baby! Pixelina!");
	
	/*glcdSetXY(5,5);
	glcdPutStr("I can write text.",1);
	glcdSetXY(8,10);
	glcdPutStr("Everywhere!",1);
	*/
	

	
	while (1) {

		
		if (rxMsgFull) {
			uint16_t sns_type;
			uint8_t snsid;
			if ( ((rxMsg.Id & CAN_MASK_CLASS)>>CAN_SHIFT_CLASS) == CAN_SNS){// FIXME ett j�kla herk, st�da upp
			
				sns_type = (uint16_t)((rxMsg.Id & CAN_MASK_SNS_TYPE) >> CAN_SHIFT_SNS_TYPE);
				snsid = (uint8_t)((rxMsg.Id & CAN_MASK_SNS_ID) >> CAN_SHIFT_SNS_ID);
				
				switch(sns_type)
				{
				case SNS_TYPE_TEMPERATURE:
					sensor_Temperature = rxMsg.Data.words[0];
					for (jxa = 0; jxa< 8; jxa++){
						glcdSetXY(0,jxa);
						for (ixa = 0; ixa < 128; ixa++){
							
glcdWriteData((uint8_t)pgm_read_byte((uint16_t)&Splash_left+ixa+jxa*128),1);
						}
					}
					


					char buffer[20];	

					unsignedtoascii(sensor_Temperature,SNS_DECPOINT_TEMPERATURE,buffer,1);
					
					if (rxMsg.Id == 0x040a0413){
						glcdSetXY(78,1);
						glcdPutStr("UT:",1);
						glcdPutStr(buffer,1);
						//glcdPutStr("C",1);
					} else if (rxMsg.Id == 0x040a0213) {
						glcdSetXY(78,2);
						glcdPutStr("IN:",1);
						glcdPutStr(buffer,1);
						//glcdPutStr("C",1);
					} else if (rxMsg.Id == 0x040a0211) {
						glcdSetXY(78,3);
						glcdPutStr("TV:",1);
						glcdPutStr(buffer,1);
						//glcdPutStr("C",1);
					}

				break;
				}
			}
			rxMsgFull = 0;
		}
	}
	
	return 0;
}

	//� la pengi. Skall libbifieras.
	void numtoascii( int16_t num, char **str ) {
		if( num==0 ) return;
		numtoascii( num/10, str );
		**str = '0' + (num%10);
		(*str)++;
	}

	void signedtoascii(int16_t num, uint8_t decimalplace, char *string, uint8_t numberofdecimals){ //decimalplace is number of decimals
		uint8_t i;

		if( num<0 ) {
			*(string++) = '-';
			num = -num;
		}
		numtoascii(num>>decimalplace, &string );
		if(numberofdecimals!=0) *(string++) = '.';
		for(i=0;i<numberofdecimals;i++) { 
			num %= 1<<decimalplace;
			num *= 10;
			*(string++) = '0' + (num>>decimalplace);
		}
		*(string++) = 0;
	}

	void unsignedtoascii(uint16_t num, uint8_t decimalplace, char *string, uint8_t numberofdecimals){ //decimalplace is number of decimals
		uint8_t i;
		numtoascii(num>>decimalplace, &string );
		if(numberofdecimals!=0) *(string++) = '.';
		for(i=0;i<numberofdecimals;i++) { 
			num %= 1<<decimalplace;
			num *= 10;
			*(string++) = '0' + (num>>decimalplace);
		}
		*(string++) = 0;
	}