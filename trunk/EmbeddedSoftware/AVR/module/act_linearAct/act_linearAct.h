/* 
	linearAct started by dberg 2011.
	
	This module is written to control an old linear actuator from Landis & Gyr, but should work
	with a broad range of linear actuators and other stuff that conforms to its simple protocol:
	
	Movement is controlled by two signals, typically controlling two relays. Feedback of 
	position is sensed by pulses connected to the hardware counter.
	
	The two control signals are ON/OFF and DIRECTION - one relay to control actuator engine on/off
	and the other one to change the polarity.
	
	The position is always relative and must be calibrated manually. Use CALIBRATION command to
	give the module full moving space.
	
	Posible features to add:
	- automatic recalibration by external switch (one or perhaps two)
	- PWM speedcontrol
*/

#ifndef ACT_LINEARACT
#define ACT_LINEARACT

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

#include <util/delay.h>

void act_linearAct_Init(void);
void act_linearAct_Process(void);
void act_linearAct_HandleMessage(StdCan_Msg_t *rxMsg);
void act_linearAct_List(uint8_t ModuleSequenceNumber);


#ifdef act_linearAct_USEEEPROM
	struct act_linearAct_Data{
		///TODO: Define EEPROM variables needed by the module
		uint16_t pulses_ee;
		uint16_t min_ee;
		uint16_t low_ee;
		uint16_t high_ee;
		uint16_t max_ee;
	};	
#endif

#endif // ACT_LINEARACT
