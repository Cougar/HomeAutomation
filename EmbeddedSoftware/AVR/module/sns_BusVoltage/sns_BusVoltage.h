#ifndef SNS_BUSVOLTAGE
#define SNS_BUSVOLTAGE

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

void sns_BusVoltage_Init(void);
void sns_BusVoltage_Process(void);
void sns_BusVoltage_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_BusVoltage_List(uint8_t ModuleSequenceNumber);

#endif
