#ifndef CHN_EG2401_CONTROLLER
#define CHN_EG2401_CONTROLLER

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

void chn_lcd_control_Init(void);
void chn_lcd_control_Process(void);
void chn_lcd_control_HandleMessage(StdCan_Msg_t *rxMsg);
void chn_lcd_control_List(uint8_t ModuleSequenceNumber);

#endif // CHN_EG2401_CONTROLLER
