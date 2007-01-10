/**
 * CanGPS.
 *
 * @date    2001-01-09
 * @author  Erik Larsson
 */

#define DEBUG           0 /* Prints debug messages to uart, only use this function if output is not connected to GPS */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdio.h>
#include <string.h>
/* lib files */
#include <can.h>
#include <serial.h>
#include <uart.h>
#include <timebase.h>

/* defines */
#define GPS_CMD_GET     0x1400
#define GPS_CMD_SEND    0x1401
#define STATUS_SEND_PERIOD 500 /* milliseconds */

#define TRUE 1
#define FALSE !TRUE
#define CR 0x0D
#define LF 0x0A

/*-------------------------------------------------------------------------
 * Global variables
 * -----------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Interrupt routines
 * -------------------------------------------------------------------------*/
ISR( PCINT2_vect )
{
}

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {

    uint32_t timeStamp = 0, timeStampTurn = 0;
    uint8_t buffering = FALSE, gpsMsg_received = FALSE, rxGps_element;
    char buffer[100], rxGps;

    Timebase_Init();
    Serial_Init();

    sei();
#if DEBUG
    printf("\n------------------------------------------------------------\n");
    printf(  "   CAN: GPS\n");
    printf(  "------------------------------------------------------------\n");

    printf("CanInit...");
    if (Can_Init() != CAN_OK) {
        printf("FAILED!\n");
    }else{
        printf("OK!\n");
    }
#else
    Can_Init();
#endif

    Can_Message_t txMsg;
    Can_Message_t rxMsg;
    txMsg.Id = GPS_CMD_SEND;
    txMsg.RemoteFlag = 0;
    txMsg.ExtendedFlag = 1;
    txMsg.DataLength = 3;

    /* main loop */
    while (1) {
        /* service the CAN routines */
        Can_Service();

        /* check if any messages have been received */
        while (Can_Receive(&rxMsg) == CAN_OK) {
            /* This node that control a servo is adressed */
            if( rxMsg.Id == GPS_CMD_GET ){

            }
        }

        if( Timebase_PassedTimeMillis(timeStamp) >= STATUS_SEND_PERIOD ){
            timeStamp = Timebase_CurrentTime();
            /* Send blinds status to CAN */

        }

        /* Get data from GPS */
        rxGps = uart_getc();
        while( rxGps != UART_NO_DATA ){
            /* '$' indicates start of message, start buffering */
            if( rxGps == '$' ){
                buffering = TRUE;
                rxGps_element = 0;
            }else if( rxGps == CR ){
                /* End of message */
                buffer[rxGps_element]='\0';
                buffering = FALSE;
                gpsMsg_received = TRUE;
            }

            if( buffering ){
                buffer[ rxGps_element ] = rxGps;
                rxGps_element++;
            }
            /* Get next character */
            rxGps = uart_getc();
        }

        if( gpsMsg_received ){
            /* One whole msg is completed, start compare and parse */
            if( !strncmp(buffer, "GPGGA", 5) ){
                /* Standard NMEA message, $GPGGA */

            }else if( !strncmp(buffer, "GPVTG", 5) ){
                /* Standard NMEA message, $GPVTG */

            }else if( !strncmp(buffer, "GPZDA", 5) ){
                /* Standard NMEA message, $GPZDA */
            }

            gpsMsg_received = FALSE; /* Get ready for next message */

        }
    }
    return 0;
}

