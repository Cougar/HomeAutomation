#ifndef ACT_HD44780
#define ACT_HD44780

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

#include <drivers/lcd/clcd/lcd_HD44780.h>
#include <string.h> //for memcpy

#include "protocol.h"

void act_hd44780_Init(void);
void act_hd44780_Process(void);
void act_hd44780_HandleMessage(StdCan_Msg_t *rxMsg);
void act_hd44780_List(uint8_t ModuleSequenceNumber);

#endif
