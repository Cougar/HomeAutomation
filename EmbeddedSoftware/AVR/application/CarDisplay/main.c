/**
 * CanCarDisplay. HID for simple carsensor network
 * For HD44780- and KS0073-based LCD displays
 *
 * Contains no intelligence. It just prints what it is told to.
 *
 * @date    2007-10-01
 * @author	Martin Norden
 *
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
/* lib files */
#include <bios.h>
#include <config.h>
#include <drivers/lcd/clcd/lcd_HD44780.h>
#include <stdlib.h> //for itoa

#include <string.h> //for memcpy


/* <SIZE> 8 bits 0xFFL */
#define LCD_SIZE_ALL	0x00L
#define LCD_SIZE_16x2	0x02L
#define LCD_SIZE_20x4	0x10L


//Borde ha en bŠttre plats.
#define max(a,b) ((a>b)?a:b)
#define min(a,b) ((a<b)?a:b)


/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define SIZE_X	20
#define SIZE_Y	4
#define SIZE_LCD LCD_SIZE_20x4

#define BUFFER_SIZE SIZE_X
#define TRUE 1
#define FALSE 0

#define LEFT	0x01
#define RIGHT	0x02
#define BUTTON_PUSH 0x03
#define BUTTON_RELEASE 0x04

volatile Can_Message_t rxMsg;

char incoming_text[ BUFFER_SIZE ];
uint8_t rxMsgFull;


char buffer[ BUFFER_SIZE ];

void send_dimensions(void);
void rotenc(void);
void rotenc_action(uint8_t type);


void updateDisplay(uint8_t screen, uint8_t transition);

#define SCR_LAMBDA		0x00
#define SCR_BOOST		0x01
#define SCR_MAXBOOST	0x02
#define SCR_RPM			0x03
#define SCR_VOLTAGE		0x04
#define LAST_SCREEN		0x04
void standardScreen(char* text, uint16_t value, uint16_t maxvalue);

uint8_t *io_s16toStr(int16_t value, uint8_t *str,uint8_t decimal,uint8_t options);
uint8_t *io_u16toStr(uint16_t value, uint8_t *str,uint8_t decimal,uint8_t options);



/*******************************************************************************
 * Sensor Values, Scale factors etc.                                           *
 *******************************************************************************/
int16_t sensor_Boost = 120; //Should be set automagically by incoming CAN
int16_t sensor_Boost_Max = 200;

uint16_t sensor_RPM = 2573;
uint8_t sensor_Lambda = 127;
uint8_t sensor_Voltage = 143;

#define scale_Boost 809 //Read from eeprom later on?
#define scale_BoostOffset 168 //Read from eeprom later on?
#define scale_BoostMAX 200
#define scale_RPMMAX 8000 //Can be set as in eeprom later perhaps?
#define scale_Voltage 1024
#define scale_VoltageMAX 150


uint8_t currentScreen;

// CAN message reception callback
// This function runs with interrupts disabled, keep it as short as possible
void can_receive(Can_Message_t *msg){
	if(!rxMsgFull){
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}

ISR( PCINT2_vect ){
// For ROTENC_A and ROTENC_B FIXME Šr inte riktigt rŠtt. Verkar vara bra tycker jag?
	rotenc();
}

ISR( PCINT0_vect ){
// For ROTENC_BTN
	rotenc();
}

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
	currentScreen = 0; //init display position
	
//	uint32_t act; not used anymoer
//	uint8_t act_type, lcd_size; not used anymore
	//uint8_t m, lcd_action;

	sei();

	/*
	 * Initialize rotaryencoders and buttons
	 */
	// Set as input
	DDRD &= ~(1<<DDD2);
	DDRB &= ~((1<<DDB7)|(1<<DDB0));
	// Enable pull-up
	PORTD |= (1<<PD2);
	PORTB |= (1<<PB7)|(1<<PB0);
	// Enable IO-pin interrupt
	PCICR |= (1<<PCIE2)|(1<<PCIE0);
	// Unmask PB7, PB0 and PD2
	PCMSK2 |= (1<<PCINT18);
	PCMSK0 |= (1<<PCINT7)|(1<<PCINT0);

	// Contrast
	TCCR0A |= (1<<COM0B1)|(1<<WGM01)|(1<<WGM00);
	TCCR0B |= (1<<CS00);
	OCR0B = 0x10;
	DDRD |= (1<<DDD5);

	// Backlight
	TCCR1A |= (1<<COM1A1)|(1<<WGM11);
	TCCR1B |= (1<<WGM13)|(1<<WGM12)|(1<<CS10);
	ICR1 = 0xFF; // Make it 8 bits
	OCR1A = 0xFF;
	DDRB |= (1<<DDB1);


	Can_Message_t txMsg;
	txMsg.ExtendedFlag=1;
	txMsg.RemoteFlag=0;
// FIXME skicka app startad
	BIOS_CanCallback = &can_receive;

	lcd_init( LCD_DISP_ON );
	lcd_clrscr();
	lcd_puts("CarDisplay");

	send_dimensions();
	lcd_gotoxy(0,1);
	lcd_puts("CAN Connected!");

    /* main loop */
	while(1){
        /* check if any messages have been received */
		if(rxMsgFull){
			uint16_t act_type;
			uint8_t lcdid;
			if ( ((rxMsg.Id & CAN_MASK_CLASS)>>CAN_SHIFT_CLASS) == CAN_ACT){// FIXME ett jŠkla herk, stŠda upp
				
				
				act_type =(uint16_t)((rxMsg.Id & CAN_MASK_ACT_TYPE) >> CAN_SHIFT_ACT_TYPE);
			
				lcdid = (uint8_t)((rxMsg.Id & CAN_MASK_ACT_ID) >> CAN_SHIFT_ACT_ID);				


			}
			rxMsgFull = 0;
		}
	}
}

