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

#define DEBUG 1

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

uint8_t currentView = MAIN_VIEW;
/*static const PROGMEM unsigned char symbolThermometer[] =
{
    0x04, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0E, 0x0E,
    0x0E, 0x0E, 0x0E, 0x0A, 0x11, 0x11, 0x0A, 0x04
};*/ // TODO behövs symbol för termometer?

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
    uint8_t subzero; // TODO städa upp
    uint8_t m, temperatureData[8], relayStatus[4], dimmerStatus[4];

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
#if DEBUG
    printf("\n------------------------------------------------------------\n");
    printf(  "   CAN LCD\n");
    printf(  "------------------------------------------------------------\n");
#endif
    lcd_clrscr();
    lcd_puts("CAN LCD test\n");
#if DEBUG
    printf("CanInit...");
    lcd_puts("CanInit... ");
    if (Can_Init() != CAN_OK) {
        printf("FAILED!\n");
        lcd_puts("FAILED!\n");
    }
    else {
        printf("OK!\n");
        lcd_puts("OK!\n");
    }
#else
    lcd_puts("CanInit... ");
    if (Can_Init() != CAN_OK) {
        lcd_puts("FAILED!\n");
    }else{
        lcd_puts("OK!\n");
    }
#endif
    uint32_t timeStamp = 0;

    Can_Message_t txMsg;
    Can_Message_t rxMsg;
    txMsg.Id = 0;
    txMsg.RemoteFlag = 0;
    txMsg.ExtendedFlag = 1;

    lcd_clrscr();

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
            if( rxMsg.Id == 0x300 ){
                /* Temperature sensor data */
                for(m=0; m<rxMsg.DataLength; m++){
                    temperatureData[m] = rxMsg.Data.bytes[m];
                }
            }else if( rxMsg.Id == 0x1201 ){
                /* Relay status (ON/OFF) */
                relayStatus[1] = rxMsg.Data.bytes[2];
            } /* TODO Add extra messages here */
        }

        /* Print views */
        // TODO använd optoencoders för att växla view
        printLCDview( MAIN_VIEW );

    }

    return 0;
}
