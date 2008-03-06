/**
 * AntennaControl
 *
 * Build for use at ETA, http://www.eta.chalmers.se/, controlling their 2 meter yagi antenna.
 *
 * @date 2007-10-28
 * @author Erik Larsson
 *
 */

#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <avr/eeprom.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
#include <drivers/timer/timer.h>

#define APP_TYPE    0xf0a0
#define APP_VERSION 0x0001

#define AZIMUTH 0

#define ROTATE_STOP 0
#define ROTATE_PLUS 1
#define ROTATE_MINUS 2

#define SET 0
#define GET 1

#define STATUS 0

#define CALIBRATE_AZIMUTH 0 // EEPROM adress

// Make sure this is a power of 2
#define AVERAGE_SIZE 16
#define AVERAGE_SIZE_SHIFT 4

// Tillfälliga defines
#define MSG_CAL_SET 0x11100001UL
#define MSG_CAL_GET 0x11100002UL
#define MSG_ABS 0x111000031UL
#define MSG_REL 0x11100004UL
#define MSG_START 0x11100005UL
#define MSG_STOP 0x11100006UL
#define MSG_STATUS 0x11100007UL

#define MSG_CAL_RET 0x111000f2UL


// ----------

// Essential pins
// 0: PD2 azimuth plus
// 1: PD1 azimuth minus 
// 8: PC4 ADC4 azimuth feedback



// A simple message "queue", with space for one message only.
// These are declared volatile to tell the compiler not to optimize away accesses.
volatile Can_Message_t rxMsg; // Message storage
volatile uint8_t rxMsgFull;   // Synchronization flag

//uint16_t actualElevationValue;
//uint16_t desiredElevationValue;
//uint16_t actualAzimuthValue;
//uint16_t desiredAzimuthValue;

// For calculating average feedback measurement
uint16_t azimuthReadout[ AVERAGE_SIZE ];

uint16_t azimuthCalibration;

// CAN message reception callback.
// This function runs with interrupts disabled, keep it as short as possible.
void can_receive( Can_Message_t *msg ) {
	if (!rxMsgFull) {
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}

// Calibration function
// mode: set / get
// value:
int16_t calibration( uint8_t mode, uint16_t value );

// Turn rotor
// position: 0-1024
uint8_t turn( uint16_t position );

// Get position
// Gets the average value for compensation of distorsions
uint16_t getPosition( void );

// Initiate ADC
void initAdcFeedback( void );

// Read antenna feedback from ADC
void readFeedback( void );

// Control rotation relays
void controlRelay( uint8_t direction );

// Sends antennas position on CAN
void sendStatus( uint8_t type );


int main( void )
{
	// Enable interrupts as early as possible
	sei();
	
	Timer_Init();

	// Set as outputs and stop rotor
	DDRD |= (1 << PD1)|(1 << PD2);
	PORTD &= ~((1 << PD1)|(1 << PD2));
	
	// Setup ADC
	initAdcFeedback();
	
	Can_Message_t txMsg;
	txMsg.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) | (NODE_ID << CAN_SHIFT_NMT_SID);
	txMsg.DataLength = 4;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.Data.words[0] = APP_TYPE;
	txMsg.Data.words[1] = APP_VERSION;
	
	// Set up callback for CAN reception
	BIOS_CanCallback = &can_receive;
	// Send CAN_NMT_APP_START
	BIOS_CanSend(&txMsg);
	
	// Read calibration value from eeprom
	azimuthCalibration = eeprom_read_word( CALIBRATE_AZIMUTH );
	
	// Timer for reading position feedback
	Timer_SetTimeout(0, 100, TimerTypeFreeRunning, 0);
	Timer_SetTimeout(1, 1000, TimerTypeFreeRunning, 0);

	sendStatus( STATUS );

	while (1) {
		if (Timer_Expired(0)) {
			// Periodicly read antennas position
			readFeedback();
		}
		if (Timer_Expired(1)) {
			sendStatus(STATUS);
		}
		
		if (rxMsgFull) {
			switch (rxMsg.Id){
				case MSG_CAL_SET: // Set calibration value
					if( 2 == rxMsg.DataLength ){
						calibration( SET, rxMsg.Data.words[0] );
					}
					break;
					
				case MSG_CAL_GET: // Get calibration value
					if( 0 == rxMsg.DataLength ){
						txMsg.Id = MSG_CAL_RET; 
						txMsg.DataLength = 2;
						txMsg.Data.words[0] = calibration( GET, 0 );
						BIOS_CanSend(&txMsg);
					}
					break;
					
				case MSG_ABS: // Start turning to absolute position
					if( 2 == rxMsg.DataLength ){
						
					}
					break;
					
				case MSG_REL: // Start turning to relative position
					if( 2 == rxMsg.DataLength ){
						
					}
					break;
					
				case MSG_START: // Start turning
					if( 1 == rxMsg.DataLength ){
						// First data byte decides direction
						controlRelay( rxMsg.Data.bytes[0] );
					}
					break;
					
				case MSG_STOP: // Stop turning
					//if( 1 == rxMsg.DataLength ){
						controlRelay( ROTATE_STOP );
					//}
					break;
				case MSG_STATUS: // Get position
					if( 0 == rxMsg.DataLength ){
						sendStatus(STATUS);
					}
					break;
					
				default:
					break;
			}

    			rxMsgFull = 0; //  
		}
	}
	
	return 0;
}


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

	// Convert to degrees, gives about 361 degrees
	// start 508, stop 1023 => 516 bits per rotation
	// 516 * 7 / 10 = 361

	// Antennen rör sig inom 180 -> 360 (=0) -> 180
	// Norr = mittläget = 0 / 360 grader = 766 decimalt innan omräkning
	// Söder = ändlägena = 180 grader = 508 och 1023 innan omräkning


	// Make it between 508 and 1023
	if( position < 508 ){
		position = 0;
	}else{
		position -= 508;
	}
	
