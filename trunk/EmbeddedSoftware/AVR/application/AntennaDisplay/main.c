/**
 * Antenna Display.
 * For controlling of the cAntenna bus using one
 * HD44780 LCD display and som buttons.
 *
 * 
 *
 * @date    2007-11-19
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
#include <stdlib.h>

/* lib files */
#include <bios.h>
#include <config.h>
#include <drivers/timer/timer.h>
#include <drivers/lcd/clcd/lcd_HD44780.h>

#include <string.h> //for memcpy

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define SIZE_X	16
#define SIZE_Y	2

#define BUFFER_SIZE SIZE_X

#define STOP 0
#define AZIMUTH 1
#define ELEVATION 2
#define SCREEN_NORMAL 3
#define SCREEN_CAL 4

#define NORMAL_MODE 0x01
#define CAL_MODE 0x02

#define AZIMUTH_PLUS 0x05
#define AZIMUTH_MINUS 0x06
#define ELEVATION_PLUS 0x07
#define ELEVATION_MINUS 0x08
#define AZIMUTH_STOP 0x09
#define ELEVATION_STOP 0x0a
#define GET_CAL 0x0b
#define SET_CAL 0x0c


#define APP_TYPE    0x1234
#define APP_VERSION 0x0001

volatile Can_Message_t rxMsg;

//char incoming_text[ BUFFER_SIZE ];
uint8_t rxMsgFull=0;

uint8_t mode = NORMAL_MODE;

// Buttons
uint8_t azimuth_plus = 0, azimuth_minus = 0;
uint8_t elevation_plus = 0, elevation_minus = 0;
uint8_t calibration = 0;


//void send_dimensions(void);
//void rotenc(void);

//void button(void);

void IO_init( void );

void print_screen( uint8_t type, uint16_t data );

void send_message( uint8_t type, uint16_t data );

void calibrate( void );

// CAN message reception callback
// This function runs with interrupts disabled, keep it as short as possible
void can_receive(Can_Message_t *msg){
	if(!rxMsgFull){
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}

void read_buttons(uint8_t timer) {
	// Periodicly read buttons
	// Read buttons
       	calibration = (PIND & (1<<PD2))?0:1;
       	azimuth_plus = (PINB & (1<<PB0))?0:1;
       	azimuth_minus = (PINB & (1<<PB7))?0:1;
	elevation_plus = (PINC & (1<<PC1))?0:1;
	elevation_minus = (PINC & (1<<PC0))?0:1;
}


/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main( void ) {
	uint8_t autostop = 0;

	sei();
	
	Timer_Init();

	IO_init();

	Can_Message_t txMsg;
	
	txMsg.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) | (NODE_ID << CAN_SHIFT_NMT_SID);
	txMsg.ExtendedFlag=1;
	txMsg.RemoteFlag=0;
	txMsg.DataLength = 4;
	
	txMsg.Data.words[0] = APP_TYPE;
	txMsg.Data.words[1] = APP_VERSION;
	
	// Send startup message
	BIOS_CanSend(&txMsg);
	
	BIOS_CanCallback = &can_receive;

	lcd_init( LCD_DISP_ON );

    	print_screen( SCREEN_NORMAL, 0 );
    
	// Timer for reading buttons
	Timer_SetTimeout(0, 50, TimerTypeFreeRunning, &read_buttons);
	Timer_SetTimeout(1, 500, TimerTypeFreeRunning, 0);
    
    /* main loop */
	while(1){
		
        /* check if any messages have been received */
		if( rxMsgFull ){

			if( rxMsg.Id == 0x1f8f0100 ){
				print_screen( AZIMUTH, rxMsg.Data.words[0] );
			}else if( rxMsg.Id == 0x1f8f0200 ){
				print_screen( ELEVATION, rxMsg.Data.words[0] );
			}
			
			rxMsgFull = 0;
		}
          
		if (Timer_Expired(1)) {

			// Perhaps someone pressed a button
			if( calibration || azimuth_plus || azimuth_minus || elevation_plus || elevation_minus ){
				if( calibration ){
					// send all stop
					send_message( AZIMUTH_STOP, 0);
					send_message( ELEVATION_STOP, 0);

					// Start calibration
					calibrate();

				}else{
			    	if( azimuth_plus ){
						//send rotate aximuth plus
			       	 	send_message( AZIMUTH_PLUS, 0 );
			    	}else if( azimuth_minus ){
		        		//send rotate azimuth minus
		        		send_message( AZIMUTH_MINUS, 0 );
			    	}else{
						// send azimuth stop
			       		send_message( AZIMUTH_STOP, 0 );
			    	}
	
			    	if( elevation_plus ){
						//send elevate plus
						send_message( ELEVATION_PLUS, 0 );
			    	}else if( elevation_minus ){
						//send elevate minus
						send_message( ELEVATION_MINUS, 0 );
			    	}else{
						//send elevation stop
						send_message( ELEVATION_STOP, 0 );
				    }

					autostop = 1; // Stop all rotors when no button is pressed
				}

			}else if ( autostop ){
				// send all stop
				send_message( AZIMUTH_STOP, 0);
				send_message( ELEVATION_STOP, 0);

				autostop = 0;
			}
		}
	}
}

