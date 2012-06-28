/**
 * Configuration file for the CAN.C software module.
 */

#ifndef MCP2515_CFG_H_
#define MCP2515_CFG_H_

#include <config.h>
/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/

/**
 * MCP2515 Chip Select port on avr. 
 */

/* Should already be defined in node configuration bios.inc */
#ifndef MCP_CS_PORT
#define MCP_CS_PORT	PORTB
#endif
#ifndef MCP_CS_DDR
#define MCP_CS_DDR	DDRB
#endif
#ifndef MCP_CS_BIT
#define MCP_CS_BIT	PB2
#endif

/* Choose prescaler for clock output */
#ifdef MCP_USART_SPI_MODE
	#if MCP_CLOCK_PRESC_USART == 8
		#define MCP_PRESCALER CLKOUT_PS8
	#elif MCP_CLOCK_PRESC_USART == 4
		#define MCP_PRESCALER CLKOUT_PS4
	#elif MCP_CLOCK_PRESC_USART == 2
		#define MCP_PRESCALER CLKOUT_PS2
	#else
		#define MCP_PRESCALER CLKOUT_PS1
	#endif
#else
	#if MCP_CLOCK_PRESC == 8
		#define MCP_PRESCALER CLKOUT_PS8
	#elif MCP_CLOCK_PRESC == 4
		#define MCP_PRESCALER CLKOUT_PS4
	#elif MCP_CLOCK_PRESC == 2
		#define MCP_PRESCALER CLKOUT_PS2
	#else
		#define MCP_PRESCALER CLKOUT_PS1
	#endif
#endif

#if defined(__AVR_ATmega8__)
	#define INT1_REG GICR
	#define DDRSS DDRB
	#define SS PB2
#elif defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328P__)
	#define INT1_REG EIMSK
	#define DDRSS DDRB
	#define SS PB2
#else
#error AVR device not supported!
#endif

#ifdef MCP_USART_SPI_MODE
	#define MCP_INT_VECTOR	INT0_vect	
	#define MCP_INT_ENABLE()	INT1_REG |= _BV(INT0)
	#define MCP_INT_DISABLE()	INT1_REG &= ~(_BV(INT0))
#else
	#define MCP_INT_VECTOR	INT1_vect	
	#define MCP_INT_ENABLE()	INT1_REG |= _BV(INT1)
	#define MCP_INT_DISABLE()	INT1_REG &= ~(_BV(INT1))
#endif

#endif /*MCP2515_CFG_H_*/
