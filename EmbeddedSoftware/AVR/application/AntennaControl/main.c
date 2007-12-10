/**
 * AntennaControl
 *
 * Build for use at ETA, http://www.eta.chalmers.se/
 * controlling their 2 meter yagi antenna.
 * For the moment only support for azimuth,
 * elevation is yet to be implemented.
 *
 * @date 2007-12-10
 * @author Erik Larsson
 *
 */

/*
 * This version uses a NodeEssential
 * and the pins used are:
 * 0: PD2 azimuth plus
 * 1: PD1 azimuth minus 
 * 8: PC4 ADC4 azimuth feedback
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <avr/eeprom.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
#include <drivers/timer/timer.h>

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define APP_TYPE    0xf0a0
#define APP_VERSION 0x0002

#define AZIMUTH 0

#define ROTATE_STOP 0
#define ROTATE_PLUS 1
#define ROTATE_MINUS 2

#define SET 0
#define GET 1

#define CALIBRATE_AZIMUTH 0 // EEPROM adress

// Make sure this is a power of 2
#define AVERAGE_SIZE 16
#define AVERAGE_SIZE_SHIFT 4

//FIXME move these message id defines
#define MSG_CAL_SET 0x11100001UL
#define MSG_CAL_GET 0x11100002UL
#define MSG_ABS 0x111000031UL
#define MSG_REL 0x11100004UL
#define MSG_START 0x11100005UL
#define MSG_STOP 0x11100006UL
#define MSG_STATUS 0x11100007UL
#define MSG_CAL_RET 0x111000f2UL


// A simple message "queue", with space for one message only.
// These are declared volatile to tell the compiler not to optimize away accesses.
volatile Can_Message_t rxMsg; // Message storage
volatile uint8_t rxMsgFull;   // Synchronization flag

// For calculating average feedback measurement
uint16_t azimuthReadout[ AVERAGE_SIZE ];

uint16_t azimuthCalibration;

/*-----------------------------------------------------------------------------
 * Declarations
 *---------------------------------------------------------------------------*/
 
/**
 * calibration
 * Sets and gets calibration data from EEPROM
 *
 * @param mode: set /get
 * @param value
 * @return value
 */
int16_t calibration( uint8_t mode, uint16_t value );

// Turn rotor
// position: 0-1024
/**
 * turn
 * Turns rotor to given position
 * FIXME implement this function
 *
 * @param position
 * @return 
 */
uint8_t turn( uint16_t position );

/**
 * getPosition
 * Calculates the antenna position
 * Uses a number of the latest adc readings
 * and gets the avarage value
 *
 * @param void
 * @return position
 */
uint16_t getPosition( void );

/**
 * initAdcFeedback
 * Starts the ADC
 *
 * @param void
 * @return void
 */
void initAdcFeedback( void );

/**
 * readFeedback
 * Gets antenna position from ADC and 
 * puts in arrary for later calculations
 *
 * @param void
 * @return void
 */
void readFeedback( void );

/**
 * controlRelay
 * Control the relay that turns the rotor
 *
 * @param direction
 * @return void
 */
void controlRelay( uint8_t direction );

/**
 * sendStatus
 * Sends antennas position on CAN bus
 *
 * @param void
 * @return void
 */
void sendStatus( void );

