#ifndef SNS_ROTARYHEX
#define SNS_ROTARYHEX

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
#include <drivers/mcu/pcint.h>

void sns_rotaryHex_Init(void);
void sns_rotaryHex_Process(void);
void sns_rotaryHex_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_rotaryHex_List(uint8_t ModuleSequenceNumber);


#ifdef sns_rotaryHex_USEEEPROM
	struct sns_rotaryHex_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#endif // SNS_ROTARYHEX
