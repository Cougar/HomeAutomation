#ifndef SNS_ULTRASONIC
#define SNS_ULTRASONIC

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

void sns_ultrasonic_Init(void);
void sns_ultrasonic_Process(void);
void sns_ultrasonic_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_ultrasonic_List(uint8_t ModuleSequenceNumber);


#ifndef sns_ultrasonic_USEEEPROM
#define sns_ultrasonic_USEEEPROM 0
#endif

#if sns_ultrasonic_USEEEPROM==1
	struct sns_ultrasonic_Data{
		uint8_t reportInterval;
	};	
#endif

#endif // SNS_ULTRASONIC
