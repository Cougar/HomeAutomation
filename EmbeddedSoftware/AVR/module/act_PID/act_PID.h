#ifndef ACT_PID
#define ACT_PID

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

void act_PID_Init(void);
void act_PID_Process(void);
void act_PID_HandleMessage(StdCan_Msg_t *rxMsg);
void act_PID_List(uint8_t ModuleSequenceNumber);
#ifdef act_PID_USEEEPROM
	struct act_PID_Data{
		///TODO: Define EEPROM variables needed by the module
		uint16_t referenceValue;
		uint8_t sensorModuleType;
		uint8_t sensorModuleId;
		uint8_t sensorId;
		uint16_t K_P;
		uint16_t K_I;
		uint16_t K_D;
		uint8_t TimeMsOrS;
		uint16_t Time;
		uint8_t actuatorModuleType;
		uint8_t actuatorModuleId;
		uint8_t actuatorId;
		uint8_t sendPeriod;
	};
#endif

#endif // ACT_PID
