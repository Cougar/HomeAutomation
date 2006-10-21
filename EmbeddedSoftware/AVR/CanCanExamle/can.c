/******************************************************************************
 * 
 * Controller Area Network (CAN) Demo-Application
 * Atmel AVR with Microchip MCP2515 
 * 
 * Copyright (C) 2005 Martin THOMAS, Kaiserslautern, Germany
 * <eversmith@heizung-thomas.de>
 * http://www.siwawi.arubi.uni-kl.de/avr_projects
 *
 *****************************************************************************
 *
 * File    : can.c
 * Version : 0.9
 * 
 * Summary : Interface between application and CAN-Hardware
 *           ("middle tier", "abstraction layer")
 *
 *****************************************************************************/

#include <string.h>

#include "can.h"
#include "mcp2515.h"
#include "timebase.h"

#if (CANDEBUG)
#include "uart.h"
#include "debughelper.h"
#include "termio.h"
#endif


/*extern*/ uint8_t gCANAutoProcess = CANAUTOPROCESS;


uint8_t can_init(uint8_t speedset)
{
	uint8_t res;
	
	res = mcp2515_init(speedset);
	
	if (res == MCP2515_OK) return CAN_OK;
	else return CAN_FAILINIT;
}

void can_initMessageStruct(CanMessage* msg)
{
	memset(msg,0,sizeof(CanMessage));
}

uint8_t can_sendMessage(const CanMessage* msg)
{
	uint8_t res, txbuf_n;
	uint8_t timeout = 0;
	uint16_t time;
	
	time = timebase_actTime();
	
	do {
		res = mcp2515_getNextFreeTXBuf(&txbuf_n); // info = addr.
		if (timebase_passedTimeMS(time) > CANSENDTIMEOUT ) timeout = 1;
	} while (res == MCP_ALLTXBUSY && !timeout);
	
	if (!timeout) {
		mcp2515_write_canMsg( txbuf_n, msg);
		mcp2515_start_transmit( txbuf_n );
		return CAN_OK;
	}
	else {
#if (CANDEBUG)
		term_puts_P("Transmit timeout\n");
#endif
		return CAN_FAILTX;
	}
}

uint8_t can_readMessage(CanMessage *msg)
{
	uint8_t stat, res;
	
	stat = mcp2515_readStatus();
	
	if ( stat & MCP_STAT_RX0IF ) {
		// Msg in Buffer 0
		mcp2515_read_canMsg( MCP_RXBUF_0, msg);
		mcp2515_modifyRegister(MCP_CANINTF, MCP_RX0IF, 0);
		res = CAN_OK;
	}
	else if ( stat & MCP_STAT_RX1IF ) {
		// Msg in Buffer 1
		mcp2515_read_canMsg( MCP_RXBUF_1, msg);
		mcp2515_modifyRegister(MCP_CANINTF, MCP_RX1IF, 0);
		res = CAN_OK;
	}
	else {
		res = CAN_NOMSG;
	}	
	
	return res;
}

/* returns either 
 #define CAN_MSGAVAIL   (3) - a message has been received
 #define CAN_NOMSG      (4) - no new message
*/
uint8_t can_checkReceive(void)
{
	uint8_t res;
	
	res = mcp2515_readStatus(); // RXnIF in Bit 1 and 0
	if ( res & MCP_STAT_RXIF_MASK ) {
		return CAN_MSGAVAIL;
	}
	else {
		return CAN_NOMSG;
	}
}

/* checks Controller-Error-State, returns CAN_OK or CAN_CTRLERROR 
   only errors (and not warnings) lead to "CTRLERROR" */
uint8_t can_checkError(void)
{
	uint8_t eflg = mcp2515_readRegister(MCP_EFLG);

	if ( eflg & MCP_EFLG_ERRORMASK ) {
		return CAN_CTRLERROR;
	}
	else {
		return CAN_OK;
	}
}

#if (CANDEBUG)

uint8_t can_testTransmit(const uint8_t ext, const uint8_t data2)
{
	CanMessage msg;
	
	can_initMessageStruct(&msg);
	msg.identifier = CANDEFAULTIDENT;
	msg.extended_identifier = ext;
	msg.dlc = 3;
	msg.dta[0] = 'M';
	msg.dta[1] = 'T';
	msg.dta[2] = data2;
	
	return can_sendMessage(&msg);
}

