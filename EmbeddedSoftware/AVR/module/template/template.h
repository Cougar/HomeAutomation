#ifndef <TEMPLATE>
#define <TEMPLATE>

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

void <template>_Init(void);
void <template>_Process(void);
void <template>_HandleMessage(StdCan_Msg_t *rxMsg);
void <template>_List(uint8_t ModuleSequenceNumber);

#endif // <TEMPLATE>
