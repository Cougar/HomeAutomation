#ifndef TST_TESTPRGRIG
#define TST_TESTPRGRIG

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


#define R51	12000
#define R50	47000
#if (5 * (R51 + R50))/(R51) > 64
	#error "ADC_FACTOR must be less then 64, change R51 and R52"
#elseif (5 * (R51 + R50))/(R51) < 32
	#define ADC_FACTOR	(5 * (R51 + R50))/(R51)
	#define ADC_SCALE	10
#else
	#define ADC_FACTOR	(5 * (R51 + R50))/(R51 / 2)
	#define ADC_SCALE	11
#endif

#define DUTCONNSTATE_IDLE 0
#define DUTCONNSTATE_QUAL1 1
#define DUTCONNSTATE_QUAL2 2
#define DUTCONNSTATE_ENABLED 3
#define DUTCONNSTATE_FAILED_CURR 254
#define DUTCONNSTATE_FAILED_VOLT 255

void tst_TestPrgRig_Init(void);
void tst_TestPrgRig_Process(void);
void tst_TestPrgRig_HandleMessage(StdCan_Msg_t *rxMsg);
void tst_TestPrgRig_List(uint8_t ModuleSequenceNumber);


#ifndef tst_TestPrgRig_USEEEPROM
#define tst_TestPrgRig_USEEEPROM 0
#endif

#if tst_TestPrgRig_USEEEPROM==1
	struct tst_TestPrgRig_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#endif // TST_TESTPRGRIG