void send_dimensions(){
	Can_Message_t txMsg;

	txMsg.ExtendedFlag=1;
	txMsg.RemoteFlag=0;
	txMsg.Id=0xA000001;
	txMsg.Data.bytes[0] = SIZE_X;
	txMsg.Data.bytes[1] = SIZE_Y;
	txMsg.DataLength = 2;

	BIOS_CanSend(&txMsg);
}

void rotenc_action(uint8_t type){
	//Let's do something with something!
	lcd_clrscr();
	if (type == LEFT){	
		if (currentScreen == 0) currentScreen = LAST_SCREEN+1;
		currentScreen--;
		updateDisplay(currentScreen,1);
	}
	if (type == RIGHT){
		currentScreen++;
		if (currentScreen > LAST_SCREEN) currentScreen = 0;
		updateDisplay(currentScreen,1);
	}
	if (type == BUTTON_PUSH){

	}
	if (type == BUTTON_RELEASE){

	}
}

void updateDisplay(uint8_t screen, uint8_t transition){
	char buffer[10];	
	
	if (transition){
		lcd_clrscr();
	}
	switch (screen){
	case SCR_LAMBDA:
		lcd_gotoxy(0,0);
		lcd_puts("LEAN Lambda RICH");
		lcd_gotoxy(0,1);
		lcdProgressBar(sensor_Lambda, 255, 16); //TODO: vacker bargraph med bara en plupp i mitten
	break;
	case SCR_RPM:		
		lcd_gotoxy(0,0);
		itoa(sensor_RPM,buffer,10);
		lcd_puts("RPM: ");
		lcd_puts(buffer);
		lcd_gotoxy(0,1);
		lcdProgressBar(sensor_RPM, scale_RPMMAX, 16);
	break;
	case SCR_BOOST:
			
		lcd_gotoxy(0,0);
		lcd_puts("Boost:");
	
		lcd_gotoxy(7,0);
		lcd_puts(io_s16toStr(((int32_t)sensor_Boost*scale_Boost)>>10,buffer,2,1));
	
		lcd_gotoxy(13,0);
		lcd_puts("bar");
		
		lcd_gotoxy(0,1);
		lcdProgressBar(max(0,sensor_Boost), ((int32_t)scale_BoostMAX)>>10, 16);
	break;
	case SCR_MAXBOOST:
		lcd_gotoxy(0,0);
		lcd_puts("Max Boost: ");
		lcd_gotoxy(11,0);
		lcd_puts(io_s16toStr(((int32_t)sensor_Boost_Max*scale_Boost)>>10,buffer,2,1));		
	break;
	case SCR_VOLTAGE:

		lcd_gotoxy(0,0);
		lcd_puts("Voltage:");
		
		lcd_gotoxy(9,0);
		lcd_puts(io_u16toStr(((uint32_t)sensor_Voltage*scale_Voltage)>>10,buffer,1,0));
		
		lcd_gotoxy(16,0);
		lcd_puts("V");		
		lcd_puts("spanning!");
		
		lcd_gotoxy(0,1);
		lcdProgressBar(sensor_Voltage, scale_VoltageMAX, 16);
	break;
	}
}

