#ifndef ACT_SLOWPWM
#define ACT_SLOWPWM

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

void act_SlowPWM_Init(void);
void act_SlowPWM_Process(void);
void act_SlowPWM_HandleMessage(StdCan_Msg_t *rxMsg);
void act_SlowPWM_List(uint8_t ModuleSequenceNumber);

#define PWMSENSORID	0

#ifdef act_SlowPWM_USEEEPROM
	struct act_SlowPWM_Data{
		///TODO: Define EEPROM variables needed by the module
		uint16_t PwmPeriod;
		uint16_t defaultPwmValue;
		uint8_t defaultStates;
		uint8_t ReportInterval;
	};	
#endif

#endif // ACT_SLOWPWM
