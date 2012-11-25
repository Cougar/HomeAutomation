#ifndef SNS_FLOWER
#define SNS_FLOWER

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

void sns_flower_Init(void);
void sns_flower_Process(void);
void sns_flower_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_flower_List(uint8_t ModuleSequenceNumber);


#ifndef sns_flower_USEEEPROM
#define sns_flower_USEEEPROM 0
#endif

#if sns_flower_USEEEPROM==1
	struct sns_flower_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#endif // SNS_FLOWER
