/**
 * CanSensors.
 *
 * A general application for using multiple sensors.
 * DS18S20, TC1047...
 * 
 * @target	AVR
 * @date	2007-01-16
 * @author	Anders Runeson, Erik Larsson
 *
 * TODO: SHTxx
 */

/*-----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
/* lib files */
#include <bios.h>
#include <config.h>
#include <settings.h>

#include <drivers/timer/timebase.h>

#if defined(USE_DS18S20)
#include <drivers/sensor/ds18s20/ds18x20.h>
#include <drivers/sensor/ds18s20/onewire.h>
#include <drivers/sensor/ds18s20/delay.h>
#endif

#if defined(USE_TC1047)
#include <drivers/sensor/tc1047/tc1047.h>
#endif

#if defined(USE_LDR)
#include <drivers/sensor/ldr/ldr.h>
#endif

#define APP_TYPE    CAN_APPTYPES_SENSOR
#define APP_VERSION 0x0001

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
#if defined(USE_DS18S20)
    uint8_t nSensors, i;
    uint8_t subzero, cel, cel_frac_bits;
    uint32_t timeStamp_DS = 0, timeStamp_DS_del = 0;
	/* This is to identify and give the sensors correct ID 
	 * TODO read ds18s20 serial id and that way give the "can id" */
	uint16_t sensor_ds_id[] = {TEMPSENSORID_1, TEMPSENSORID_2, TEMPSENSORID_3, TEMPSENSORID_4};
#endif
#if defined(USE_TC1047)
    uint32_t tcTemperature = 0, timeStamp_TC = 0;
    adcTemperatureInit();
#endif
#if defined(USE_LDR)
	uint32_t timeStamp_LDR = 0;
	uint8_t ldr;
	LDR_SensorInit();
#endif
    sei();

	Timebase_Init();

    Can_Message_t txMsg;
	txMsg.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) | (NODE_ID << CAN_SHIFT_NMT_SID);
	txMsg.DataLength = 4;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.Data.words[0] = APP_TYPE;
	txMsg.Data.words[1] = APP_VERSION;
	BIOS_CanSend(&txMsg);

#if defined(USE_DS18S20)
    /* Make sure there is no more then 4 DS-sensors. */
    nSensors = search_sensors();
#endif

    txMsg.DataLength = 2;

    /* main loop */
    while (1) {
#if defined(USE_DS18S20)
        /* check temperature and send on CAN */
		if( Timebase_PassedTimeMillis(timeStamp_DS) >= DS_SEND_PERIOD ){
			timeStamp_DS = Timebase_CurrentTime();

            if ( DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL ) == DS18X20_OK) {
                delay_ms(DS18B20_TCONV_12BIT);
                for ( i=0; i<nSensors; i++ ) {
                    if ( DS18X20_read_meas( &gSensorIDs[i][0], &subzero, &cel, &cel_frac_bits) == DS18X20_OK ) {

                        if (subzero) {
                            txMsg.Data.bytes[0] = -cel-1;
                            txMsg.Data.bytes[1] = ~(cel_frac_bits<<4);
                        }else{
                            txMsg.Data.bytes[0] = cel;
                            txMsg.Data.bytes[1] = (cel_frac_bits<<4);
                        }
                    }
					/* Delay */
					timeStamp_DS_del = Timebase_CurrentTime();
					while(Timebase_PassedTimeMillis(timeStamp_DS_del) < DS_SEND_DELAY){}

					/* send txMsg */
					txMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_TEMPERATURE << CAN_SHIFT_SNS_TYPE) | (NODE_ID << CAN_SHIFT_SNS_SID));
					txMsg.Id |= ( sensor_ds_id[i] << CAN_SHIFT_SNS_ID); /* Set sensor id */
					BIOS_CanSend(&txMsg);
                }
            }
        }
#endif
#if defined(USE_TC1047)
#error tc1047 id is still not correct // FIXME
            /* check temperature and send on CAN */
		if( bios->timebase_get() - timeStamp_TC >= TC_SEND_PERIOD ){
			timeStamp_TC = bios->timebase_get();

            tcTemperature = getTC1047temperature();
            txMsg.Data.bytes[0] = tcTemperature & 0x00FF;
            txMsg.Data.bytes[1] = (tcTemperature & 0xFF00)>>8;
            txMsg.DataLength = 2;

			txMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_TEMPERATURE << CAN_SHIFT_SNS_TYPE) | (NODE_ID << CAN_SHIFT_SNS_SID));
			txMsg.Id |= (TEMPSENSORID_5 << CAN_SHIFT_SNS_ID); // sätt korrekt sensorid

            BIOS_CanSend(&txMsg);
        }
#endif
#if defined(USE_LDR)
            /* check light and send on CAN */
		if( Timebase_PassedTimeMillis(timeStamp_LDR) >= LDR_SEND_PERIOD ){
			timeStamp_LDR = Timebase_CurrentTime();

            ldr = (uint8_t)LDR_GetData(0);
            txMsg.Data.bytes[0] = ldr;
            txMsg.DataLength = 1;

			txMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_LIGHT << CAN_SHIFT_SNS_TYPE) | (LIGHTSENSORID_1 << CAN_SHIFT_SNS_ID) | (NODE_ID << CAN_SHIFT_SNS_SID));

			BIOS_CanSend(&txMsg);
        }
#endif
    }

    return 0;
}
