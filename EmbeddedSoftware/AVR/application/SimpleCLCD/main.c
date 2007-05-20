/**
 * CanSimpleCLCD. Simple version.
 * For HD44780- and KS0073-based LCD displays
 *
 * Contains no intelligence. It just prints what it is told to.
 *
 * @date    2006-12-11
 * @author  Erik Larsson
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
#include <lcd_HD44780.h>
/* funcdefs */
#include <funcdefs/funcdefs.h>

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

volatile Can_Message_t rxMsg;

char incoming_text[ BUFFER_SIZE ];
uint8_t rxMsgFull;


char buffer[ BUFFER_SIZE ];

void send_dimensions();
void rotenc();

// CAN message reception callback
// This function runs with interrupts disabled, keep it as short as possible
void can_receive(Can_Message_t *msg){
	if(!rxMsgFull){
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}

ISR( PCINT2_vect ){
// For ROTENC_A and ROTENC_B FIXME är inte riktigt rätt
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
	uint32_t act;
	uint8_t m, act_type, lcd_action, lcd_size;

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
	OCR1A = 0x80;
	DDRB |= (1<<DDB1);

	Can_Message_t txMsg;
	txMsg.ExtendedFlag=1;
	txMsg.RemoteFlag=0;
// FIXME skicka app startad
	BIOS_CanCallback = &can_receive;

	lcd_init( LCD_DISP_ON );
	lcd_clrscr();
	lcd_puts("HomeAutomation\n");

	send_dimensions();
	lcd_puts("Very nice!\n");

    /* main loop */
	while(1){
        /* check if any messages have been received */
		if(rxMsgFull){

			if( ((rxMsg.Id & CLASS_MASK)>> CLASS_MASK_BITS) == CLASS_ACT ){ // FIXME ett jäkla herk, städa upp
				act = (rxMsg.Id & TYPE_MASK) >> TYPE_MASK_BITS;
				act_type = (act & ACT_TYPE_MASK) >> ACT_TYPE_BITS;
				lcd_action = (act & LCD_ACTION_MASK) >> LCD_ACTION_BITS;
				lcd_size = (act & LCD_SIZE_MASK) >> LCD_SIZE_BITS;

				if( act_type == ACT_TYPE_LCD && (lcd_size == SIZE_LCD || lcd_size == LCD_SIZE_ALL)){
					switch( lcd_action ){
					case LCD_ACTION_CLR:
						// Clear LCD
						lcd_clrscr();
					break;
					case LCD_ACTION_CURS:
						// Move cursor
						lcd_gotoxy( rxMsg.Data.bytes[0],rxMsg.Data.bytes[1] );
					break;
					case LCD_ACTION_GET_SIZE:
						// Request LCD dimension
						send_dimensions();
					break;
					case LCD_ACTION_TEXT:
						// Print text to LCD
						for(m=0;m<rxMsg.DataLength;m++){
							lcd_putc( (char)rxMsg.Data.bytes[m] );
						}
					break;
					case LCD_ACTION_SET_CONT:
						// Set contrast
						if(rxMsg.DataLength == 1){
							OCR0B = rxMsg.Data.bytes[0];
						}
					break;
					case LCD_ACTION_SET_BLIGHT:
						// Set backlight
						if(rxMsg.DataLength == 1){
							OCR1AL = rxMsg.Data.bytes[0];
						}
					break;
					case LCD_ACTION_GET_CONT:
						// Get contrast
						txMsg.DataLength = 1;
						txMsg.Id = ( CLASS_ACT<<CLASS_MASK_BITS )|( ACT_TYPE_LCD<<ACT_TYPE_BITS )|( LCD_ACTION_SEND_CONT<<LCD_ACTION_BITS )|NODE_ID;
						txMsg.Data.bytes[0] = OCR0B;
						BIOS_CanSend(&txMsg);
					break;
					case LCD_ACTION_GET_BLIGHT:
						// Get backlight
						txMsg.DataLength = 1;
						txMsg.Id = ( CLASS_ACT<<CLASS_MASK_BITS )|( ACT_TYPE_LCD<<ACT_TYPE_BITS )|( LCD_ACTION_SEND_BLIGHT<<LCD_ACTION_BITS )|NODE_ID;
						txMsg.Data.bytes[0] = OCR1AL;
						BIOS_CanSend(&txMsg);
					break;
					default:
					// Take over the world
					break;
					}
				}
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

void rotenc(){
	uint8_t rot_data = 0;
	static uint8_t rot_lastdir = 0, rot_laststate = 0; // FIXME måste flyttas ut igen?

	Can_Message_t txMsg;
	txMsg.ExtendedFlag=1;
	txMsg.RemoteFlag=0;
	txMsg.Id = ( CLASS_SNS<<CLASS_MASK_BITS )|( SNS_ACT_BUTTON<<SNS_TYPE_BITS )|( 0x01<<SNS_ID_BITS )| NODE_ID;
	txMsg.DataLength=2;

	if(PINB&(1<<PB0)){
		rot_data |= 0x01;
	}
	if(PIND&(1<<PD2)){
		rot_data |= 0x02;
	}

	if( rot_data==0 || rot_data==3 ){
		if( rot_data==0 && rot_laststate!=rot_data ){
			if( rot_lastdir&0x01 ){
				// Moving right
				txMsg.Data.bytes[0]=RIGHT;
				txMsg.Data.bytes[1] = (PINB&(1<<PB7))?0:1;
				txMsg.DataLength=2;
				BIOS_CanSend(&txMsg);
				// For testing TODO remove
				if(OCR1A<0xFF){
					OCR1A++;
				}
				if(OCR0B>0){
					OCR0B--;
				}
			}else{
				// Moving left
				txMsg.Data.bytes[0]=LEFT;
				txMsg.Data.bytes[1] = (PINB&(1<<PB7))?0:1;
				txMsg.DataLength=2;
				BIOS_CanSend(&txMsg);
				// For testing TODO remove
				if(OCR1A>0){
					OCR1A--;
				}
				if(OCR0B<0xFF){
					OCR0B++;
				}
			}
		}
		rot_laststate = rot_data;
	}else{
		rot_lastdir = rot_data;
	}
}

