#ifndef ACT_SOFTPWM
#define ACT_SOFTPWM

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

//to use PCINT lib. uncomment the line below
//#include <drivers/mcu/pcint.h>

void act_softPWM_Init(void);
void act_softPWM_Process(void);
void act_softPWM_HandleMessage(StdCan_Msg_t *rxMsg);
void act_softPWM_List(uint8_t ModuleSequenceNumber);

#ifndef act_softPWM_USEEEPROM
#define act_softPWM_USEEEPROM 0
#endif

#if act_softPWM_USEEEPROM==1
	struct act_softPWM_Data{
		///TODO: Define EEPROM variables needed by the module
		uint16_t maxTimer;
		uint8_t resolution;
		uint8_t ReportInterval;
	};	
#endif

#endif // ACT_SOFTPWM
