#ifndef ACT_RGBDRIVER
#define ACT_RGBDRIVER

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
#include <drivers/adc/adc.h>

#include "protocol.h"

void act_RGBdriver_Init(void);
void act_RGBdriver_Process(void);
void act_RGBdriver_HandleMessage(StdCan_Msg_t *rxMsg);
void act_RGBdriver_List(uint8_t ModuleSequenceNumber);

#endif // ACT_RGBDRIVER
