#ifndef SNS_PIR
#define SNS_PIR

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
#include <drivers/adc/ltc2450.h>
#include <drivers/adc/adc.h>

//to use PCINT lib. uncomment the line below
//#include <drivers/mcu/pcint.h>

void sns_PIR_Init(void);
void sns_PIR_Process(void);
void sns_PIR_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_PIR_List(uint8_t ModuleSequenceNumber);


#ifndef sns_PIR_USEEEPROM
#define sns_PIR_USEEEPROM 0
#endif

#if sns_PIR_USEEEPROM==1
	struct sns_PIR_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#endif // SNS_PIR
