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

#include <drivers/DTMF/mt8870/mt8870.h>

#define SNS_SIMPLEDTMF_STATE_IDLE			0
#define SNS_SIMPLEDTMF_STATE_GET_DATA		1
#define SNS_SIMPLEDTMF_STATE_START_WAIT		2
#define SNS_SIMPLEDTMF_STATE_WAIT			3
#define SNS_SIMPLEDTMF_STATE_STOP			4

#include "protocol.h"

void sns_SimpleDTMF_Init(void);
void sns_SimpleDTMF_Process(void);
void sns_SimpleDTMF_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_SimpleDTMF_List(uint8_t ModuleSequenceNumber);

#endif // SNS_SIMPLEDTMF
