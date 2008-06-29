#ifndef SNS_SIMPLEDTMF
#define SNS_SIMPLEDTMF

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

#include <drivers/DTMF/dtmf.h>

#define SNS_SIMPLEDTMF_STATE_IDLE			0
#define SNS_SIMPLEDTMF_STATE_COPYDATA		1
#define SNS_SIMPLEDTMF_STATE_WAIT			2
#define SNS_SIMPLEDTMF_STATE_SEND			3

#include "protocol.h"

void sns_SimpleDTMF_Init(void);
void sns_SimpleDTMF_Process(void);
void sns_SimpleDTMF_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_SimpleDTMF_List(uint8_t ModuleSequenceNumber);

#endif // SNS_SIMPLEDTMF
