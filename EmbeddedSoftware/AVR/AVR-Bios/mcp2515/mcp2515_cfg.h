/**
 * Configuration file for the CAN.C software module.
 */

#ifndef MCP2515_CFG_H_
#define MCP2515_CFG_H_

#include "../avr_cfg.h"
/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/

/**
 * MCP2515 clock frequency. You can use crystals values from 8MHz to 24MHz.
 */ 
#define MCP_CLOCK_FREQ_MHZ		16

/**
 * MCP2515 CAN bitrate. You can choose between 125, 250, 500 and 1000 kbps.
 */
#define MCP_CAN_BITRATE_KBPS	1000 

/**
 * MCP2515 Chip Select port on avr. 
 */ 
#define MCP_CS_PORT	PORTB
#define MCP_CS_DDR	DDRB
#define MCP_CS_BIT	PB2

#define MCP_INT_VECTOR	INT1_vect	
#define MCP_INT_ENABLE()	INT1_REG |= _BV(INT1)
#define MCP_INT_DISABLE()	INT1_REG &= ~(_BV(INT1))

#endif /*MCP2515_CFG_H_*/
