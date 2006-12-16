/**
 * DS18S20 over CAN program.
 * 
 * @target	AVR
 * @date	2006-12-09
 * @author	Jimmy Myhrman, Anders Runeson
 *   
 * Högst experimentell, decimaldel fel vid negativ temperatur? 
 * negativ temperatur helt otestad!
 * 
 */

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
#include <uart.h>
#include <timebase.h>

#include <onewire.h>
#include <ds18x20.h>

#include <delay.h>



#define MAXSENSORS 4

uint8_t gSensorIDs[MAXSENSORS][OW_ROMCODE_SIZE];

uint8_t search_sensors(void)
{
	uint8_t i;
	uint8_t id[OW_ROMCODE_SIZE];
	uint8_t diff, nSensors;
	
	printf( "\nScanning Bus for DS18X20\n" );
	
	nSensors = 0;
	
	for( diff = OW_SEARCH_FIRST; 
		diff != OW_LAST_DEVICE && nSensors < MAXSENSORS ; )
	{
		DS18X20_find_sensor( &diff, &id[0] );
		
		if( diff == OW_PRESENCE_ERR ) {
			printf( "No Sensor found\n" );
			break;
		}
		
		if( diff == OW_DATA_ERR ) {
			printf( "Bus Error\n" );
			break;
		}
		
		for (i=0;i<OW_ROMCODE_SIZE;i++)
			gSensorIDs[nSensors][i]=id[i];
		
		nSensors++;
	}
	
	return nSensors;
}



/*-----------------------------------------------------------------------------
 * Main Program
 *---------------------------------------------------------------------------*/
int main(void) {
	uint8_t nSensors, i;
	uint8_t subzero, cel, cel_frac_bits;
	
	Timebase_Init();
	Serial_Init();
	sei();
	
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
	
	uint32_t timeStamp = 0;
	
	Can_Message_t txMsg;
	Can_Message_t rxMsg;
	
	txMsg.DataLength = 2;
	txMsg.Id = 0;
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
			
			if ( DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL ) == DS18X20_OK) {
				printf("Measuring temperature... ");
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
					else printf("CRC Error (lost connection?)\n");
				}
				
				txMsg.DataLength = nSensors*2;
				printf("sending...\n");
				/* send txMsg */
				Can_Send(&txMsg);
			}
			else printf("Start meas. failed (short circuit?)\n");

		}
	}
	
	return 0;
}
