#ifndef TEMPLATE
#define TEMPLATE

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

#include "protocol.h"

void TEMPLATE_Init(void);
void TEMPLATE_Process(void);
void TEMPLATE_HandleMessage(StdCan_Msg_t *rxMsg);
void TEMPLATE_List(uint8_t ModuleSequenceNumber);

#endif
