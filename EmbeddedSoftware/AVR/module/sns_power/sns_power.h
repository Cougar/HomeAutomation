#ifndef SNS_POWER
#define SNS_POWER

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

void sns_power_Init(void);
void sns_power_Process(void);
void sns_power_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_power_List(uint8_t ModuleSequenceNumber);


#ifdef sns_power_USEEEPROM
	struct sns_power_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t reportInterval;
	};	
#endif

#endif // SNS_POWER
