/**
 * Configuration file for the CAN.C software module.
 */

#ifndef MCP2515_CFG_H_
#define MCP2515_CFG_H_

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/

/**
 * MCP2515 clock frequency. You can use crystals values from 8MHz to 24MHz.
 */ 
#define MCP_CLOCK_FREQ_MHZ		24


#define MCP_CS_PORT PORTB
#define MCP_CS_DDR  DDRB
#define MCP_CS_BIT  PB2


/**
 * Define this to enable MCP2515 debug mode. A lot of debug info will be
 * printed to stdout. Turn off for silent mode.
 */
//#define MCP_DEBUGMODE


#endif /*MCP2515_CFG_H_*/