void can_debug1(void)
{
	uint8_t i;
	// CanMessage msg;
	
	term_puts_P("CAN-DEBUG 1\r");
	i = mcp2515_readRegister(MCP_CANCTRL);
	Debug_ByteToUart_P("CANCTRL", i);
	i = mcp2515_readRegister(MCP_CANSTAT);
	Debug_ByteToUart_P("CANSTAT", i);
	i = mcp2515_readRegister(MCP_CNF1);
	Debug_ByteToUart_P("CNF1", i);
	i = mcp2515_readRegister(MCP_CNF2);
	Debug_ByteToUart_P("CNF2", i);
	i = mcp2515_readRegister(MCP_CNF3);
	Debug_ByteToUart_P("CNF3", i);
	
#if CANUSELOOP == 1
	term_puts_P("Setting Loopback-Mode - ");
	if ( mcp2515_setCANCTRL_Mode(MODE_LOOPBACK) == MCP2515_OK) {
		uart_puts_P("OK\r");
	}
	else {
		uart_puts_P("failed\r");
	}
#else
term_puts_P("Setting Normal-Mode - ");
	if ( mcp2515_setCANCTRL_Mode(MODE_NORMAL) == MCP2515_OK) {
		uart_puts_P("OK\r");
	}
	else {
		uart_puts_P("failed\r");
	}
#endif 
	
	i = mcp2515_readRegister(MCP_CANCTRL);
	Debug_ByteToUart_P("CANCTRL", i);
	i = mcp2515_readRegister(MCP_RXB0CTRL);
	Debug_ByteToUart_P("RXB0CTRL", i);
	i = mcp2515_readRegister(MCP_RXB1CTRL);
	Debug_ByteToUart_P("RXB1CTRL", i);

#if 0	
	can_initMessageStruct(&msg);
	msg.identifier = CANDEFAULTIDENT;
	msg.extended_identifier = CAN_STDID; // CANDEFAULTIDENTEXT;
	msg.dlc = 3;
	msg.dta[0] = 'M';
	msg.dta[1] = 'T';
	msg.dta[2] = '1';
	term_puts_P("Sending a message...");
	can_sendMessage(&msg);
	
	msg.extended_identifier = CANDEFAULTIDENTEXT;
	msg.dta[2] = '2';
	term_puts_P("Sending a message...");
	can_sendMessage(&msg);
#endif
}

void can_dumpMessage(CanMessage *msg)
{
	uint8_t i;

	term_puts_P("\nMessage-Dump :\n");
	// identifier type 0=standard 1=extended:
	if ( (msg->extended_identifier) == CAN_EXTID ) {
		term_puts_P("Extended");
	}
	else {
		term_puts_P("Standard");
	}
	term_puts_P("-Identifier :\n");
	Debug_ByteToUart_P("Byte 0", (uint8_t) ( (msg->identifier) ));
	Debug_ByteToUart_P("Byte 1", (uint8_t) ( (msg->identifier) >> 8 ));
	if ( (msg->extended_identifier) == CAN_EXTID ) {
		Debug_ByteToUart_P("Byte 2", (uint8_t) ( (msg->identifier) >> 16 ));
		Debug_ByteToUart_P("Byte 3", (uint8_t) ( (msg->identifier) >> 24 ));
	}
	
	// data length:
	term_puts_P("Data :\n");
	Debug_ByteToUart_P( "dlc", msg->dlc );
	for (i=0;i<msg->dlc;i++) {
		Debug_ByteToUart_P( "dta", msg->dta[i] );
	}
	
	term_puts_P("Meta:\n");
	// used for receive only:
	// Received Remote Transfer Bit 
	//  (0=no... 1=remote transfer request received)
	Debug_ByteToUart_P( "rtr", msg->rtr );
	// Acceptence Filter that enabled the reception
	Debug_ByteToUart_P( "filhit", msg->filhit );
}

void can_dumpStatus(void)
{
	mcp2515_dumpExtendedStatus();
}

#endif /* CANDEBUG */
