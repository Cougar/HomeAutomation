/**
 * AntennaDisplay
 * For controlling of the cAntenna bus using one
 * HD44780 LCD display and buttons.
 *
 * @date    2007-12-10
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
#include <bios.h>	// BIOS interface declarations, including CAN structure and ID defines.
#include <config.h>	// All configuration parameters
#include <drivers/timer/timer.h>
#include <drivers/lcd/clcd/lcd_HD44780.h>

#include <string.h> //for memcpy

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define APP_TYPE    0x1234
#define APP_VERSION 0x0002

// LCD size
#define SIZE_X	16
#define SIZE_Y	2

#define BUFFER_SIZE SIZE_X

#define STOP 0
#define AZIMUTH 1
#define ELEVATION 2
#define SCREEN_NORMAL 3
#define SCREEN_CAL 4

// Display modes
#define NORMAL_MODE 0x01
#define CAL_MODE 0x02

// Actions
#define AZIMUTH_PLUS 0x05
#define AZIMUTH_MINUS 0x06
#define ELEVATION_PLUS 0x07
#define ELEVATION_MINUS 0x08
#define AZIMUTH_STOP 0x09
#define ELEVATION_STOP 0x0a
#define GET_CAL 0x0b
#define SET_CAL 0x0c

volatile Can_Message_t rxMsg;

uint8_t rxMsgFull=0;

// Buttons
uint8_t azimuth_plus = 0, azimuth_minus = 0;
uint8_t elevation_plus = 0, elevation_minus = 0;
uint8_t calibration = 0;


/*-----------------------------------------------------------------------------
 * Declarations
 *---------------------------------------------------------------------------*/

/**
 * IO_init
 * Enable button inputs
 * Enable contrast and backlight for LCD
 *
 * @param void
 * @return void
 */
void IO_init( void );

/**
 * print_sceen
 * Prints normal and calibration data to LCD
 *
 * @param screen type, normal or calibration
 * @param data to print
 * @return void
 */
void print_screen( uint8_t type, uint16_t data );

/**
 * send_message
 * Handles all messages to antenna nodes
 *
 * @param type of message
 * @param data to send
 * @return void
 */
void send_message( uint8_t type, uint16_t data );

/**
 * calibrate
 * Gets calibration data from node, adjusts it and returns
 *
 * @param void
 * @return void
 */
void calibrate( void );

/**
 * read_buttons
 * Periodicly read buttons
 *
 * @param trigging timer
 * @return void
 */
void read_buttons(uint8_t timer) {
	// Read buttons
	calibration = (PIND & (1<<PD2))?0:1;
	azimuth_plus = (PINB & (1<<PB0))?0:1;
	azimuth_minus = (PINB & (1<<PB7))?0:1;
	elevation_plus = (PINC & (1<<PC1))?0:1;
	elevation_minus = (PINC & (1<<PC0))?0:1;
}

