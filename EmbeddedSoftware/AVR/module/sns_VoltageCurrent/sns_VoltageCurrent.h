#ifndef SNS_VOLTAGECURRENT
#define SNS_VOLTAGECURRENT

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

void sns_VoltageCurrent_Init(void);
void sns_VoltageCurrent_Process(void);
void sns_VoltageCurrent_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_VoltageCurrent_List(uint8_t ModuleSequenceNumber);


#ifdef sns_VoltageCurrent_USEEEPROM
	struct sns_VoltageCurrent_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#endif // SNS_VOLTAGECURRENT
