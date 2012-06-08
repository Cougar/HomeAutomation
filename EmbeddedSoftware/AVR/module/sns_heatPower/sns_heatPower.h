#ifndef SNS_HEATPOWER
#define SNS_HEATPOWER

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

void sns_heatPower_Init(void);
void sns_heatPower_Process(void);
void sns_heatPower_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_heatPower_List(uint8_t ModuleSequenceNumber);


#ifndef sns_heatPower_USEEEPROM
#define sns_heatPower_USEEEPROM 0
#endif

#if sns_heatPower_USEEEPROM==1
	struct sns_heatPower_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#endif // SNS_HEATPOWER
