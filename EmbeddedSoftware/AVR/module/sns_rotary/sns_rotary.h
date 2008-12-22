#ifndef SNS_ROTARY
#define SNS_ROTARY

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

void sns_rotary_Init(void);
void sns_rotary_Process(void);
void sns_rotary_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_rotary_List(uint8_t ModuleSequenceNumber);

#endif // SNS_ROTARY
