#ifndef SNS_IRTRANSCEIVE
#define SNS_IRTRANSCEIVE

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

/* delay.h must be declared after F_CPU */
#include <util/delay.h>

#include <drivers/mcu/gpio.h>
#include <drivers/ir/transceiver/irtransceiverMulti.h>
#include <drivers/ir/protocols.h>

void sns_irTransceive_Init(void);
void sns_irTransceive_Process(void);
void sns_irTransceive_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_irTransceive_List(uint8_t ModuleSequenceNumber);

#define sns_irTransceive_STATE_IDLE				0
#define sns_irTransceive_STATE_IR_REPEAT		1
#define sns_irTransceive_STATE_START_RECEIVE	2
#define sns_irTransceive_STATE_RECEIVING		3
#define sns_irTransceive_STATE_START_PAUSE		4
#define sns_irTransceive_STATE_PAUSING			5
#define sns_irTransceive_STATE_START_IDLE		6

#define sns_irTransceive_STATE_START_TRANSMIT	7
#define sns_irTransceive_STATE_TRANSMITTING		8

#define sns_irTransceive_STATE_DISABLED			0xff


#define TRUE 1
#define FALSE 0

#endif // SNS_IRTRANSCEIVE
