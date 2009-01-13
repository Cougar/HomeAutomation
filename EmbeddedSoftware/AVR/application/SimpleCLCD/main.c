/**
 * CanSimpleCLCD. Simple version.
 * For HD44780- and KS0073-based LCD displays
 *
 * Contains no intelligence. It just prints what it is told to.
 *
 * @date    2006-12-11
 * @author  Erik Larsson
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

#include <string.h> //for memcpy


/* funcdefs */
//#include <funcdefs/funcdefs.h>


//a lot of crap from previous funcdefs
/* ACT LCD 12 bits <ACTION><SIZE> 0xFFFL
 *
 * <ACTION>, 4 bits = Type och action.
 * <SIZE>, 8 bits = Size of display.
 */
#define LCD_ACTION_MASK 0xF00L /* bit[12..8] */
#define LCD_SIZE_MASK 0x0FFL /* bit[7..0] */

#define LCD_ACTION_BITS 8
#define LCD_SIZE_BITS 0


/* <ACTION> 4 bits 0xFL */

#define LCD_ACTION_CLR			0x0L
#define LCD_ACTION_CURS			0x1L
#define LCD_ACTION_TEXT			0x2L
#define LCD_ACTION_GET_SIZE		0x3L
#define LCD_ACTION_SET_CONT		0x4L
#define LCD_ACTION_SET_BLIGHT	0x5L
#define LCD_ACTION_GET_CONT		0x6L
#define LCD_ACTION_GET_BLIGHT	0x7L

#define LCD_ACTION_SEND_CONT	0x8L
#define LCD_ACTION_SEND_BLIGHT	0x9L



/* <SIZE> 8 bits 0xFFL */
#define LCD_SIZE_ALL	0x00L
#define LCD_SIZE_16x2	0x02L
#define LCD_SIZE_20x4	0x10L



/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define SIZE_X	20
#define SIZE_Y	2
#define SIZE_LCD LCD_SIZE_20x2

#define BUFFER_SIZE SIZE_X
#define TRUE 1
#define FALSE 0

#define LEFT	0x01
#define RIGHT	0x02

volatile Can_Message_t rxMsg;

char incoming_text[ BUFFER_SIZE ];
uint8_t rxMsgFull;


char buffer[ BUFFER_SIZE ];

void send_dimensions(void);
void rotenc(void);

