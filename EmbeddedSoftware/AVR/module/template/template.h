#ifndef <TEMPLATE>
#define <TEMPLATE>

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

void <template>_Init(void);
void <template>_Process(void);
void <template>_HandleMessage(StdCan_Msg_t *rxMsg);
void <template>_List(uint8_t ModuleSequenceNumber);


#ifndef <template>_USEEEPROM
#define <template>_USEEEPROM 0
#endif

#if <template>_USEEEPROM==1
	struct <template>_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#endif // <TEMPLATE>
