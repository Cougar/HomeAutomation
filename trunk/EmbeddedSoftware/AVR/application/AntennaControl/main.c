/*
 * AntennaControl
 *
 * Build for use at ETA, http://www.eta.chalmers.se/, controlling their 2 meter quad antenna.
 *
 * @date 2007-09-22
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
//#include <drivers/uart/serial.h>
#include <drivers/timer/timer.h>

#define APP_TYPE    0xf0a0
#define APP_VERSION 0x0001

#define AZIMUTH 0
#define ELEVATION 1

#define ROTATE_STOP 0
#define ROTATE_PLUS 1
#define ROTATE_MINUS 2

#define SET 0
#define GET 1
#define CALIBRATE_ELEVATION 0
#define CALIBRATE_AZIMUTH 2

// Make sure this is a power of 2
#define AVERAGE_SIZE 16
#define AVERAGE_SIZE_SHIFT 4

// Essential pins
// 0: PD2 azimuth plus
// 1: PD1 azimuth minus
// 2: PD0 elevation plus
// 3: PD4 elevation minus
// 8: PC4 ADC4 azimuth feedback
// 9: PC5 ADC5 elevation feedback



// A simple message "queue", with space for one message only.
// These are declared volatile to tell the compiler not to optimize away accesses.
volatile Can_Message_t rxMsg; // Message storage
volatile uint8_t rxMsgFull;   // Synchronization flag

//uint16_t actualElevationValue;
//uint16_t desiredElevationValue;
//uint16_t actualAzimuthValue;
//uint16_t desiredAzimuthValue;

uint16_t azimuthReadout[ AVERAGE_SIZE ];
uint16_t elevationReadout[ AVERAGE_SIZE ];

// CAN message reception callback.
// This function runs with interrupts disabled, keep it as short as possible.
void can_receive( Can_Message_t *msg ) {
	if (!rxMsgFull) {
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}

// Calibration function
// axis: Elevation / Azimuth
// mode: set / get
// value:
int16_t calibration( uint8_t axis, uint8_t mode, uint16_t value );

// Turn rotors
// axis: Elevation / Azimuth
// position: 0-1024
uint8_t turn( uint8_t axis, uint16_t position );

// Get position
// axis: Elevation / Azimuth
// Gets the average value for compensation of distorsions
uint16_t getPosition( uint8_t axis );

// Read position
// axis: Elevation / Azimuth
// uses two adc
uint16_t readPosition(uint8_t axis);

void changeFeedbackAxis( uint8_t axis );

// Initiate ADC
void initAdcFeedback( void );

// Store result from ADC
void readAdcFeedback( void );

// Control rotation relays
// axis: Elevation / Azimuth
void controlRelay( uint8_t axis, uint8_t direction );

// Timer callback function used for some timer tests
void timer_callback( uint8_t timer ) {
	Can_Message_t msg;
	
	msg.ExtendedFlag = 1;
	msg.Id = (CAN_TST << CAN_SHIFT_CLASS) | NODE_ID;
	msg.RemoteFlag = 0;
	msg.DataLength = 1;
	msg.Data.bytes[0] = timer;
	
	BIOS_CanSend(&msg);
}


ISR( ADC_vect ) // ADC Conversion Complete
{
	
}

int main( void )
{
	// Enable interrupts as early as possible
	sei();
	
	Timer_Init();
//	Serial_Init();
	
//	unsigned long time;
	
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
	
	printf("AVR Test Application\n");
	
	txMsg.Id = (CAN_TST << CAN_SHIFT_CLASS) | NODE_ID;
//	txMsg.Data.dwords[0] = 0x01020304;
//	txMsg.DataLength = 8;

	// Set up three timers (assume at least three has been defined)
	// The timeout is specified in ticks, which is equal to ms if
	// the tick frequency is set to 1000.
	Timer_SetTimeout(0, 10, TimerTypeFreeRunning, 0);
//	Timer_SetTimeout(1, 10768, TimerTypeOneShot, &timer_callback);
//	Timer_SetTimeout(2, 3141, TimerTypeFreeRunning, &timer_callback);
	
	while (1) {
		if (Timer_Expired(0)) {
//			ad-omvandla
		}
		
		if (rxMsgFull) {
//			// Print the received message
//			printf("RX: ID=%08lx, DLC=%u, EXT=%u, RTR=%u, data={ ", 
//			        rxMsg.Id,
//			        (uint16_t)rxMsg.DataLength, 
//			        (uint16_t)rxMsg.ExtendedFlag, 
//			        (uint16_t)rxMsg.RemoteFlag);
//    		for (uint8_t i=0; i<rxMsg.DataLength; i++) {
//    			printf("%02x ", rxMsg.Data.bytes[i]);
//    		}
//    		printf("}\n");
			
			
    		rxMsgFull = 0; //  
		}
	}
	
	return 0;
}


int16_t calibration( uint8_t axis, uint8_t mode, uint16_t value )
{
	// set / get calibration value
	if( SET == mode ){
		if( ELEVATION == axis ){
			eeprom_write_word( CALIBRATE_ELEVATION, value );
		}else if( AZIMUTH == axis ){
			eeprom_write_word( CALIBRATE_AZIMUTH, value );
		}	
	}else if( GET == mode ){
		if( ELEVATION == axis ){
			return eeprom_read_word( CALIBRATE_ELEVATION );
		}else if( AZIMUTH == axis ){
			return eeprom_read_word( CALIBRATE_AZIMUTH );
		}		
	}
	return 0;
}

uint8_t turn( uint8_t axis, uint16_t position )
{
	// start relay
	// read feedback
	// callibrate measurement
	
	while(0){
		
	}
	return 0;
}

uint16_t getPosition( uint8_t axis )
{
/*	uint16_t position = 0;
	
	// Get average value of position
	for( uint8_t i ; i < 4 ; i++ ){
		position += readPosition( axis );
	}
	
	position = (position >> 2);
	
	return position;
*/
	return 0;
}

