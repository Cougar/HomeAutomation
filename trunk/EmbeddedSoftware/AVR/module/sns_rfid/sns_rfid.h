#ifndef SNS_RFID
#define SNS_RFID

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
#include <drivers/mcu/gpio.h>
#include <drivers/adc/adc.h>
#include <drivers/mcu/pcint.h>

struct sns_rfid_card_t {
	uint32_t id;
	uint8_t version;
} sns_rfid_card;

void sns_rfid_Init(void);
void sns_rfid_Process(void);
void sns_rfid_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_rfid_List(uint8_t ModuleSequenceNumber);

void sns_rfid_HandleCardEvent( uint8_t event );

#endif // SNS_RFID
