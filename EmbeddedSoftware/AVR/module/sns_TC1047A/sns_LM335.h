#ifndef SNS_LM335
#define SNS_LM335

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

void sns_LM335_Init(void);
void sns_LM335_Process(void);
void sns_LM335_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_LM335_List(uint8_t ModuleSequenceNumber);

#endif
