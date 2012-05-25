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

#include <drivers/mcu/pcint.h>

void sns_power_Init(void);
void sns_power_Process(void);
void sns_power_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_power_List(uint8_t ModuleSequenceNumber);


#ifndef sns_power_USEEEPROM
#define sns_power_USEEEPROM 0
#endif

#ifndef sns_power_PIN_PULLUP
#define sns_power_PIN_PULLUP 1
#endif

#ifndef sns_power_PIN_PULLUP_ch2
#define sns_power_PIN_PULLUP_ch2 1
#endif

#if sns_power_USEEEPROM==1
	struct sns_power_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t reportInterval;
		uint16_t EnergyCounterUpper;
		uint16_t EnergyCounterLower;
#ifdef POWER_SNS_PIN_ch2
		uint16_t EnergyCounterUpper_ch2;
		uint16_t EnergyCounterLower_ch2;
#endif
	};	
#endif

#endif // SNS_POWER
