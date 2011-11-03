#ifndef ACT_DOTMATRIX
#define ACT_DOTMATRIX

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

void act_DotMatrix_Init(void);
void act_DotMatrix_Process(void);
void act_DotMatrix_HandleMessage(StdCan_Msg_t *rxMsg);
void act_DotMatrix_List(uint8_t ModuleSequenceNumber);

#define USART_SPI_CS_DDR	DDRC
#define USART_SPI_CS		PC0
#define USART_SPI_XCK_DDR	DDRD
#define USART_SPI_XCK		PD4
#define waitspi() while(!(UCSR0A&(1<<RXC0)))

#define act_DotMatrix_PWM_FACT (256UL*0xff)/10000UL


#ifdef act_DotMatrix_USEEEPROM
	struct act_DotMatrix_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#endif // ACT_DOTMATRIX
