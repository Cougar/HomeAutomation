/**
 * @file	spi.c
 * SPI drivers for ATmega8.
 * 
 * @author	Jimmy Myhrman (jimmy@myhrman.org)
 * @date	2005-11-28
 */

/* -----------------------------------------------------------------------------
 * Includes
 * ---------------------------------------------------------------------------*/
#include "spi.h"


/* -----------------------------------------------------------------------------
 * Functions
 * ---------------------------------------------------------------------------*/

/**
 * Initializes the SPI interface to master mode.
 */
void SPI_master_init() {
	DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK)|(1<<DD_CS);	/* MOSI, SCK and SS are outputs */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);			/* enable SPI, Master, set clock rate fck/16 */
}

/**
 * Activates the chip select signal, which is active low.
 */
void SPI_chip_select() {
	PORTB &= ~(1<<DD_CS);		/* enable chip select (active low) */
}

/**
 * De-activates the chip select signal, which is active low.
 */
void SPI_chip_unselect() {
	PORTB |= (1<<DD_CS);		/* disable chip select (active low) */
}

/**
 * Sends a data byte via SPI as a master. The function waits till the byte has
 * been transmitted before it returns.
 * 
 * @param byte The data byte to send.
 * @return The received byte.
 */
uint8_t SPI_master_send(uint8_t byte) {
	SPDR = byte;				/* start transmission */
	while(!(SPSR & (1<<SPIF)));	/* wait for transmission complete */
	return SPDR;
}

/**
 * Initializes the SPI interface to slave mode.
 */
void SPI_slave_init() {
	DDR_SPI = (1<<DD_MISO);		/* set MISO output, all others input */
	SPCR = (1<<SPE);			/* enable SPI */
}

/**
 * Reads a data byte from the SPI as a slave. The function waits for a reception
 * to complete and returns the received byte.
 * 
 * @return The received byte.
 */
uint8_t SPI_slave_receive(void) {
	while(!(SPSR & (1<<SPIF)));	/* wait for reception complete */
	return SPDR;				/* return data register */
}
