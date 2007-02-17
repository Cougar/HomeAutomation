/**
 * CAN LCD.
 * For HD44780- and KS0073-based LCD displays
 * This is the early version that only prints temperature sensor data.
 *
 * @date    2006-12-11
 * @author  Erik Larsson
 *
 * TODO Fix more types of output: relay status, mail recieved etc.
 * Make it configurable to use UART, because it's not necessary on the final PCB
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
#include <clcd20x4.h>
/* funcdefs */
//#include <global_funcdefs.h>
#include <eqlazer_funcdefs.h>

/* defines */
#define BUFFER_SIZE 20
#define BOUNCE_TIME 10
#define TRUE 1
#define FALSE 0

uint8_t currentView = MAIN_VIEW, msg_received = FALSE, encoderPosition, rot = FALSE;
Can_Message_t rxMsg;
uint8_t temperatureData[8], servoPosition[4], //relayStatus[4], dimmerStatus[4],
            temperatureLength = 8, servoLength = 0; //, relayLength=0; //TODO mer generaliserat
uint32_t rot_timeStamp;
char buf[10];

ISR( PCINT2_vect ){

	rot_timeStamp = bios->timebase_get();
	rot = TRUE;
	/* stäng av interrupt på pinnarna */
	PCICR &= ~(1<<PCIE2);
}

void can_receive(Can_Message_t *msg){
	uint8_t temp, m;
	/* Check for incoming messages from IR-receiver node */
	if( CLASS_SNS == ((msg->Id & CLASS_MASK) >> CLASS_MASK_BITS) ){
		/* FIXME detta är väldigt fult och borde fixas
		 * knapp 4 på fjärren bläddrar åt vänster
		 * knapp 6 åt höger
		 */
		if( SNS_IR_RECEIVE == (msg->Id & TYPE_MASK)>>TYPE_MASK_BITS){
			if(msg->Data.bytes[0] == 0 && msg->Data.bytes[1] == 0 && msg->Data.bytes[2] == 0){
				if(msg->Data.bytes[3] == 4 && currentView>0){
					currentView--;
					msg_received = TRUE;
				}else if(msg->Data.bytes[3] == 6 && currentView<LAST_VIEW){
					currentView++;
					msg_received = TRUE;
				}
			}
		}
		/* Other sensor messages should be placed here */

		/* DS18S20 sensors */
		if( SNS_TEMP_DS18S20 == (msg->Id & SNS_TYPE_MASK)>>SNS_TYPE_BITS ){
			temp = (msg->Id & SNS_ID_MASK)>>SNS_ID_BITS;
			temperatureData[ (temp-1)*2 ] = msg->Data.bytes[0];
			temperatureData[ (temp-1)*2+1 ] = msg->Data.bytes[1];
		}

		/* Servo controllers */
		if( SNS_ACT_STATUS_SERVO == (msg->Id & TYPE_MASK)>>TYPE_MASK_BITS ){ // TODO annan struktur på statusmeddelanden från servo
			temp = msg->DataLength - 3;
			for(m=0;m<temp;m++){
				servoPosition[ m ] = msg->Data.bytes[m+3];
			}
			servoLength = temp;
		}

	}
}


/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {

    char buffer[ BUFFER_SIZE ];
    /*
     * Initialize rotaryencoders and buttons
     */
    /* Set as input and enable pull-up */
    DDRD &= ~( (1<<DDD6)|(1<<DDD7) );
//    PORTD |= (1<<PD6)|(1<<PD7);
    /* Enable IO-pin interrupt */
//    PCICR |= (1<<PCIE2);
    /* Unmask PD6 and PD7 */
    PCMSK2 |= (1<<PCINT22)|(1<<PCINT23);

    sei();
	bios->can_callback = &can_receive;

	lcd_init( LCD_DISP_ON );
	lcd_clrscr();

    lcd_clrscr();
    printLCDview( MAIN_VIEW );

    /* main loop */
	while(1){
        /* check if any messages have been received */
		if(msg_received){
                /* Print views skeleton and if existing its data */
			printLCDview( currentView );
			if(currentView == TEMPSENS_VIEW){
				printLCDviewData( TEMPSENS_VIEW, temperatureData, temperatureLength);
			}else if(currentView == SERVO_VIEW){
				printLCDviewData( SERVO_VIEW, servoPosition, servoLength);
			}
			msg_received = FALSE;
		}

        // TODO använd rotary encoders för att växla view
        // också med fjärrkontroll
/*		if((bios->timebase_get() - rot_timeStamp >= BOUNCE_TIME) && rot == TRUE){
			PCICR |= (1<<PCIE2);
			rot = FALSE;
		}*/
	}


}
