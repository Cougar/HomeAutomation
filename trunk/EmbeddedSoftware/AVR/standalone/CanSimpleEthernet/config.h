#ifndef CONFIG_H_
#define CONFIG_H_

// Kolla makefile för att sätta MCU!!

#define F_CPU 					8000000UL

//#define MCU atmega88
#define CAN_CONTROLLER 			MCP2515
#define MCP_CS_PORT 			PORTB
#define MCP_CS_DDR 				DDRB
#define MCP_CS_BIT 				PB2
#define MCP_CAN_BITRATE_KBPS 	250
#define MCP_CLOCK_FREQ_MHZ 		20
#define MCP_CLOCK_PRESC 		1


//#define ENC28J60_USART_SPI_MODE

 /**
 * ENC28J60 Chip Select port on avr. 
 */ 
 
#define ENC28J60_CS_PORT	PORTC
#define ENC28J60_CS_DDR		DDRC
#define ENC28J60_CS			0

 /**
 * ENC28J60 Reset port on avr. 
 */ 

#define ENC28J60_RESET_PORT	PORTC
#define ENC28J60_RESET_DDR	DDRC
#define ENC28J60_RESET	1

// only for USART in SPImode
#define XCK_DDR				DDRD
#define XCK					PD4


#endif /*CONFIG_H_*/
