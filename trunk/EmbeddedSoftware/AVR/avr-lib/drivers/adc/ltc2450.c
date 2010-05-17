
/*-----------------------------------------------
 * Inclusions
 * ---------------------------------------------*/
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include <config.h>
#include "ltc2450.h"


/*-----------------------------------------------
 * Function Implementations
 * ---------------------------------------------*/

void ltc2450_init() {
	UBRR0 = 0;
	// CS is output
	SPI_DDR |= (1<<SPI_CS);
	LTC2450_UNSELECT();
	// SCK is output
	SPI_DDR |= (1<<SPI_SCK);
	// MISO is input
	SPI_DDR &= ~(1<<SPI_MISO);
	//select MSPIM, SPI-mode 0
	UCSR0C = (1<<UMSEL01)|(1<<UMSEL00)|(0<<UCPHA0)|(0<<UCPOL0);
	//enable receiver
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	//set "baudrate" to maximum ( BAUD=Fosc/2(UBRR0+1) )
	UBRR0 = 0;
	// clear old sample
	ltc2450_read();
}

uint16_t ltc2450_read() {
	uint8_t msb, lsb;
	LTC2450_SELECT();
	// read MSB
	UDR0 = 0xFF;
	while (!(UCSR0A&(1<<RXC0)));
	msb = UDR0;
	// read LSB
	UDR0 = 0xFF;
	while (!(UCSR0A&(1<<RXC0)));
	lsb = UDR0;
	LTC2450_UNSELECT();
	uint16_t result = (((uint16_t)msb) << 8) | ((uint16_t)lsb << 0);
	return result;
}

