#ifndef SNS_WATER
#define SNS_WATER

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
#include <drivers/mcu/gpio.h>
#include <drivers/adc/adc.h>
#include <drivers/mcu/pcint.h>

void sns_water_Init(void);
void sns_water_Process(void);
void sns_water_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_water_List(uint8_t ModuleSequenceNumber);


#ifndef sns_water_USEEEPROM
#define sns_water_USEEEPROM 0
#endif

#define HIGH		1
#define LOW			2
//#define NOCHANGE 	0
//#define HIGH_NOCH	3
//#define LOW_NOCH	4

#if sns_water_USEEEPROM==1
	struct sns_water_Data{
		uint8_t reportInterval;
		uint16_t VolumeCounterUpper;
		uint16_t VolumeCounterLower;
	};	
#endif

#endif // SNS_WATER
