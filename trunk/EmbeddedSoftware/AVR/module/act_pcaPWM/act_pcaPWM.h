#ifndef ACT_PCAPWM
#define ACT_PCAPWM

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

void act_pcaPWM_Init(void);
void act_pcaPWM_Process(void);
void act_pcaPWM_HandleMessage(StdCan_Msg_t *rxMsg);
void act_pcaPWM_List(uint8_t ModuleSequenceNumber);


#ifdef act_pcaPWM_USEEEPROM
	struct act_pcaPWM_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#endif // ACT_PCAPWM