uint16_t readPosition( uint8_t axis )
{
	uint16_t averageValue = 0;
	uint16_t *measuredAxis;
	
	if( ELEVATION == axis ){
		// Calculate elevation value
		measuredAxis = elevationReadout;
	}else if( AZIMUTH == axis ){
		// Calculate aximuth value
		measuredAxis = azimuthReadout;
	}else{
		return 0;
	}

	for(uint8_t i = 0 ; i<AVERAGE_SIZE ; i++ ){
		// Summarize
		averageValue += measuredAxis[i];
	}
	
	averageValue = averageValue >> AVERAGE_SIZE_SHIFT;
		
		
		
	return averageValue;
}


void changeFeedbackAxis( uint8_t axis )
{
	if( AZIMUTH == axis ){
		// Enable ADC4
		ADMUX |= ( 1 << MUX2 );
		ADMUX &= ~(( 1 << MUX0 )|( 1 << MUX1 )|( 1 << MUX3 ));
	}else if( ELEVATION == axis ){
		// Enable ADC5
		ADMUX |= ( 1 << MUX0 )|( 1 << MUX2 );
		ADMUX &= ~(( 1 << MUX1 )|( 1 << MUX3 ));
	}	
}

void initAdcFeedback( void )
{
	// ADC4: Azimuth feedback
	// ADC5: Elevation feedback
	
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
	
	// Auto Trigger
//	ADCSRA |= ( 1 << ADATE );
	
	// Disable digital input
	DIDR0 |= ( 1 << ADC5D )|( 1 << ADC4D );
	
	// Wake uo ADC and enable it
	PRR &= ~( 1 << PRADC );
	ADCSRA |= ( 1 << ADEN );
}

void controlRelay( uint8_t axis, uint8_t direction )
{
	if( AZIMUTH == axis ){
		
		if( ROTATE_PLUS == direction ){
			PORTD &= ~(1 << PD2);
			PORTD |= (1 << PD1);
		}else if ( ROTATE_MINUS == direction ){
			PORTD &= ~(1 << PD1);
			PORTD |= (1 << PD2);
		}else{
			// stop azimuth rotor
			PORTD |= (1 << PD1);
			PORTD |= (1 << PD2);
		}
		
	}else if( ELEVATION == axis ){
		
		if( ROTATE_PLUS == direction ){
			PORTD &= ~(1 << PD0);
			PORTD |= (1 << PD4);
		}else if ( ROTATE_MINUS == direction ){
			PORTD &= ~(1 << PD4);
			PORTD |= (1 << PD0);
		}else{
			// stop elevation rotor
			PORTD |= (1 << PD0);
			PORTD |= (1 << PD4);
		}
		
	}else{
		// stop all rotors
		PORTD |= (1 << PD0);
		PORTD |= (1 << PD1);
		PORTD |= (1 << PD2);
		PORTD |= (1 << PD4);
	}
	
}

void readAdcFeedback( void )
{
	static uint8_t azimuthArrayPosition = 0;
	static uint8_t elevationArrayPosition = 0;
	static uint8_t lastAxis = AZIMUTH;
	
	while( ADCSRA & (1 << ADSC) ); // Wait for conversion to be done
		
	// Get measurement
	if( AZIMUTH == lastAxis ){
		azimuthReadout[ azimuthArrayPosition ] = ADCW;
		azimuthArrayPosition++;
		if( AVERAGE_SIZE <= azimuthArrayPosition ){
			azimuthArrayPosition = 0;
		}
		
		lastAxis = ELEVATION;
		
	}else if( ELEVATION == lastAxis ){
		elevationReadout[ elevationArrayPosition ] = ADCW;
		elevationArrayPosition++;
		if( AVERAGE_SIZE <= elevationArrayPosition ){
			elevationArrayPosition = 0;
		}
		
		lastAxis = AZIMUTH;
	}
	
	// Next time read other axis
	changeFeedbackAxis( lastAxis );
	
	// Start next measurement
	ADCSRA |= (1 << ADSC);
}
