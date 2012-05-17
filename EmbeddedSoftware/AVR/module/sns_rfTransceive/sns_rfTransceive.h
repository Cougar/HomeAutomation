#ifndef SNS_RFTRANSCEIVE
#define SNS_RFTRANSCEIVE

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

void sns_rfTransceive_Init(void);
void sns_rfTransceive_Process(void);
void sns_rfTransceive_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_rfTransceive_List(uint8_t ModuleSequenceNumber);




#define sns_rfTransceive_STATE_IDLE						(0)
#define sns_rfTransceive_STATE_IR_REPEAT				(1)
#define sns_rfTransceive_STATE_START_RECEIVE			(2)
#define sns_rfTransceive_STATE_RECEIVING				(3)
#define sns_rfTransceive_STATE_START_PAUSE				(4)
#define sns_rfTransceive_STATE_PAUSING					(5)
#define sns_rfTransceive_STATE_START_IDLE				(6)

#define sns_rfTransceive_STATE_START_TRANSMIT			(7)

#define sns_rfTransceive_STATE_TRANSMITTING				(11)

#define sns_rfTransceive_STATE_DISABLED			0xff


#define TRUE 1
#define FALSE 0

#ifndef sns_rfTransceive_USEEEPROM
#define sns_rfTransceive_USEEEPROM 0
#endif

#if sns_rfTransceive_USEEEPROM==1
	struct sns_rfTransceive_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#endif // SNS_RFTRANSCEIVE


