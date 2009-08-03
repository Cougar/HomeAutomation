#ifndef SNS_IRTRANSMIT
#define SNS_IRTRANSMIT

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

#include <drivers/ir/transceiver/irtransceiver.h>
#include <drivers/ir/protocols.h>

void sns_irTransmit_Init(void);
void sns_irTransmit_Process(void);
void sns_irTransmit_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_irTransmit_List(uint8_t ModuleSequenceNumber);

#define sns_irTransmit_STATE_IDLE		0
#define sns_irTransmit_STATE_START_TRANSMIT	1
#define sns_irTransmit_STATE_TRANSMITTING	2
#define sns_irTransmit_STATE_START_PAUSE	3
#define sns_irTransmit_STATE_PAUSING		4
#define sns_irTransmit_STATE_STOP		5

#endif // SNS_IRTRANSMIT
