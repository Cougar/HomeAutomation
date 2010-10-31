#ifndef SNS_NTC
#define SNS_NTC

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

void sns_NTC_Init(void);
void sns_NTC_Process(void);
void sns_NTC_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_NTC_List(uint8_t ModuleSequenceNumber);

#endif