void standardScreen(char* text, uint16_t value, uint16_t maxvalue){
	lcd_gotoxy(0,0);
	char buffer[5];
	itoa(value,buffer,10);
	lcd_puts(text);
	lcd_puts(" ");
	lcd_puts(buffer);
	lcd_gotoxy(0,1);
	lcdProgressBar(value, maxvalue, 16);
}
















void rotenc(){
	uint8_t rot_data = 0;
	static uint8_t rot_lastdir = 0, rot_laststate = 0, btn_laststate = 0;

	Can_Message_t txMsg;
	txMsg.ExtendedFlag=1;
	txMsg.RemoteFlag=0;
	//txMsg.Id = ( CLASS_SNS<<CLASS_MASK_BITS )|( SNS_ACT_BUTTON<<SNS_TYPE_BITS )|( 0x01<<SNS_ID_BITS )| NODE_ID;//FIXME: borttaget
	txMsg.DataLength=2;

	//Take care of button push
	if ((PINB&(1<<PB7)) != btn_laststate){ //The buttonstate has changed! Let's send a message!
		btn_laststate = (PINB&(1<<PB7));
		txMsg.Data.bytes[0] = 5;
		txMsg.Data.bytes[1] = (btn_laststate)?0:1;
		txMsg.DataLength=2;
		BIOS_CanSend(&txMsg);
		rotenc_action((btn_laststate)?BUTTON_RELEASE:BUTTON_PUSH);
	}
	
	//Take care of rotary encoder movement
	if(PINB&(1<<PB0)){
		rot_data |= 0x01;
	}
	if(PIND&(1<<PD2)){
		rot_data |= 0x02;
	}

	if( rot_data==0 || rot_data==3 ){ // Are both signals high or low?
		if( rot_data==0 && rot_laststate!=rot_data ){ // Are both signals low? In that case we are finished with one turn and should print out the direction it went. 
			if( rot_lastdir&0x01 ){
				// Moving right
				txMsg.Data.bytes[0]=RIGHT;
				txMsg.Data.bytes[1] = (PINB&(1<<PB7))?0:1;
				txMsg.DataLength=2;
				BIOS_CanSend(&txMsg);
				rotenc_action(RIGHT);
			}else{
				// Moving left
				txMsg.Data.bytes[0]=LEFT;
				txMsg.Data.bytes[1] = (PINB&(1<<PB7))?0:1;
				txMsg.DataLength=2;
				BIOS_CanSend(&txMsg);
				rotenc_action(LEFT);
			}
		}
		rot_laststate = rot_data;
	} else { // No, only one of the signals are high. We can use this to find out what direction we are moving.
		rot_lastdir = rot_data;
	}
}








//Supersnodd:



uint8_t _io_buf[10];

uint8_t *io_s16toStr(int16_t value,uint8_t *str,uint8_t decimal,uint8_t options) {
	uint8_t i=0,j=0,w=0;
	uint16_t tmp;
	
	if(value<0) {
		str[i++]='-';
		tmp=-value;
	} else {
		if(options&0x01) str[i++]=' ';
		tmp=value;
	}
	while(tmp>0) {
		_io_buf[j++]='0'+(tmp%10);
		tmp=tmp/10;
	}
	if(j<=decimal) {
		str[i++]='0';
		if(decimal>0) str[i++]='.';
		w=i;
		while((i-w)<(decimal-j)) str[i++]='0';
		while(j!=0) str[i++]=_io_buf[--j];
	} else {
		while(j!=0) {
			str[i++]=_io_buf[--j];
			if(j==decimal) str[i++]='.';
		}
	}
	str[i++]=0;
	return(str);
}

uint8_t *io_u16toStr(uint16_t value,uint8_t *str,uint8_t decimal,uint8_t options) {
	uint8_t i=0;
	uint8_t j=0;
	uint8_t w=0;
	
	while(value>0) {
		_io_buf[j++]='0'+(value%10);
		value=value/10;
	}

	if(j<=decimal) {
		str[i++]='0';
		if(decimal>0) str[i++]='.';
		w=i;
		while((i-w)<(decimal-j)) str[i++]='0';
		while(j!=0) str[i++]=_io_buf[--j];
	} else {
		while(j>0) {
			str[i++]=_io_buf[--j];
			if((decimal>0)&&(j==decimal)) str[i++]='.';
		}
	}
	str[i++]=0;
	return(str);
}
