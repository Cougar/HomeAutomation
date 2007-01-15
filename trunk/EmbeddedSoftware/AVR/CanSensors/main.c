/**
 * CanSensors.
 *
 * A general application for using multiple sensors.
 * ie DS18S20, SHT71...
 * 
 * @target	AVR
 * @date	2006-12-09
 * @author	Jimmy Myhrman, Anders Runeson, Erik Larsson
 *   
 * Högst experimentell, decimaldel fel vid negativ temperatur? 
 * negativ temperatur helt otestad!
 * 
 */

//#define DEBUG
#define USE_DS18S20
#define USE_SHT71
#define USE_TC1047

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

#ifdef DEBUG
#include <uart.h>
#endif

#ifdef USE_DS18S20
#include <onewire.h>
#include <ds18x20.h>
#include <delay.h>
#endif

#ifdef USE_TC1047
#include <tc1047.h>
#endif

#ifdef USE_SHT71
#include <sht7x.h>
#endif
/*-----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
#ifdef USE_DS18S20
	uint8_t nSensors, i;
	uint8_t subzero, cel, cel_frac_bits;
#endif
	Mcu_Init();
	Timebase_Init();
#ifdef DEBUG
	Serial_Init();
#endif
#ifdef USE_TC1047
    uint32_t tcTemperature = 0;
    adcTemperatureInit();
#endif
#ifdef USE_SHT71
    HumidityInit();
#endif
	sei();
#ifdef DEBUG
#ifdef USE_DS18S20
	printf( "\nDS18X20 1-Wire-Reader\n" );
	printf( "-----------------------" );
	nSensors = search_sensors();
	printf( "%i DS18X20 Sensor(s) available:\n", (int) nSensors );
	
	for (i=0; i<nSensors; i++) {
		printf("Sensor# %i is a ", (int) i+1);
		if ( gSensorIDs[i][0] == DS18S20_ID)
			printf("DS18S20/DS1820");
		else printf("DS18B20");
		printf(" which is ");
		if ( DS18X20_get_power_status( &gSensorIDs[i][0] ) ==
			DS18X20_POWER_PARASITE ) 
			printf( "parasite" );
		else printf( "externally" ); 
		printf( " powered\n" );
	}
	
	printf("CanInit...\n");
	if (Can_Init() != CAN_OK) {
		printf("FAILED!\n");
	}
	else {
		printf("OK!\n");
	}
#endif
#else
	Can_Init();
#endif

    uint32_t timeStamp = 0;
	
	Can_Message_t txMsg;
	Can_Message_t rxMsg;
	
	txMsg.DataLength = 2;
	txMsg.Id = 0x450;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	
	/* main loop */
	while (1) {
		/* service the CAN routines */
		Can_Service();

		/* check if any messages have been received */
		while (Can_Receive(&rxMsg) == CAN_OK) {
			
		}

		/* check temperature and send on CAN once every other second */
		if (Timebase_PassedTimeMillis(timeStamp) >= 2000) {
			timeStamp = Timebase_CurrentTime();

#ifdef USE_DS18S20
			if ( DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL ) == DS18X20_OK) {
#ifdef DEBUG
				printf("Measuring temperature... ");
#endif
				delay_ms(DS18B20_TCONV_12BIT);
				for ( i=0; i<nSensors; i++ ) {
					if ( DS18X20_read_meas( &gSensorIDs[i][0], &subzero,
							&cel, &cel_frac_bits) == DS18X20_OK ) {
						
						//txMsg.Data.bytes[0] = subzero;
						if (subzero) {
							txMsg.Data.bytes[i*2] = -cel;
							txMsg.Data.bytes[i*2+1] = ~(cel_frac_bits<<4);
						} else {
							txMsg.Data.bytes[i*2] = cel;
							txMsg.Data.bytes[i*2+1] = (cel_frac_bits<<4);
						}
						
					}
#ifdef DEBUG
					else printf("CRC Error (lost connection?)\n");
#endif
				}
				
				txMsg.DataLength = nSensors*2;
#ifdef DEBUG
				printf("sending...\n");
#endif
				/* send txMsg */
				Can_Send(&txMsg);
			}
#ifdef DEBUG
			else printf("Start meas. failed (short circuit?)\n");
#endif
#endif
#ifdef USE_TC1047
        tcTemperature = getTC1047temperature();
        txMsg.Data.bytes[0] = tcTemperature & 0x00FF;
        txMsg.Data.bytes[1] = (tcTemperature & 0xFF00)>>8;
        txMsg.DataLength = 2;
        txMsg.Id = 0x460;

        Can_Send(&txMsg);
#endif
#ifdef USE_SHT71
#endif
		}
	}
	
	return 0;
}