// CAN message reception callback
// This function runs with interrupts disabled, keep it as short as possible
void can_receive(Can_Message_t *msg){
	if(!rxMsgFull){
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main( void ) {
	// Flag for automatic sending stop message after released buttons
	uint8_t autostop = 0;

	sei();
	
	// Enable timers
	Timer_Init();

	// Enable buttons and display
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
    
	// Timer for reading buttons and doing actions
	Timer_SetTimeout(0, 50, TimerTypeFreeRunning, &read_buttons);
	Timer_SetTimeout(1, 500, TimerTypeFreeRunning, 0);
    
    /* main loop */
	while(1){
		
        /* check if any messages have been received */
		if( rxMsgFull ){

			if( rxMsg.Id == 0x1f8f0100 ){
				// Received new azimuth data and print it
				print_screen( AZIMUTH, rxMsg.Data.words[0] );
				
			}else if( rxMsg.Id == 0x1f8f0200 ){
				// Received new elevation data and print it
				print_screen( ELEVATION, rxMsg.Data.words[0] );
			}
			
			rxMsgFull = 0;
		}
        
		// Periodicly take care of button actions
		if (Timer_Expired(1)) {

			// Perhaps someone pressed a button
			if( calibration || azimuth_plus || azimuth_minus || elevation_plus || elevation_minus ){
				if( calibration ){
					// Enters calibration mode
					// Send all stop
					send_message( AZIMUTH_STOP, 0);
					send_message( ELEVATION_STOP, 0);

					// Start calibration
					calibrate();

				}else{
			    	if( azimuth_plus ){
						// Send rotate aximuth plus
			       	 	send_message( AZIMUTH_PLUS, 0 );
			       	 	
			    	}else if( azimuth_minus ){
		        		// Send rotate azimuth minus
		        		send_message( AZIMUTH_MINUS, 0 );
		        		
			    	}else{
						// Send azimuth stop
			       		send_message( AZIMUTH_STOP, 0 );
			    	}
	
			    	if( elevation_plus ){
						// Send elevate plus
						send_message( ELEVATION_PLUS, 0 );
						
			    	}else if( elevation_minus ){
						// Send elevate minus
						send_message( ELEVATION_MINUS, 0 );
						
			    	}else{
						// Send elevation stop
						send_message( ELEVATION_STOP, 0 );
				    }

					autostop = 1; // Stop all rotors when no button is pressed
				}

			}else if ( autostop ){
				// Send all stop
				// Only doing it once, no need to spam
				send_message( AZIMUTH_STOP, 0);
				send_message( ELEVATION_STOP, 0);

				autostop = 0;
			}
		}
	}
}

/*-----------------------------------------------------------------------------
 * Definitions
 *---------------------------------------------------------------------------*/

void send_message( uint8_t type, uint16_t data ){
	Can_Message_t txMsg;

	txMsg.ExtendedFlag=1;
	txMsg.RemoteFlag=0;

    switch(type){
    
    case AZIMUTH_PLUS:
		// Increase azimuth
        txMsg.Id = 0x11100005UL;
        txMsg.Data.bytes[0] = 0x01;
        txMsg.DataLength = 1;
        break;
    
    case AZIMUTH_MINUS:
    	// Decrease azimuth
		txMsg.Id = 0x11100005UL;
		txMsg.Data.bytes[0] = 0x02;
        txMsg.DataLength = 1;
        break;
    
    case AZIMUTH_STOP:
    	// Stop azimuth rotating
        txMsg.Id = 0x11100005UL;
		txMsg.Data.bytes[0] = 0x00;
        txMsg.DataLength = 1;
        break;
    
    case ELEVATION_PLUS:
    	// Increase elevation angle
        txMsg.Id = 0x11100015UL;
        txMsg.Data.bytes[0] = 0x01;
        txMsg.DataLength = 1;
        break;
    
    case ELEVATION_MINUS:
    	// Decrease elevation angle
		txMsg.Id = 0x111000015UL;
		txMsg.Data.bytes[0] = 0x02;
        txMsg.DataLength = 1;
        break;
    
    case ELEVATION_STOP:
    	// Stop elevation rotating
        txMsg.Id = 0x11100015UL;
		txMsg.Data.bytes[0] = 0x00;
        txMsg.DataLength = 1;
        break;
        
    case GET_CAL:
		// Get calibration data from nodes
		//FIXME add elevation
    	txMsg.Id = 0x11100002UL;
    	txMsg.DataLength = 0;
		break;
		
	case SET_CAL:
		// Set calibration data in nodes
		//FIXME add elevation
		txMsg.Id = 0x11100001UL;
		txMsg.DataLength = 2;
		txMsg.Data.words[0] = data;
		break;
		
    default:
    	// Stop all rotating
        txMsg.Id = 0x11100005UL;
		txMsg.Data.bytes[0] = 0x00;
        txMsg.DataLength = 1;
        break;
    }

    BIOS_CanSend(&txMsg);
}

/**
 * print_screen, 2x16 characters
 *
 * Normal screen (for rotating):
 * ##################
 * #Azimuth     183*#
 * #Elevation    45*#
 * ##################  
 *
 * Calibration screen:
 * ##################
 * #Cal Azimuth  20*#
 * #Cal Elevat.  12*#
 * ##################
 */

void print_screen(uint8_t type, uint16_t data){
    char buffer[ BUFFER_SIZE ];

    if( SCREEN_NORMAL == type){
    	// Normal screen for rotating
        lcd_clrscr();
        lcd_puts("Azimuth     ???*\n");
        lcd_puts("Elevation   ???*\n");

    }else if( SCREEN_CAL == type){
    	// Screen for calibration
		lcd_clrscr();
		lcd_puts("Cal Azimuth ???*\n");
		lcd_puts("Cal Elevat. ???*\n");

    }else if( AZIMUTH == type ){
    	// Prints azimuth position or calibration data
		lcd_gotoxy(12,0);
		lcd_puts("   *");
		lcd_gotoxy(12,0);

		snprintf(buffer, BUFFER_SIZE, "%i", data);
		lcd_puts(buffer);

	}else if( ELEVATION == type ){
		// Prints elevation position or calibration data
		lcd_gotoxy(12,1);
		lcd_puts("   *");
		lcd_gotoxy(12,1);

		snprintf(buffer, BUFFER_SIZE, "%i", data);
		lcd_puts(buffer);
	}
}

/**
 * Enable buttons and display light
 *
 * PB0: Azimuth plus
 * PB7: Azimuth minus
 * PC0: Elevation plus
 * PC1: Elevation minus
 * PD2: Calibration
 */
void IO_init( void )
{
	// Set as inputs
	DDRD |= ( 1<<DDD2 ); // Calibration
	DDRB |= ( 1<<DDB0 )|( 1<<DDB7 ); // Azimuth
	DDRC |= ( 1<<DDC0 )|( 1<<DDC1 ); // Elevation

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
	
	// Startup LCD
	lcd_init( LCD_DISP_ON );

	// Prints first characters to display
	print_screen( SCREEN_NORMAL, 0 );
}


void calibrate( void ){
	int16_t calib_value_az = 0, calib_value_elv = 0;
	
	// Flags
	uint8_t received_cal = 0, calibration_mode = 0;
	
	// Show calibration display
	print_screen( SCREEN_CAL, 0 );
	
	// Aquire old calibration data from node
	send_message( GET_CAL, 0 );
	
	while ( 1 ) {
	
		/* check if any messages have been received */
		if( rxMsgFull ){
			if( 0x111000f2 == rxMsg.Id && 2 == rxMsg.DataLength ){ // MSG_CAL_RET
				calib_value_az = rxMsg.Data.words[0];
				
				received_cal = 1;
				
				// Print old calibration data
				print_screen( AZIMUTH, calib_value_az );
			} //FIXME elevation
				
			rxMsgFull = 0;
		}
	
		/* When we got the old calibration data and we want to adjust it */
		if ( Timer_Expired(1) && received_cal ) { 
			if ( !calibration ){// wait for released calibration button
				calibration_mode = 1;
				
				if(azimuth_plus){
					// Increase value
			  		calib_value_az++;
					print_screen( AZIMUTH, calib_value_az );		        

		   		}else if(azimuth_minus){
					// Decrease value
			   		calib_value_az--;
					print_screen( AZIMUTH, calib_value_az );
		   		}

				// FIXME elevation	
		   		if(elevation_plus){
					// Increase value
			   		calib_value_elv++;
			    
		   		}else if(elevation_minus){
		   			// Decrease value
		   		 	calib_value_elv--;
		   		}	

			}else if( calibration && calibration_mode ){
				// Send new calibration value to antenna node
				send_message( SET_CAL, calib_value_az );
				//FIXME elevation

				// Return to normal screen
				print_screen( SCREEN_NORMAL, 0 );
				
				return;
			}
		}
	}
}
