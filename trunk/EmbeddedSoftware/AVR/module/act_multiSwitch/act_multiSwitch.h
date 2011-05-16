#ifndef ACT_MULTISWITCH
#define ACT_MULTISWITCH

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

void act_multiSwitch_Init(void);
void act_multiSwitch_Process(void);
void act_multiSwitch_HandleMessage(StdCan_Msg_t *rxMsg);
void act_multiSwitch_List(uint8_t ModuleSequenceNumber);


#ifdef act_multiSwitch_USEEEPROM
	struct act_multiSwitch_Data{
		uint8_t activeSwitch;
	};	
#endif

#endif // ACT_MULTISWITCH