void send_message( uint8_t type, uint16_t data ){
	Can_Message_t txMsg;

	txMsg.ExtendedFlag=1;
	txMsg.RemoteFlag=0;

    switch(type){
    
    case AZIMUTH_PLUS:
        txMsg.Id = 0x11100005UL;
        txMsg.Data.bytes[0] = 0x01;
        txMsg.DataLength = 1;
        break;
    
    case AZIMUTH_MINUS:
		txMsg.Id = 0x11100005UL;
		txMsg.Data.bytes[0] = 0x02;
        txMsg.DataLength = 1;
        break;
    
    case AZIMUTH_STOP:
        txMsg.Id = 0x11100005UL;
		txMsg.Data.bytes[0] = 0x00;
        txMsg.DataLength = 1;
        break;
    
    case ELEVATION_PLUS:
        txMsg.Id = 0x11100015UL;
        txMsg.Data.bytes[0] = 0x01;
        txMsg.DataLength = 1;
        break;
    
    case ELEVATION_MINUS:
		txMsg.Id = 0x111000015UL;
		txMsg.Data.bytes[0] = 0x02;
        txMsg.DataLength = 1;
        break;
    
    case ELEVATION_STOP:
        txMsg.Id = 0x11100015UL;
		txMsg.Data.bytes[0] = 0x00;
        txMsg.DataLength = 1;
        break;
        
    case GET_CAL:
    	txMsg.Id = 0x11100002UL; // MSG_CAL_GET
    	txMsg.DataLength = 0;
		break;
		
	case SET_CAL:
		txMsg.Id = 0x11100001UL; // MSG_CAL_SET
		txMsg.DataLength = 2;
		txMsg.Data.words[0] = data;
		break;
		
    default:
    	// Stop rotating
        txMsg.Id = 0x11100005UL;
		txMsg.Data.bytes[0] = 0x00;
        txMsg.DataLength = 1;
        break;
    }

    BIOS_CanSend(&txMsg);
}

