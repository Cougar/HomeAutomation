#ifndef SNS_QTOUCH
#define SNS_QTOUCH

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

void sns_qtouch_Init(void);
void sns_qtouch_Process(void);
void sns_qtouch_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_qtouch_List(uint8_t ModuleSequenceNumber);


#ifdef sns_qtouch_USEEEPROM
	struct sns_qtouch_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#endif // SNS_QTOUCH
