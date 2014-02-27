#ifndef ACT_PIDV1
#define ACT_PIDV1

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

void act_PIDv1_Init(void);
void act_PIDv1_Process(void);
void act_PIDv1_HandleMessage(StdCan_Msg_t *rxMsg);
void act_PIDv1_List(uint8_t ModuleSequenceNumber);


#ifndef act_PIDv1_USEEEPROM
#define act_PIDv1_USEEEPROM 0
#endif

#if act_PIDv1_USEEEPROM==1
	struct act_PIDv1_Data{
		///TODO: Define EEPROM variables needed by the module
		float referenceValue;
		uint8_t sensorModuleType;
		uint8_t sensorModuleId;
		uint8_t sensorId;
		float K_P;
		float K_I;
		float K_D;
		uint8_t TimeMsOrS;
		uint16_t Time;
		uint8_t actuatorModuleType;
		uint8_t actuatorModuleId;
		uint8_t actuatorId;
		float MAX;
		float MIN;
	};	
#endif

#endif // ACT_PIDV1