void print_screen(uint8_t type, uint16_t data){
    char buffer[ BUFFER_SIZE ];

    if( SCREEN_NORMAL == type){ // rotation
        lcd_clrscr();
        lcd_puts("Azimuth     ---*\n");
        lcd_puts("Elevation   ---*\n");

    }else if( SCREEN_CAL == type){ // calibration
	lcd_clrscr();
        lcd_puts("Cal Azimuth ---*\n");
        lcd_puts("Cal Elevat. ---*\n");

    }else if( AZIMUTH == type ){ // azimuth position
		lcd_gotoxy(12,0);
		lcd_puts("   *");
		lcd_gotoxy(12,0);

	/*	if( data&0x8000 ){ //Negative position, -360 - 0
			//
			lcd_gotoxy(0,1);
			lcd_puts("     ");
			lcd_gotoxy(0,1);
			snprintf(buffer, BUFFER_SIZE, "%i", data);
		    lcd_puts(buffer);
			//
			lcd_gotoxy(12,0);
			lcd_puts("-");
			data = -data;
			snprintf(buffer, BUFFER_SIZE-1, "%i", data);
		    lcd_puts(buffer);
		}else{*/ //Positive position, 0 - 360
			snprintf(buffer, BUFFER_SIZE, "%i", data);
		    	lcd_puts(buffer);
		//}

	}else if( ELEVATION == type ){ // elevation position
		lcd_gotoxy(12,1);
		lcd_puts("   *");
		lcd_gotoxy(12,1);

		if( data&0x80 ){ //Negative position, -360 - 0
			lcd_puts("-");
			data = -data;
			snprintf(buffer, BUFFER_SIZE-1, "%i", data);
		    	lcd_puts(buffer);
		}else{ //Positive position, 0 - 360
			snprintf(buffer, BUFFER_SIZE, "%i", data);
		    	lcd_puts(buffer);
		}
	}
}

void IO_init( void )
{
	/*
	 * Enable buttons and display light
	 */

	// Set as inputs
	DDRD &= ~( 1<<DDD2 ); // Calibration
	DDRB &= ~(( 1<<DDB0 )|( 1<<DDB7 )); // Azimuth
	DDRC &= ~(( 1<<DDC0 )|( 1<<DDC1 )); // Elevation

	// Enable pull up
	PORTD |= ( 1<<PD2 );
	PORTB |= ( 1<<PB0 )|( 1<<PB7 );
	PORTC |= ( 1<<PC0 )|( 1<<PC1 );	

	// Contrast
	TCCR0A |= (1<<COM0B1)|(1<<WGM01)|(1<<WGM00);
	TCCR0B |= (1<<CS00);
	OCR0B = 0x02;
	DDRD |= (1<<DDD5);

	// Backlight
	TCCR1A |= (1<<COM1A1)|(1<<WGM11);
	TCCR1B |= (1<<WGM13)|(1<<WGM12)|(1<<CS10);
	ICR1 = 0xFF; // Make it 8 bits
	OCR1A = 0xC0;
	DDRB |= (1<<DDB1);
}


void calibrate( void ){
	int16_t calib_value_az = 0, calib_value_elv = 0;
	
	// flags
	uint8_t received_cal = 0, calibration_mode = 0;
	
	print_screen( SCREEN_CAL, 0 );
	
	send_message( GET_CAL, 0 );
	
	while ( 1 ) {
	
	        /* check if any messages have been received */
		if( rxMsgFull ){
			if( rxMsg.Id == 0x111000f2 && rxMsg.DataLength == 2 ){ // MSG_CAL_RET
				calib_value_az = rxMsg.Data.words[0];
				
				received_cal = 1;
				print_screen( AZIMUTH, calib_value_az );
			}
				
			rxMsgFull = 0;
		}
	
		if ( Timer_Expired(1) && received_cal ) { 
			if ( !calibration ){// wait for released button
				calibration_mode = 1;
				
				if(azimuth_plus){
			  		calib_value_az++;
					print_screen( AZIMUTH, calib_value_az );		        

		   		}else if(azimuth_minus){
			   		calib_value_az--;
					print_screen( AZIMUTH, calib_value_az );
		   		}
	
		   		if(elevation_plus){
			   		calib_value_elv++;
			    
		   		}else if(elevation_minus){
		   		 	calib_value_elv--;
		   		}	

			}else if( calibration && calibration_mode ){
				// Send calibration value to antenna node
				send_message( SET_CAL, calib_value_az );
				// return to normal state
				print_screen( SCREEN_NORMAL, 0 );
				
				return;
			}
		}
	}
}
