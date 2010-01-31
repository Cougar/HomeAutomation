#ifndef ACT_RGBW
#define ACT_RGBW

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

//to use PCINT lib. uncomment the line below
//#include <drivers/mcu/pcint.h>

void act_RGBW_Init(void);
void act_RGBW_Process(void);
void act_RGBW_HandleMessage(StdCan_Msg_t *rxMsg);
void act_RGBW_List(uint8_t ModuleSequenceNumber);

#define ACT_RGBW_DEMO_STATE_NOT_RUNNING	0
#define ACT_RGBW_DEMO_STATE_DECREASE		1
#define ACT_RGBW_DEMO_STATE_INCREASE		2
#define ACT_RGBW_DEMO_STATE_GOBACK		3

#define ACT_RGBW_MAX_DIM				10000UL
#define ACT_RGBW_MIN_DIM				0

#ifdef act_RGBW_USEEEPROM
	struct act_RGBW_Data{
		///TODO: Define EEPROM variables needed by the module
		uint16_t ch1;
		uint16_t ch2;
		uint16_t ch3;
		uint16_t ch4;
	};	
#endif

#endif // ACT_RGBW
