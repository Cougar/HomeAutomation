#ifndef ACT_DIMMER230
#define ACT_DIMMER230

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
/* lib files */
#include <config.h>
#include <bios.h>
#include <drivers/can/stdcan.h>
#include <drivers/timer/timer.h>
#include <drivers/mcu/gpio.h>

#define ACT_DIMMMER230_STATE_IDLE			0
#define ACT_DIMMMER230_STATE_TIMER_ON		1
#define ACT_DIMMMER230_STATE_xyz			2

#define ACT_DIMMMER230_DEMO_STATE_NOT_RUNNING	0
#define ACT_DIMMMER230_DEMO_STATE_DECREASE		1
#define ACT_DIMMMER230_DEMO_STATE_INCREASE		2
#define ACT_DIMMMER230_DEMO_STATE_GOBACK		3

#define ACT_DIMMMER230_PERIOD_TIME			10000

#define ACT_DIMMMER230_MAX_DIM				255
#define ACT_DIMMMER230_MIN_DIM				0

#define ACT_DIMMMER230_50HZ_PERIOD_TIME		10000
#define ACT_DIMMMER230_60HZ_PERIOD_TIME		8000

#if act_dimmer230_ZC_PCINT<8
#define act_dimmer230_ZC_PCINT_vect	PCINT0_vect		//interrupt vector
#define act_dimmer230_ZC_PCIE		PCIE0			//interrupt enable
#define act_dimmer230_ZC_PCIF		PCIF0			//interrupt flag
#define act_dimmer230_ZC_PCMSK		PCMSK0			//mask register
#define act_dimmer230_ZC_PCINT_BIT	act_dimmer230_ZC_PCINT		//interrupt bit
#endif

#if act_dimmer230_ZC_PCINT>=8 && act_dimmer230_ZC_PCINT<16
#define act_dimmer230_ZC_PCINT_vect	PCINT1_vect		//interrupt vector
#define act_dimmer230_ZC_PCIE		PCIE1			//interrupt enable
#define act_dimmer230_ZC_PCIF		PCIF1			//interrupt flag
#define act_dimmer230_ZC_PCMSK		PCMSK1			//mask register
#define act_dimmer230_ZC_PCINT_BIT	act_dimmer230_ZC_PCINT-8	//interrupt bit
#endif

#if act_dimmer230_ZC_PCINT>=16 && act_dimmer230_ZC_PCINT<24
#define act_dimmer230_ZC_PCINT_vect	PCINT2_vect		//interrupt vector
#define act_dimmer230_ZC_PCIE		PCIE2			//interrupt enable
#define act_dimmer230_ZC_PCIF		PCIF2			//interrupt flag
#define act_dimmer230_ZC_PCMSK		PCMSK2			//mask register
#define act_dimmer230_ZC_PCINT_BIT	act_dimmer230_ZC_PCINT-16	//interrupt bit
#endif

void act_dimmer230_Init(void);
void act_dimmer230_Process(void);
void act_dimmer230_HandleMessage(StdCan_Msg_t *rxMsg);
void act_dimmer230_List(uint8_t ModuleSequenceNumber);

#endif // ACT_DIMMER230
