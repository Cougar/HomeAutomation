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

/* Already defined in node configuration bios.inc */
#ifndef MCP_CS_PORT
#define MCP_CS_PORT	PORTB
#endif
#ifndef MCP_CS_DDR
#define MCP_CS_DDR	DDRB
#ifndef MCP_CS_BIT
#define MCP_CS_BIT	PB2
#endif

#if defined(__AVR_ATmega8__)
#define INT1_REG GICR
#elif defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__)
#define INT1_REG EIMSK
#else
#error AVR device not supported!
#endif

#define MCP_INT_VECTOR	INT1_vect	
#define MCP_INT_ENABLE()	INT1_REG |= _BV(INT1)
#define MCP_INT_DISABLE()	INT1_REG &= ~(_BV(INT1))

#endif /*MCP2515_CFG_H_*/
