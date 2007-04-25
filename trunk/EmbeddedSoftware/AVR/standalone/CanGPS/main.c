/**
 * CanGPS.
 *
 * Messages: <byte0><byte1>...<byte7>
 * $GPGGA: <GGA_MSG><UTC><Latitude><Latitude dec.><N/S><Longitude><Longitude dec.><E/W>
 * $GPVTG: <VTG_MSG><Course true><Course dec. true><Course magnetic><Course dec. magnetic><Speed km/h><Speed dec. km/h>
 * $GPZDA: <ZDA_MSG><hh><mm><ss><day><month><year>
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

#define GGA_MSG     0x01
#define VTG_MSG     0x02
#define ZDA_MSG     0x03
#define GGA_MSG_BYTES 8
#define VTG_MSG_BYTES 7
#define ZDA_MSG_BYTES 7

#define TRUE 1
#define FALSE !TRUE
#define CR 0x0D
#define LF 0x0A
#define GPS_MSG_DELIMITER ","

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
    /* GPGGA, Global positioning system fix data */
    char gga_utc[7], gga_latitude[9], gga_latitude_NS, gga_longitude[10], gga_longitude_EW;
    /* GPVTG, Course over ground and speed */
    char vtg_course_true[6], vtg_course_magnetic[6], vtg_speed_kmh[6];
    /* GPZDA, Time & Date */
    char zda_time[7], zda_day[3], zda_month[3], zda_year[5];

    char *pch;

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
                strtok(buffer, GPS_MSG_DELIMITER); /* Throw away "GPGGA" */

                /* Get UTC */
                pch = strtok(NULL, GPS_MSG_DELIMITER);
                strcpy(gga_utc, pch);

                /* Get latitude */
                pch = strtok(NULL, GPS_MSG_DELIMITER);
                strcpy(gga_latitude, pch);

                /* Get latitude N/S */
                pch = strtok(NULL, GPS_MSG_DELIMITER);
                strcpy(gga_latitude_NS, pch);

                /* Get longitude */
                pch = strtok(NULL, GPS_MSG_DELIMITER);
                strcpy(gga_longitude, pch);

                /* Get longitude E/W */
                pch = strtok(NULL, GPS_MSG_DELIMITER);
                strcpy(gga_longitude_EW, pch);

            }else if( !strncmp(buffer, "GPVTG", 5) ){
                /* Standard NMEA message, $GPVTG */
                strtok(buffer, GPS_MSG_DELIMITER); /* Throw away "GPVTG" */

                /* Get course over ground, true degree */
                pch = strtok(NULL, GPS_MSG_DELIMITER);
                strcpy(vtg_course_true, pch);

                /* Get course over ground, magnetic degree */
                pch = strtok(NULL, GPS_MSG_DELIMITER);
                strcpy(vtg_course_magnetic, pch);

                /* Get speed over ground, km/h */
                pch = strtok(NULL, GPS_MSG_DELIMITER);
                strcpy(vtg_speed_kmh, pch);

            }else if( !strncmp(buffer, "GPZDA", 5) ){
                /* Standard NMEA message, $GPZDA */
                strtok(buffer, GPS_MSG_DELIMITER); /* Throw away "GPZDA" */

                /* Get time */
                pch = strtok(NULL, GPS_MSG_DELIMITER);
                strcpy(zda_time, pch);

                /* Get day, 0-31 */
                pch = strtok(NULL, GPS_MSG_DELIMITER);
                strcpy(zda_day, pch);

                /* Get month, 1-12 */
                pch = strtok(NULL, GPS_MSG_DELIMITER);
                strcpy(zda_month, pch);

                /* Get year */
                pch = strtok(NULL, GPS_MSG_DELIMITER);
                strcpy(zda_year, pch);
            }

            gpsMsg_received = FALSE; /* Get ready for next message */

        }
    }
    return 0;
}

