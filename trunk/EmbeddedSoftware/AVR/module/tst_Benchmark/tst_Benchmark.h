#ifndef TST_BENCHMARK
#define TST_BENCHMARK

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

void tst_Benchmark_Init(void);
void tst_Benchmark_Process(void);
void tst_Benchmark_HandleMessage(StdCan_Msg_t *rxMsg);
void tst_Benchmark_List(uint8_t ModuleSequenceNumber);

/* LIST ALL TESTS HERE AND DEACTIVATE BY DEFAULT */

#ifndef tst_Benchmark_CanSend_TestActive
#define tst_Benchmark_CanSend_TestActive 0
#endif

#ifndef tst_Benchmark_CpuTime_TestActive
#define tst_Benchmark_CpuTime_TestActive 0
#endif


#ifndef tst_Benchmark_USEEEPROM
#define tst_Benchmark_USEEEPROM 0
#endif

#if tst_Benchmark_USEEEPROM==1
	struct tst_Benchmark_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif


#endif // TST_BENCHMARK
