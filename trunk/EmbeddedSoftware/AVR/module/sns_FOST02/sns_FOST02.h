#ifndef SNS_FOST02
#define SNS_FOST02

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
#include <drivers/sensor/FOST02/FOST02.h>
#include <drivers/sensor/ds18s20/delay.h>

#include "protocol.h"

void sns_FOST02_Init(void);
void sns_FOST02_Process(void);
void sns_FOST02_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_FOST02_List(uint8_t ModuleSequenceNumber);

#endif
