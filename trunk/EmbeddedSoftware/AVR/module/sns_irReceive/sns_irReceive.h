#ifndef SNS_IRRECEIVE
#define SNS_IRRECEIVE

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

void sns_irReceive_Init(void);
void sns_irReceive_Process(void);
void sns_irReceive_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_irReceive_List(uint8_t ModuleSequenceNumber);

#define sns_irReceive_STATE_IDLE			0
#define sns_irReceive_STATE_IR_REPEAT		1
#define sns_irReceive_STATE_START_RECEIVE	2
#define sns_irReceive_STATE_RECEIVING		3
#define sns_irReceive_STATE_START_PAUSE		4
#define sns_irReceive_STATE_PAUSING			5
#define sns_irReceive_STATE_START_IDLE		6


#endif // SNS_IRRECEIVE
