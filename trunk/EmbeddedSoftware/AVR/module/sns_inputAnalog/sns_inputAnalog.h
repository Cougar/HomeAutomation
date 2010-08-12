#ifndef SNS_INPUTANALOG
#define SNS_INPUTANALOG

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
#include <drivers/io/PCA95xx.h>

//to use PCINT lib. uncomment the line below
//#include <drivers/mcu/pcint.h>

void sns_inputAnalog_Init(void);
void sns_inputAnalog_Process(void);
void sns_inputAnalog_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_inputAnalog_List(uint8_t ModuleSequenceNumber);


#ifdef sns_inputAnalog_USEEEPROM
	struct sns_inputAnalog_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#define TRUE 1
#define FALSE 0

#endif // SNS_INPUTANALOG
