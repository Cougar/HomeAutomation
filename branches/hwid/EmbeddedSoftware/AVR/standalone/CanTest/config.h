#ifndef CONFIG_H_
#define CONFIG_H_

// Kolla makefile för att sätta MCU!!

#define F_CPU 					20000000UL

#define UART_OUTPUT /* If you got the node connected to a computer enable this */
//#define LED_OUTPUT /* If you havent got any connection to a computer but still got some IOs free */

#define SENDING_ID	0x1fffffffUL
#define ECHO_RECEIVE_ID		0x1700000UL
#define ECHO_SENDING_ID		0x17000FFUL


//#define MCU atmega88
#define CAN_CONTROLLER 			MCP2515
#define MCP_CS_PORT 			PORTB
#define MCP_CS_DDR 				DDRB
#define MCP_CS_BIT 				PB2
#define MCP_CAN_BITRATE_KBPS 	250
#define MCP_CLOCK_FREQ_MHZ 		20
#define MCP_CLOCK_PRESC 		1

#define SERIAL_BAUDRATE 		9600
#define SERIAL_RX_BUFFER_SIZE	32
#define SERIAL_TX_BUFFER_SIZE	64

#endif /*CONFIG_H_*/
