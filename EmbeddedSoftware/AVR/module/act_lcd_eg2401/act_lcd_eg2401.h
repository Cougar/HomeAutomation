#ifndef ACT_LCD_EG2401
#define ACT_LCD_EG2401

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
//#include <drivers/mcu/pcint.h>
#include <avr/pgmspace.h>

void act_lcd_eg2401_Init(void);
void act_lcd_eg2401_Process(void);
void act_lcd_eg2401_HandleMessage(StdCan_Msg_t *rxMsg);
void act_lcd_eg2401_List(uint8_t ModuleSequenceNumber);

extern uint8_t lcd_framebuffer[256];


#ifdef act_lcd_eg2401_USEEEPROM
	struct act_lcd_eg2401_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#endif // ACT_LCD_EG2401
