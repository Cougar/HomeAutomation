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

#define act_pcaPWM_FACT 0xffUL/10000UL

#ifndef act_pcaPWM_USEEEPROM
#define act_pcaPWM_USEEEPROM 0
#endif

#if act_pcaPWM_USEEEPROM==1
	struct act_pcaPWM_Data{
		uint16_t ch1;
		uint16_t ch2;
		uint16_t ch3;
		uint16_t ch4;
		uint16_t ch5;
		uint16_t ch6;
		uint16_t ch7;
		uint16_t ch8;
	};	
#endif

#define ACT_PCAPWM_MAX_DIM				10000UL
#define ACT_PCAPWM_MIN_DIM				0

#define ACT_PCAPWM_DEMO_STATE_NOT_RUNNING	0
#define ACT_PCAPWM_DEMO_STATE_DECREASE		1
#define ACT_PCAPWM_DEMO_STATE_INCREASE		2
#define ACT_PCAPWM_DEMO_STATE_GOBACK		3

#endif // ACT_PCAPWM
