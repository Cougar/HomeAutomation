#ifndef SNS_TOUCH
#define SNS_TOUCH

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

void sns_Touch_Init(void);
void sns_Touch_Process(void);
void sns_Touch_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_Touch_List(uint8_t ModuleSequenceNumber);

typedef struct
{
	uint8_t x;
	uint8_t y;
} point;

typedef struct
{
	uint8_t f1;
	uint8_t f2;
	uint8_t f3;
	uint8_t f4;
	uint8_t f5;
	uint8_t f6;
	uint8_t f7;
	uint8_t f8;
	uint8_t f9;
} gesture;

static __inline__ uint8_t min(uint8_t val1, uint8_t val2) {
  if (val1 > val2)
  {
  	return val2;
  }
  return val1;
}

static __inline__ uint8_t max(uint8_t val1, uint8_t val2) {
  if (val1 > val2)
  {
  	return val1;
  }
  return val2;
}

#ifdef sns_Touch_USEEEPROM
	struct sns_Touch_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#endif // SNS_TOUCH
