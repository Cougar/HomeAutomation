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


#define ENC28J60_USART_SPI_MODE

 /* 
 * mac and ip have to be unique in your local area network. 
 * You can not have the same numbers in two devices: 
 */

#define ENC28J60_MAC1			0x54
#define ENC28J60_MAC2			0x55
#define ENC28J60_MAC3			0x58
#define ENC28J60_MAC4			0x10
#define ENC28J60_MAC5			0x00
#define ENC28J60_MAC6			0x27

#define ENC28J60_IP1			192
#define ENC28J60_IP2			168
#define ENC28J60_IP3			0
#define ENC28J60_IP4			50

 /**
 * ENC28J60 Chip Select port on avr. 
 */ 

#define ENC28J60_CS_PORT	PORTC
#define ENC28J60_CS_DDR		DDRC
#define ENC28J60_CS			PC0

 /**
 * ENC28J60 Reset port on avr. 
 */ 
#define ENC28J60_RESET_PORT	PORTC
#define ENC28J60_RESET_DDR	DDRC
#define ENC28J60_RESET		PC1

// only for USART in SPImode
#define XCK_DDR				DDRD
#define XCK					PD4


#endif /*CONFIG_H_*/
