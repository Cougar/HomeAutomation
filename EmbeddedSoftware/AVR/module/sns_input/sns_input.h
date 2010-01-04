#ifndef SNS_INPUT
#define SNS_INPUT

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

//to use PCINT lib. uncomment the line below
#include <drivers/mcu/pcint.h>

void sns_input_Init(void);
void sns_input_Process(void);
void sns_input_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_input_List(uint8_t ModuleSequenceNumber);


#endif // SNS_INPUT
