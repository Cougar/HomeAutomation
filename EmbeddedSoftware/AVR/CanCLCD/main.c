/**
 * CanCLCD.
 * For HD44780- and KS0073-based LCD displays
 * Prints sensor data and other interesting things.
 *
 * @date    2006-12-11
 * @author  Erik Larsson
 *
 * TODO Fix more types of output: relay status, mail recieved etc.
 *
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
/* lib files */
#include <bios.h>
#include <lcd_HD44780.h>
/* funcdefs */
//#include <eqlazer_funcdefs.h>

/* defines */
#define SIZE_X	20
#define SIZE_Y	4

//#define BUFFER_SIZE 20
#define TRUE 1
#define FALSE 0

#define LEFT	0x01
#define RIGHT	0x02

Can_Message_t rxMsg;
Can_Message_t txMsg;


uint8_t		msg_received = FALSE;

uint8_t rot_lastdir = 0,
		rot_laststate = 0;


void can_receive(Can_Message_t *msg){
// FIXME
}


ISR( PCINT0_vect ){ // For ROTENC_A and ROTENC_B
	uint8_t rot_data = 0;

	txMsg.Id=0xA000000;
	txMsg.DataLength=2;

	if(PINB&(1<<PB7)){
		rot_data |= 0x01;
	}
	if(PINB&(1<<PB0)){
		rot_data |= 0x02;
	}

	if( rot_data==0 || rot_data==3 ){
		if( rot_data==0 && rot_laststate!=rot_data ){
			if( rot_lastdir&0x01 ){
				// Moving right
				txMsg.Data.bytes[0]=RIGHT;
				txMsg.Data.bytes[1] = (PIND&(1<<PD2))?0:1;
				bios->can_send(&txMsg);
			}else{
				// Moving left
				txMsg.Data.bytes[0]=LEFT;
				txMsg.Data.bytes[1] = (PIND&(1<<PD2))?0:1;
				bios->can_send(&txMsg);
			}
		}
		rot_laststate = rot_data;
	}else{
		rot_lastdir = rot_data;
	}
}

ISR( PCINT2_vect ){ // For ROTENC_BTN
	lcd_puts("a");
}

	void send_dimensions();

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {

//    char buffer[ BUFFER_SIZE ];
	/*
	 * Initialize rotaryencoders and buttons
	 */
	/* Set as input */
	DDRD &= ~(1<<DDD2);
	DDRB &= ~((1<<DDB7)|(1<<DDB0));
	/* Enable pull-up */
	PORTD |= (1<<PD2);
	PORTB |= (1<<PB7)|(1<<PB0);
	/* Enable IO-pin interrupt */
	PCICR |= (1<<PCIE2)|(1<<PCIE0);
	/* Unmask PB7, PB0 and PD2 */
	PCMSK2 |= (1<<PCINT18);
	PCMSK0 |= (1<<PCINT7)|(1<<PCINT0);

	txMsg.ExtendedFlag=1;
	txMsg.RemoteFlag=0;

	sei();
	bios->can_callback = &can_receive;

	lcd_init( LCD_DISP_ON );
	lcd_clrscr();

	send_dimensions();

    /* main loop */
	while(1){
        /* check if any messages have been received */
		if(msg_received){

			msg_received = FALSE;
		}
	}
}

void send_dimensions(){
	txMsg.Id=0xA000001;
	txMsg.Data.bytes[0] = SIZE_X;
	txMsg.Data.bytes[1] = SIZE_Y;
	txMsg.DataLength = 2;
	bios->can_send(&txMsg);
}
