/**
 * @file	test.c
 * Very basic test program for the MCP2515 driver. This currently tests only
 * the SPI communication with the chip.
 * 
 * @author	Jimmy Myhrman (jimmy@myhrman.org)
 * @date	2005-11-03
 */


/*------------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------------*/
#include <avr/interrupt.h>
#include "mcp2510.h"
#include "uart.h"


/*------------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 * Globals
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 * Prototypes
 *----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 * Main Program
 *----------------------------------------------------------------------------*/
int main(void) {
	uartInit(12);		/* baudrate 38400 */
	sei();
	MCP_init();
	
	uartPutString("MCP2515 CAN Test\n");
	
	/* request configuration mode */
	MCP_WRITE_B(CANCTRL, CANCTRL_REQOP, REQOP_CONFIG);
	
	/* configure SJW and Tq duration */
	MCP_WRITE_B(CNF1, CNF1_SJW, 4-1); 	/* SJW = X-1 [Tq] */
	MCP_WRITE_B(CNF1, CNF1_BRP, 0);		/* Tq = 2*(X+1)/FOSC [s] */
	
	/* configure phase seg1 and propseg */
	MCP_WRITE_B(CNF2, CNF2_BTLMODE, 1);
	MCP_WRITE_B(CNF2, CNF2_SAM, 1);
	MCP_WRITE_B(CNF2, CNF2_PHSEG1, 4-1);	/* PHSEG1 = X-1 [Tq] */
	MCP_WRITE_B(CNF2, CNF2_PRSEG, 4-1);		/* PRSEG = X-1 [Tq] */
	
	/* configure phase seg2 and wakeup filter */
	MCP_WRITE_B(CNF3, CNF3_WAKFIL, 1);
	MCP_WRITE_B(CNF3, CNF3_PHSEG2, 4-1);	/* PHSEG2 = X-1 [Tq] */
	
	/* configure interrupts */
	MCP_WRITE_R(CANINTE, 0);
	
	// Mark all filter bits as don't care:
    mcp_write_can_id(RXMnSIDH(0), 1, 0);
    mcp_write_can_id(RXMnSIDH(1), 1, 0);
    // Anyway, set all filters to 0:
    mcp_write_can_id(RXF0SIDH, 0, 0);
    mcp_write_can_id(RXF1SIDH, 0, 0);
    mcp_write_can_id(RXF2SIDH, 0, 0);
    mcp_write_can_id(RXF3SIDH, 0, 0);
    mcp_write_can_id(RXF4SIDH, 0, 0);
    mcp_write_can_id(RXF5SIDH, 0, 0);
	
	/* request loopback mode */
	MCP_WRITE_B(CANCTRL, CANCTRL_REQOP, REQOP_LOOPBACK);
	
	uint8_t tmp = MCP_read(CANCTRL);
	uartPutString("CANCTRL = ");
	uartPutUInt8(tmp, 2);
	uartPutString("\n");
	tmp = MCP_read(CANSTAT);
	uartPutString("CANSTAT = ");
	uartPutUInt8(tmp, 2);
	uartPutString("\n");
	tmp = MCP_read(CNF1);
	uartPutString("CNF1 = ");
	uartPutUInt8(tmp, 2);
	uartPutString("\n");
	tmp = MCP_read(CNF2);
	uartPutString("CNF2 = ");
	uartPutUInt8(tmp, 2);
	uartPutString("\n");
	tmp = MCP_read(CNF3);
	uartPutString("CNF3 = ");
	uartPutUInt8(tmp, 2);
	uartPutString("\n");
	tmp = MCP_read(CANINTE);
	uartPutString("CANINTE = ");
	uartPutUInt8(tmp, 2);
	uartPutString("\n");
	
	/* main loop */
	while(1) {
		/* do nothing */
		
	}
}
