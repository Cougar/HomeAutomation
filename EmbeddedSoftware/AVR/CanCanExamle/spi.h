#ifndef SPI_H_
#define SPI_H_

#include <inttypes.h>
#include <avr/io.h>

#define SPIPORT PORTB
#define SPIPIN  PINB
#define SPIDDR  DDRB

#if defined(__AVR_ATmega8__)
#define SPISCK   PB5  
#define SPIMISO  PB4 
#define SPIMOSI  PB3  
#define SPISS    PB2 
#elif defined(__AVR_ATmega32__) || defined(__AVR_ATmega16__)
#define SPISCK   PB7
#define SPIMISO  PB6 
#define SPIMOSI  PB5  
#define SPISS    PB4
#else
#error "SPI pins undefined for this target"
#endif

#define SPI_SS_HIGH() (SPIPORT |= (1<<SPISS))
#define SPI_SS_LOW() (SPIPORT &= ~(1<<SPISS))

void spi_init(void);
uint8_t spi_readwrite(uint8_t data);
uint8_t spi_read(void);

#endif

