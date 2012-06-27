#ifndef SNS_NMEA
#define SNS_NMEA

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
/* lib files */
#include <config.h>
#include <bios.h>
#include <drivers/can/stdcan.h>
#include <drivers/timer/timer.h>

#ifndef TRIMBLE_LASSEN_LP
#define TRIMBLE_LASSEN_LP 0
#endif

#if TRIMBLE_LASSEN_LP == 1
#define LAT_LENGTH 9
#define LON_LENGTH 10
#define TSIP_AT_STARTUP
#else // Default
#define LAT_LENGTH 10
#define LON_LENGTH 11
#endif

void sns_nmea_Init(void);
void sns_nmea_Process(void);
void sns_nmea_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_nmea_List(uint8_t ModuleSequenceNumber);

struct {
	uint8_t time_h, time_m, time_s;
	uint8_t lat_deg, lon_deg; //Följer inte auml:s enhetsstandard
	uint8_t lat_min, lon_min;
	uint8_t lat_sec_h, lat_sec_l, lon_sec_h, lon_sec_l; //Last bit contains cardinal direction
	
	uint8_t fixtype; //1=no fix, 2=2D, 3=3D
	uint8_t fixvalid; //From GGA, 0 or 1
	uint8_t usedsat;

	uint8_t speed;
	uint8_t cmg_h, cmg_l;
	uint8_t date_y, date_m, date_d;

    uint32_t unixtimestamp;
} NMEA_data;

#endif // SNS_NMEA
