#ifndef SNS_TOUCH
#define SNS_TOUCH

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
#include <drivers/touch/gesture/gesture.h>

//to use PCINT lib. uncomment the line below
//#include <drivers/mcu/pcint.h>

void sns_Touch_Init(void);
void sns_Touch_Process(void);
void sns_Touch_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_Touch_List(uint8_t ModuleSequenceNumber);

#ifndef sns_Touch_USEEEPROM
#define sns_Touch_USEEEPROM 0
#endif

#if sns_Touch_USEEEPROM==1
	struct sns_Touch_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#endif // SNS_TOUCH
