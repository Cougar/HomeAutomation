#ifndef CONFIG_H_
#define CONFIG_H_

// Kolla makefile för att sätta MCU!!

#define F_CPU 					20000000UL

//#define MCU atmega88
#define CAN_CONTROLLER 			MCP2515
#define MCP_CS_PORT 			PORTB
#define MCP_CS_DDR 				DDRB
#define MCP_CS_BIT 				PB2
#define MCP_CAN_BITRATE_KBPS 	250
#define MCP_CLOCK_FREQ_MHZ 		20
#define MCP_CLOCK_PRESC 		1

#define SERIAL_BAUDRATE 		19200
#define SERIAL_RX_BUFFER_SIZE	32
#define SERIAL_TX_BUFFER_SIZE	64

/* set to 0 for not sending timestamps on bus */
#define SENDTIMESTAMP			0

#define USE_STDCAN				1
#define NODE_ID 				0
#define STDCAN_FILTER 			0


#endif /*CONFIG_H_*/