// CAN message reception callback.
// This function runs with interrupts disabled, keep it as short as possible.
void can_receive( Can_Message_t *msg ) {
	if (!rxMsgFull) {
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main( void )
{
	// Enable interrupts as early as possible
	sei();
	
	Timer_Init();

    DDRD |= (1 << PD1)|(1 << PD2);
    PORTD |= (1 << PD1)|(1 << PD2);

	// Setup ADC
	initAdcFeedback();
	
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
	
	// Read calibration value from eeprom
	azimuthCalibration = eeprom_read_word( CALIBRATE_AZIMUTH );
	
	// Timer for reading position feedback
	Timer_SetTimeout(0, 50, TimerTypeFreeRunning, 0);
	Timer_SetTimeout(1, 1000, TimerTypeFreeRunning, 0);

    sendStatus();

	while (1) {
		if (Timer_Expired(0)) {
			// Periodicly read antennas position
			readFeedback();
		}
		if (Timer_Expired(1)) {
			// Send antennas position
			sendStatus();
		}
		
		/* If any messages is received */
		if (rxMsgFull) {

			switch ( rxMsg.Id ){
				case MSG_CAL_SET:
					// Set calibration value
					if( 2 == rxMsg.DataLength ){

						calibration( SET, rxMsg.Data.words[0] );
					}
					
					break;
					
				case MSG_CAL_GET:
					// Get calibration value
					if( 0 == rxMsg.DataLength ){

						txMsg.Id = MSG_CAL_RET; 
						txMsg.DataLength = 2;
						txMsg.Data.words[0] = calibration( GET, 0 );

						BIOS_CanSend(&txMsg);
					}
					
					break;
					
				case MSG_ABS:
					// Start turning to absolute position
					if( 2 == rxMsg.DataLength ){

						//FIXME implement this
					}
					
					break;
					
				case MSG_REL:
					// Start turning to relative position
					if( 2 == rxMsg.DataLength ){

						//FIXME implement this
					}
					
					break;
					
				case MSG_START:
					// Start turning rotor
					if( 1 == rxMsg.DataLength ){

						// First data byte decides direction
						controlRelay( rxMsg.Data.bytes[0] );
					}
					
					break;
					
				case MSG_STOP:
					// Stop turning rotor
					
					controlRelay( ROTATE_STOP );
					
					break;
					
				case MSG_STATUS:
					// Get position
					if( 0 == rxMsg.DataLength ){
						sendStatus();
					}
					
					break;
					
				default:
					break;
			}

    		rxMsgFull = 0;
		}
	}
	
	return 0;
}

/*-----------------------------------------------------------------------------
 * Definitions
 *---------------------------------------------------------------------------*/

int16_t calibration( uint8_t mode, uint16_t value )
{
	// set / get calibration value
	if( SET == mode ){
		eeprom_write_word( CALIBRATE_AZIMUTH, value );
		azimuthCalibration = value;
	
	}else if( GET == mode ){
		return azimuthCalibration;
	}
	return 0;
}

uint8_t turn( uint16_t position )
{
	//FIXME implement this
	// start relay
	// read feedback
	// callibrate measurement
	
	while(0){
		
	}
	return 0;
}

uint16_t getPosition( void )
{
	uint16_t position = 0;
	
	// Get average value of position
	for( uint8_t i=0; i < AVERAGE_SIZE; i++ ){
		position += azimuthReadout[i];
	}
	
	// Calculate average
	position = position >> AVERAGE_SIZE_SHIFT;

	// Convert to degrees, gives about 379 degrees
	position /= 3;

	// Calibrate
	position += (int16_t)azimuthCalibration;
	
	return position;
}

void initAdcFeedback( void )
{
	// ADC4: Azimuth feedback
	
	// Enable ADC4
	ADMUX |= ( 1 << MUX2 );
	ADMUX &= ~(( 1 << MUX0 )|( 1 << MUX1 )|( 1 << MUX3 ));

	// Prescaler /128
	ADCSRA |= ( 1 << ADPS2)|( 1 << ADPS1)|( 1 << ADPS0);
	
	// Enable AVcc as Voltage Reference
	ADMUX |= ( 1 << REFS0 );
	ADMUX &= ~( 1 << REFS1 );
	
	// Right adjust the result
	ADMUX &= ~( 1 << ADLAR );
	
	// Disable digital input
	DIDR0 |= ( 1 << ADC5D )|( 1 << ADC4D );
	
	// Wake up ADC and enable it
	PRR &= ~( 1 << PRADC );
	ADCSRA |= ( 1 << ADEN );
	
	// Start first conversion
	ADCSRA |= ( 1 << ADSC );
}

void controlRelay( uint8_t direction )
{
	if( ROTATE_PLUS == direction ){
		// Turn CW
		PORTD &= ~(1 << PD2);
		PORTD |= (1 << PD1);
	}else if ( ROTATE_MINUS == direction ){
		// Turn CCW
		PORTD &= ~(1 << PD1);
		PORTD |= (1 << PD2);
	}else{
		// Stop azimuth rotor
		PORTD |= (1 << PD1);
		PORTD |= (1 << PD2);
	}
}

void readFeedback( void )
{
	static uint8_t azimuthArrayPosition = 0;

	// Wait for ADC conversion to complete
	while( ADCSRA & ( 1 << ADSC )){} 
	
	// Put readout in ringbuffer
	azimuthReadout[ azimuthArrayPosition ] = ADCW;
	azimuthArrayPosition++;
	
	if( AVERAGE_SIZE <= azimuthArrayPosition ){
		azimuthArrayPosition = 0;
	}
	
	// Start next conversion
	ADCSRA |= ( 1 << ADSC );
}

void sendStatus( void )
{
	// Gets antennas position and sends on CAN bus
	Can_Message_t txMsg;
	
	txMsg.Id = 0x1f8f0100UL; //FIXME id should not be defined here
	
	txMsg.DataLength = 2;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	
	txMsg.Data.words[ 0 ] = getPosition();
	
	BIOS_CanSend( &txMsg );
	
}

