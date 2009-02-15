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

void sns_TC1047A_Init(void);
void sns_TC1047A_Process(void);
void sns_TC1047A_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_TC1047A_List(uint8_t ModuleSequenceNumber);

#endif