// CAN message reception callback
// This function runs with interrupts disabled, keep it as short as possible
void can_receive(Can_Message_t *msg){
	if(!rxMsgFull){
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}

ISR( PCINT2_vect ){
// For ROTENC_A and ROTENC_B FIXME �r inte riktigt r�tt. Verkar vara bra tycker jag?
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
	uint8_t m, lcd_action;

	sei();

#if 0
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
#endif
	
	// Contrast
	TCCR0A |= (1<<COM0B1)|(1<<WGM01)|(1<<WGM00);
	TCCR0B |= (1<<CS00);
	OCR0B = 0xf0;
	DDRD |= (1<<DDD5);

#if 0
	// Backlight
	TCCR1A |= (1<<COM1A1)|(1<<WGM11);
	TCCR1B |= (1<<WGM13)|(1<<WGM12)|(1<<CS10);
	ICR1 = 0xFF; // Make it 8 bits
	OCR1A = 0xFF;
	DDRB |= (1<<DDB1);
#endif

	Can_Message_t txMsg;
	txMsg.ExtendedFlag=1;
	txMsg.RemoteFlag=0;
// FIXME skicka app startad
	BIOS_CanCallback = &can_receive;

	lcd_init( LCD_DISP_ON );
	lcd_clrscr();
	lcd_puts("HomeAutomation\n");

	send_dimensions();
	lcd_puts("CAN Connected!\n");

	uint8_t cnt=0;
    /* main loop */
	while(1){
        /* check if any messages have been received */
		if(rxMsgFull){
			if (cnt == 0) {
				lcd_home();
				lcd_puts(".                   ");
			} else if (cnt == 1) {
				lcd_home();
				lcd_puts(" .                  ");
			} else if (cnt == 2) {
				lcd_home();
				lcd_puts("  .                 ");
			} else if (cnt == 3) {
				lcd_home();
				lcd_puts("   .                ");
			} else if (cnt == 4) {
				lcd_home();
				lcd_puts("    .               ");
			} else if (cnt == 5) {
				lcd_home();
				lcd_puts("     .              ");
			} else if (cnt == 6) {
				lcd_home();
				lcd_puts("      .             ");
			} else if (cnt == 7) {
				lcd_home();
				lcd_puts("       .            ");
			} else if (cnt == 8) {
				lcd_home();
				lcd_puts("        .           ");
			} else if (cnt == 9) {
				lcd_home();
				lcd_puts("         .          ");
			} else if (cnt == 10) {
				lcd_home();
				lcd_puts("          .         ");
			} else if (cnt == 11) {
				lcd_home();
				lcd_puts("           .        ");
			} else if (cnt == 12) {
				lcd_home();
				lcd_puts("            .       ");
			} else if (cnt == 13) {
				lcd_home();
				lcd_puts("             .      ");
			} else if (cnt == 14) {
				lcd_home();
				lcd_puts("              .     ");
			} else if (cnt == 15) {
				lcd_home();
				lcd_puts("               .    ");
			} else if (cnt == 16) {
				lcd_home();
				lcd_puts("                .   ");
			} else if (cnt == 17) {
				lcd_home();
				lcd_puts("                 .  ");
			} else if (cnt == 18) {
				lcd_home();
				lcd_puts("                  . ");
			} else if (cnt == 19) {
				lcd_home();
				lcd_puts("                   .");
			}
			cnt += 1;
			if (cnt == 20) {
				cnt = 0;
			}
			
			uint16_t act_type;
			uint8_t lcdid;
			if ( ((rxMsg.Id & CAN_MASK_CLASS)>>CAN_SHIFT_CLASS) == CAN_ACT){// FIXME ett j�kla herk, st�da upp
				
				
				act_type =(uint16_t)((rxMsg.Id & CAN_MASK_ACT_TYPE) >> CAN_SHIFT_ACT_TYPE);
			
				lcdid = (uint8_t)((rxMsg.Id & CAN_MASK_ACT_ID) >> CAN_SHIFT_ACT_ID);
				

				if( act_type == ACT_TYPE_LCD ){ //FIXME add id to check here
					lcd_action =  rxMsg.Data.bytes[0];
					switch( lcd_action ){
					case LCD_ACTION_CLR:
						// Clear LCD
						lcd_clrscr();
					break;
					case LCD_ACTION_CURS:
						// Move cursor
						lcd_gotoxy( rxMsg.Data.bytes[1],rxMsg.Data.bytes[2] );
					break;
					case LCD_ACTION_GET_SIZE:
						// Request LCD dimension
						send_dimensions();
					break;
					case LCD_ACTION_TEXT:
						// Print text to LCD
						for(m=1;m<rxMsg.DataLength;m++){
							lcd_putc( (char)rxMsg.Data.bytes[m] );
						}
					break;
					case LCD_ACTION_SET_CONT:
						// Set contrast
						if(rxMsg.DataLength == 1){
							OCR0B = rxMsg.Data.bytes[1];
						}
					break;
					case LCD_ACTION_SET_BLIGHT:
						// Set backlight
						if(rxMsg.DataLength == 2){
							OCR1AL = rxMsg.Data.bytes[1];
						}
					break;
					case LCD_ACTION_GET_CONT:
						// Get contrast
						txMsg.DataLength = 1;
						//txMsg.Id = ( CLASS_ACT<<CLASS_MASK_BITS )|( ACT_TYPE_LCD<<ACT_TYPE_BITS )|( LCD_ACTION_SEND_CONT<<LCD_ACTION_BITS )|NODE_ID; FIXME
						txMsg.Data.bytes[1] = OCR0B;
						BIOS_CanSend(&txMsg);
					break;
					case LCD_ACTION_GET_BLIGHT:
						// Get backlight
						txMsg.DataLength = 1;
						//txMsg.Id = ( CLASS_ACT<<CLASS_MASK_BITS )|( ACT_TYPE_LCD<<ACT_TYPE_BITS )|( LCD_ACTION_SEND_BLIGHT<<LCD_ACTION_BITS )|NODE_ID; FIXME
						txMsg.Data.bytes[1] = OCR1AL;
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
			}else{
				// Moving left
				txMsg.Data.bytes[0]=LEFT;
				txMsg.Data.bytes[1] = (PINB&(1<<PB7))?0:1;
				txMsg.DataLength=2;
				BIOS_CanSend(&txMsg);
			}
		}
		rot_laststate = rot_data;
	} else { // No, only one of the signals are high. We can use this to find out what direction we are moving.
		rot_lastdir = rot_data;
	}
}

