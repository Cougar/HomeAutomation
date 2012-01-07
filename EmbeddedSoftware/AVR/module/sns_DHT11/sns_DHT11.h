#ifndef SNS_DHT11
#define SNS_DHT11

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
#include <drivers/sensor/DHT11/DHT11.h>

void sns_DHT11_Init(void);
void sns_DHT11_Process(void);
void sns_DHT11_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_DHT11_List(uint8_t ModuleSequenceNumber);


#ifdef sns_DHT11_USEEEPROM
	struct sns_DHT11_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#endif // SNS_DHT11
