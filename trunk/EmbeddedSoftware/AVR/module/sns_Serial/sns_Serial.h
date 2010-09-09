#ifndef SNS_SERIAL
#define SNS_SERIAL

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
#include <drivers/uart/uart.h>

//to use PCINT lib. uncomment the line below
//#include <drivers/mcu/pcint.h>

void sns_Serial_Init(void);
void sns_Serial_Process(void);
void sns_Serial_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_Serial_List(uint8_t ModuleSequenceNumber);


#ifdef sns_Serial_USEEEPROM
	struct sns_Serial_Data{
		uint32_t baudRate;
		uint16_t format;
	};	
#endif

#endif // SNS_SERIAL
