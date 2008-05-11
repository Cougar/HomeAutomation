#ifndef SNS_DS18X20
#define SNS_DS18X20

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

#include <drivers/sensor/ds18s20/ds18x20.h>
#include <drivers/sensor/ds18s20/onewire.h>
#include <drivers/sensor/ds18s20/delay.h>


void sns_ds18x20_Init(void);
void sns_ds18x20_Process(void);
void sns_ds18x20_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_ds18x20_List(uint8_t ModuleSequenceNumber);
#endif
