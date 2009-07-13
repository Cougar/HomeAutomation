#ifndef TST_CORELED
#define TST_CORELED

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
//#include <drivers/mcu/pcint.h>

void tst_CoreLED_Init(void);
void tst_CoreLED_Process(void);
void tst_CoreLED_HandleMessage(StdCan_Msg_t *rxMsg);
void tst_CoreLED_List(uint8_t ModuleSequenceNumber);

#endif // TST_CORELED
