#ifndef ACT_PROTECTEDOUTPUT
#define ACT_PROTECTEDOUTPUT

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


/*-----------------------------------------------------------------------------
 * Exported Functions
 *---------------------------------------------------------------------------*/

void act_protectedOutput_Init(void);
void act_protectedOutput_Process(void);
void act_protectedOutput_HandleMessage(StdCan_Msg_t *rxMsg);
void act_protectedOutput_List(uint8_t ModuleSequenceNumber);


#endif // ACT_PROTECTEDOUTPUT
