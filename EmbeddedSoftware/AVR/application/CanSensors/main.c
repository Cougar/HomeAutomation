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
#define USE_DS18S20
//#define USE_TC1047
#define USE_LDR

#define DS_SEND_PERIOD	10000
#define DS_SEND_DELAY	2000
#define TC_SEND_PERIOD	10000
#define LDR_SEND_PERIOD	10000

#include <funcdefs/funcdefs.h>

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
#include <timebase.h>

#if defined(USE_DS18S20)
#include <ds18x20.h>
#include <onewire.h>
#include <delay.h>
#endif

#if defined(USE_TC1047)
#include <tc1047.h>
#endif

#if defined(USE_LDR)
#include <ldr.h>
#endif

/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
#if defined(USE_DS18S20)
    uint8_t nSensors, i;
    uint8_t subzero, cel, cel_frac_bits;
    uint32_t timeStamp_DS = 0, timeStamp_DS_del = 0;
	/* This is to identify and give the sensors correct ID (For eqlazers net)
	 * TODO read ds18s20 serial id and that way give the "can id" */
	uint16_t sensor_ds_id[] = {SNS_DS18S20_FREEZER, SNS_DS18S20_INSIDE1, SNS_DS18S20_OUTSIDE, SNS_DS18S20_INSIDE2};
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

#if defined(USE_DS18S20)
    /* Make sure there is no more then 4 DS-sensors. */
    nSensors = search_sensors();
#endif

    Can_Message_t txMsg;

    txMsg.ExtendedFlag = 1;
    txMsg.DataLength = 2;

	/* Set class and sid (sid=NODE_ID from bios.h) */
	txMsg.Id = (CLASS_SNS << CLASS_MASK_BITS)|(NODE_ID);

    /* main loop */
    while (1) {
#if defined(USE_DS18S20)
        /* check temperature and send on CAN */
		if( Timebase_PassedTimeMillis(timeStamp_DS) >= DS_SEND_PERIOD ){
			timeStamp_DS = Timebase_CurrentTime();

			txMsg.Id &= ~TYPE_MASK; /* Clear TYPE */
			txMsg.Id |= (SNS_TEMP_DS18S20 << SNS_TYPE_BITS); /* Set SNS_TYPE */

            if ( DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL ) == DS18X20_OK) {
                delay_ms(DS18B20_TCONV_12BIT);
                for ( i=0; i<nSensors; i++ ) {
                    if ( DS18X20_read_meas( &gSensorIDs[i][0], &subzero,
                        &cel, &cel_frac_bits) == DS18X20_OK ) {

                        if (subzero) {
                            txMsg.Data.bytes[0] = -cel;
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
					txMsg.Id &= ~SNS_ID_MASK; /* Clear sensor id */
					txMsg.Id |= ( sensor_ds_id[i] << SNS_ID_BITS); /* Set sensor id */
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

			txMsg.Id &= ~TYPE_MASK; // rensa type
			txMsg.Id |= (SNS_TEMP_TC1047 << TYPE_MASK_BITS); // sätt korrekt type

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

			txMsg.Id &= ~TYPE_MASK; /* Clear TYPE */
			txMsg.Id |= (SNS_LIGHT_LDR << SNS_TYPE_BITS); /* Set SNS_TYPE */
			txMsg.Id &= ~SNS_ID_MASK; /* Clear sensor id */
			txMsg.Id |= ( SNS_LDR_TEST1 << SNS_ID_BITS); /* Set sensor id */

			BIOS_CanSend(&txMsg);
        }
#endif
    }

    return 0;
}
