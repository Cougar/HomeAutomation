#ifndef SNS_COUNTER
#define SNS_COUNTER

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

void sns_counter_Init(void);
void sns_counter_Process(void);
void sns_counter_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_counter_List(uint8_t ModuleSequenceNumber);


#ifndef sns_counter_USEEEPROM
#define sns_counter_USEEEPROM 0
#endif

#if sns_counter_USEEEPROM==1
	struct sns_counter_Data{
		uint8_t reportInterval;
		uint32_t Count0;
	};
#endif

#endif // SNS_COUNTER
