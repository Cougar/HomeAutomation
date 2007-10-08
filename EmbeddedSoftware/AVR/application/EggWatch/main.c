/**
 * CanEggWatch. Currenty only supports nodeCLCD, this will probably change since I want 7-segments in the long run.
 * Hardware: http://projekt.auml.se/homeautomation:hardware:avr:clcd
 * TODO: Sending messages to report status, cleanup, graphical bar, nicer GUI, multiple timers
 *
 *
 * @date    2007-09-09
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
#include <drivers/timer/timebase.h>


#include <string.h> //for memcpy
#include <stdlib.h> //for itoa


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
void action(uint8_t type);
void alarm(void);
void refreshDisplay(void);

uint32_t timeBase = 0;

uint32_t timeBase_turnspeed = 0;

char lcdBuffer[5];

//Struct for countdown
typedef struct
{
	uint16_t timerCnt;
	uint8_t alarmTimer;
	uint16_t timerMax;
} countDown;

countDown countTimer1;


// CAN message reception callback
// This function runs with interrupts disabled, keep it as short as possible
void can_receive(Can_Message_t *msg){
	if(!rxMsgFull){
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}

ISR( PCINT2_vect ){
// For ROTENC_A and ROTENC_B FIXME är inte riktigt rätt. Verkar vara bra tycker jag?
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
//	uint32_t act; not used anymoer
//	uint8_t act_type, lcd_size; not used anymore
//	uint8_t m, lcd_action; These are probably not needed now

	sei();
	
	Timebase_Init();

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
	lcd_puts("CanEggWatch\n");

	send_dimensions();
	lcd_puts("CAN Connected!\n");
	
	lcd_clrscr();
	

    /* main loop */
	while(1){
		
		
		if( Timebase_PassedTimeMillis(timeBase) >= 1000 ){
			if (countTimer1.timerCnt != 0){
				countTimer1.timerCnt--;
				refreshDisplay();
			} else {
				if (countTimer1.alarmTimer != 0){
					alarm();
					countTimer1.alarmTimer--;
				}
			}
			timeBase =  Timebase_CurrentTime();
		}
		
		
        /* check if any messages have been received */
		if(rxMsgFull){
			//Throw it away. We don't use them yet. TODO: Support for time and remote setting.
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
		action((btn_laststate)?BUTTON_RELEASE:BUTTON_PUSH);
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
				action(RIGHT);
			}else{
				// Moving left
				txMsg.Data.bytes[0]=LEFT;
				txMsg.Data.bytes[1] = (PINB&(1<<PB7))?0:1;
				txMsg.DataLength=2;
				BIOS_CanSend(&txMsg);
				action(LEFT);
			}
		}
		rot_laststate = rot_data;
	} else { // No, only one of the signals are high. We can use this to find out what direction we are moving.
		rot_lastdir = rot_data;
	}
}

void action(uint8_t type){
	//Let's do something with something!
	lcd_clrscr();
	if (type == LEFT){
		countTimer1.timerCnt = countTimer1.timerCnt - 10;
		refreshDisplay();
	}
	if (type == RIGHT){
		countTimer1.timerCnt = countTimer1.timerCnt + 10;
		if  (countTimer1.timerCnt > countTimer1.timerMax){
			countTimer1.timerMax = countTimer1.timerCnt;
		}
		refreshDisplay();
	}
	if (type == BUTTON_PUSH){
		lcd_puts("Init!");
	}
	if (type == BUTTON_RELEASE){
		timeBase = Timebase_CurrentTime();
		countTimer1.timerCnt = 10;
		countTimer1.timerMax = 10;
		countTimer1.alarmTimer = 1;
		refreshDisplay();
	}
}

void alarm(void){
	lcd_puts("Riing!");
}

void refreshDisplay(void){
	lcd_clrscr();
	itoa(countTimer1.timerCnt/60,lcdBuffer,10);
	lcd_puts(lcdBuffer);
	lcd_puts(":");
	itoa(countTimer1.timerCnt%60,lcdBuffer,10);
	lcd_puts(lcdBuffer);
	
	lcd_gotoxy(0,1);
	lcdProgressBar(countTimer1.timerCnt,countTimer1.timerMax,16);	
}
