#ifndef ACT_SIGMADELTA_DIMMER
#define ACT_SIGMADELTA_DIMMER

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

void act_sigmadelta_dimmer_Init(void);
void act_sigmadelta_dimmer_Process(void);
void act_sigmadelta_dimmer_HandleMessage(StdCan_Msg_t *rxMsg);
void act_sigmadelta_dimmer_List(uint8_t ModuleSequenceNumber);


#endif // ACT_SIGMADELTA_DIMMER
