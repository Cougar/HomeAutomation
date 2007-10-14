/**
 * Configuration file .
 */

#ifndef ENC28J60_CFG_H_
#define ENC28J60_CFG_H_

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
 
 /**
 * ENC28J60 SPI select. For USART in SPI-mode, use the following defined 
 */ 

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
#define ENC28J60_RESET_CS	1

// only for USART in SPImode
#define XCK_DDR				DDRD
#define XCK					PD4


#endif /*ENC28J60_CFG_H_*/