//	if( 0x8000&position ){
//		position = -position;
//		position *= 7;
//		position /= 10;
//		//position = -position;
//	}else{

		// Make it between 0 and 360
		position *= 7;
		position /= 10;
//	}

	// Move it to 180 and 540
	position += 180;
	
	// Calibrate
	position += (int16_t)azimuthCalibration;
	
	// North is now at 360
	if( position > 360 ){
		// Antenna is between 0 and 180
		position -= 360;
	}
	
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
	if( ROTATE_PLUS == direction ){ // Turn clockwise
		PORTD &= ~(1 << PD1);
		PORTD |= (1 << PD2);
	}else if ( ROTATE_MINUS == direction ){ // Turn counter clockwise
		PORTD &= ~(1 << PD2);
		PORTD |= (1 << PD1);
	}else{
		// Stop azimuth rotor
		PORTD &= ~(1 << PD1);
		PORTD &= ~(1 << PD2);
	}
}

void readFeedback( void )
{
	static uint8_t azimuthArrayPosition = 0;

	while( ADCSRA & ( 1 << ADSC )){} // Wait for conversion to complete
	
	azimuthReadout[ azimuthArrayPosition ] = ADCW; // Store result in ring buffer
	azimuthArrayPosition++;
	if( AVERAGE_SIZE <= azimuthArrayPosition ){
		azimuthArrayPosition = 0;
	}

	// Start next conversion
	ADCSRA |= ( 1 << ADSC );
}

void sendStatus( uint8_t type )
{
	Can_Message_t txMsg;
	
	//txMsg.Id = 0x1f8f0100UL; //(( CAN_SNS << CAN_SHIFT_CLASS )|( SNS_TYPE_STATUS << CAN_SHIFT_SNS_TYPE )|( SNS_ID_ANTENNA_STATUS << CAN_SHIFT_SNS_ID )|( NODE_ID << CAN_SHIFT_SNS_SID );
	
	txMsg.DataLength = 2;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	
//	txMsg.Data.words[ 0 ] = getPosition();

	if(STATUS == type){
		txMsg.Id = 0x1f8f0100UL;
		txMsg.Data.words[ 0 ] = getPosition();
		txMsg.DataLength = 2;
	}else{
		txMsg.Id = 0x12020202UL;
		txMsg.Data.bytes[ 0 ] = type;
		txMsg.DataLength = 1;
	}

	// Try to resend up to four times
	for(int i=0; i<3;i++){
		if( CAN_OK == BIOS_CanSend( &txMsg )){
			return;
		}
	}		
	
}
