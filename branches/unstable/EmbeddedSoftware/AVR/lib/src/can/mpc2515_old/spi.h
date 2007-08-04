#ifndef SPI_H_
#define SPI_H_

/* -----------------------------------------------------------------------------
 * Includes
 * ---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <avr/io.h>

/* -----------------------------------------------------------------------------
 * Defines
 * ---------------------------------------------------------------------------*/
#define DDR_SPI		DDRB
#define DD_MOSI		PB3
#define DD_MISO		PB4
#define DD_SCK		PB5
#define DD_CS		PB2

/* -----------------------------------------------------------------------------
 * Function prototypes
 * ---------------------------------------------------------------------------*/
void SPI_master_init(void);
void SPI_chip_select(void);
void SPI_chip_unselect(void);
uint8_t SPI_master_send(uint8_t byte);
void SPI_slave_init(void);
uint8_t SPI_slave_receive(void);


#endif /*SPI_H_*/
