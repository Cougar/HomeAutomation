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

#define DEBUG 0

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdio.h>
/* lib files */
#include <can.h>
#include <serial.h>
#include <timebase.h>
#include <lcd_HD44780.h>
#include <clcd20x4.h>
/* funcdefs */
#include <global_funcdefs.h>
#include <eqlazer_funcdefs.h>

/* defines */
#define BUFFER_SIZE 20

#define LCD_CMD_GET 0x1600
#define VIEW_LEFT 0x01
#define VIEW_RIGHT 0x02
#define VIEW_EDIT 0x03


uint8_t currentView = MAIN_VIEW;

// TODO ISR() för att använda optoencoders
ISR( PCINT2_vect ){
    // TODO do the magic stuff
    // vriden medurs currentView++
    // moturs currentView--
} // TODO lägga denna koden i ett eget lib?

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {

    char buffer[ BUFFER_SIZE ];
    uint8_t m, temperatureData[8], servoPosition[4], relayStatus[4], dimmerStatus[4],
            temperatureLength, servoLength;

    /*
     * Initialize optoencoders and buttons
     */
    /* Set as input and enable pull-up */ // TODO fixa rätt ingångar
    DDRD &= ~( (1<<DDD6)|(1<<DDD7) );
    PORTD |= (1<<PD6)|(1<<PD7);
    /* Enable IO-pin interrupt */
    PCICR |= (1<<PCIE1);
    /* Unmask PD6 and PD7 */
    PCMSK2 |= (1<<PCINT22)|(1<<PCINT23);


    Timebase_Init();
#if DEBUG
    Serial_Init();
#endif
    sei();
    lcd_init( LCD_DISP_ON );
    lcd_clrscr();
    lcd_puts("CAN LCD test\n");
    lcd_puts("CanInit... ");
    if (Can_Init() != CAN_OK) {
        lcd_puts("FAILED!\n");
    }else{
        lcd_puts("OK!\n");
    }

    uint32_t timeStamp = 0;

    Can_Message_t txMsg;
    Can_Message_t rxMsg;
    txMsg.Id = 0;
    txMsg.RemoteFlag = 0;
    txMsg.ExtendedFlag = 1;

    lcd_clrscr();
    printLCDview( MAIN_VIEW );

    /* main loop */
    while (1) {
        /* service the CAN routines */
        Can_Service();

        /* check if any messages have been received and print to uart */
        while (Can_Receive(&rxMsg) == CAN_OK) {
#if DEBUG
            printf("MSG Received: ID=%lx, DLC=%u, EXT=%u, RTR=%u, ", rxMsg.Id, (uint16_t)(rxMsg.DataLength), (uint16_t)(rxMsg.ExtendedFlag), (uint16_t)(rxMsg.RemoteFlag));
            printf("data={ ");

            for (uint8_t i=0; i<rxMsg.DataLength; i++) {
                printf("%x ", rxMsg.Data.bytes[i]);
            }
            printf("}\n");
#endif
            /* Get data from bus and store */
            if( rxMsg.Id == 0x1502 ){
                /* Temperature sensor data */
                for(m=0; m<rxMsg.DataLength; m++){
                    temperatureData[m] = rxMsg.Data.bytes[m];
                    temperatureLength = rxMsg.DataLength;
                }
            }else if( rxMsg.Id == 0x1201 ){
                /* Relay status (ON/OFF) */
                relayStatus[1] = rxMsg.Data.bytes[2];
            }else if( rxMsg.Id == 0x1301){
                /* Servo and blinds status */
                for( m=3; m<rxMsg.DataLength; m++ ){
                    servoPosition[m-3] = rxMsg.Data.bytes[m];
                    servoLength=rxMsg.DataLength-3;
                }
            }
            /* TODO Add extra messages here */

            /* Incoming command to change view */
            if(rxMsg.Id == LCD_CMD_GET){
                if(rxMsg.Data.bytes[0]== VIEW_LEFT){
                    if(currentView>MAIN_VIEW){
                        currentView--;
                    }
                }else if(rxMsg.Data.bytes[0]== VIEW_RIGHT){
                    if(currentView<SERVO_VIEW){ /* TODO increase value if you add views */
                        currentView++;
                    }
                }

                /* Print views skeleton and if existing its data */
                printLCDview( currentView );
                if(currentView == TEMPSENS_VIEW){
                    printLCDviewData( TEMPSENS_VIEW, temperatureData, temperatureLength);
                }else if(currentView == SERVO_VIEW){
                    printLCDviewData( SERVO_VIEW, servoPosition, servoLength);
                }
            }
        }

        // TODO använd optoencoders för att växla view
        // också med fjärrkontroll

    }


}
