/**
 * CanSensors.
 *
 * A general application for using multiple sensors.
 * DS18S20, TC1047...
 * 
 * @target	AVR
 * @date	2007-01-16
 * @author	Jimmy Myhrman, Anders Runeson, Erik Larsson
 *
 * TODO: SHTxx
 */

/*-----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/
#define USE_DS18S20
//#define USE_TC1047

#define SENSOR_SEND 0x1500
#define SENSOR_GET 0x1501
#define SENSOR_SEND_DS 0x1502
#define SENSOR_SEND_TC 0x1503

#define DS_SEND_PERIOD 5000
#define TC_SEND_PERIOD 2000

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
#include <timebase.h>

#if defined(USE_DS18S20)
#include <onewire.h>
#include <ds18x20.h>
#include <delay.h>
#endif

#if defined(USE_TC1047)
#include <tc1047.h>
#endif

/*-----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
#if defined(USE_DS18S20)
    uint8_t nSensors, i;
    uint8_t subzero, cel, cel_frac_bits;
    uint32_t timeStamp_DS = 0;
#endif
#if defined(USE_TC1047)
    uint32_t tcTemperature = 0, timeStamp_TC = 0;
    adcTemperatureInit();
#endif
    Timebase_Init();
    sei();

#if defined(USE_DS18S20)
    /* Make sure there is no more then 4 DS-sensors. */
    nSensors = search_sensors();
#endif
    Can_Init();

    Can_Message_t txMsg;
    Can_Message_t rxMsg;

    txMsg.ExtendedFlag = 1;

    /* main loop */
    while (1) {
        /* service the CAN routines */
       Can_Service();

        /* check if any messages have been received */
        while (Can_Receive(&rxMsg) == CAN_OK) {

        }

#if defined(USE_DS18S20)
        /* check temperature and send on CAN */
        if (Timebase_PassedTimeMillis(timeStamp_DS) >= DS_SEND_PERIOD) {
            timeStamp_DS = Timebase_CurrentTime();

            if ( DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL ) == DS18X20_OK) {
                delay_ms(DS18B20_TCONV_12BIT);
                for ( i=0; i<nSensors; i++ ) {
                    if ( DS18X20_read_meas( &gSensorIDs[i][0], &subzero,
                        &cel, &cel_frac_bits) == DS18X20_OK ) {

                        //txMsg.Data.bytes[0] = subzero;
                        if (subzero) {
                            txMsg.Data.bytes[i*2] = -cel;
                            txMsg.Data.bytes[i*2+1] = ~(cel_frac_bits<<4);
                        }else{
                            txMsg.Data.bytes[i*2] = cel;
                            txMsg.Data.bytes[i*2+1] = (cel_frac_bits<<4);
                        }
                    }
                }
                txMsg.DataLength = nSensors*2;
                /* send txMsg */
                txMsg.Id = SENSOR_SEND_DS;
                Can_Send(&txMsg);
            }
        }
#endif
#if defined(USE_TC1047)
            /* check temperature and send on CAN */
        if (Timebase_PassedTimeMillis(timeStamp_TC) >= TC_SEND_PERIOD) {
            timeStamp_TC = Timebase_CurrentTime();

            tcTemperature = getTC1047temperature();
            txMsg.Data.bytes[0] = tcTemperature & 0x00FF;
            txMsg.Data.bytes[1] = (tcTemperature & 0xFF00)>>8;
            txMsg.DataLength = 2;
            txMsg.Id = SENSOR_SEND_TC;

            Can_Send(&txMsg);
        }
#endif
    }

    return 0;
}
