/************************************************************************/
/*                                                                      */
/*        Access DHT11 1-Wire Device with ATMEL AVRs                    */
/*                                                                      */
/*              Author: Peter Dannegger                                 */
/*                      danni@specs.de                                  */
/*                                                                      */
/* modified by Martin Thomas <eversmith@heizung-thomas.de> 9/2004       */
/* modified by Linus Lundin from Dallas 1-wire to DHT11 protocol        */
/************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>
#include "DHT11.h"

#include <config.h>

#ifdef DHT11_ONE_BUS

#define DHT11_GET_IN()   gpio_get_state(DHT11_PIN)
#define DHT11_OUT_LOW()  gpio_clr_pin(DHT11_PIN)
#define DHT11_OUT_HIGH() gpio_set_pin(DHT11_PIN)
#define DHT11_DIR_IN()   gpio_set_in(DHT11_PIN)
#define DHT11_DIR_OUT()  gpio_set_out(DHT11_PIN)


#else

/* set bus-config with dht11_set_bus() */
uint8_t DHT11_PIN_MASK;
volatile uint8_t* DHT11_IN;
volatile uint8_t* DHT11_OUT;
volatile uint8_t* DHT11_DDR;

#define DHT11_GET_IN()  ( *DHT11_IN & DHT11_PIN_MASK )
#define DHT11_OUT_LOW() ( *DHT11_OUT &= (uint8_t) ~DHT11_PIN_MASK )
#define DHT11OUT_HIGH() ( *DHT11_OUT |= (uint8_t)  DHT11_PIN_MASK )
#define DHT11DIR_IN()   ( *DHT11_DDR &= (uint8_t) ~DHT11_PIN_MASK )
#define DHT11DIR_OUT()  ( *DHT11_DDR |= (uint8_t)  DHT11_PIN_MASK )

void dht11_set_bus(volatile uint8_t* in,
	volatile uint8_t* out,
	volatile uint8_t* ddr,
	uint8_t pin)
{
	DHT11_DDR=ddr;
	DHT11_OUT=out;
	DHT11_IN=in;
	DHT11_PIN_MASK=(1<<pin);
	dht11_reset();
}
#endif

uint8_t dht11_readData(uint8_t* word)
{
	// set Pin as input - wait for clients to pull low
	DHT11_DIR_IN(); // input
	
	_delay_us(80);
	if (DHT11_GET_IN())		// no presence detect
	{
		//printf("Fel 1\n");
		return 0;
	}
	// nobody pulled to low, still high
	
	// after a delay the clients should release the line
	// and input-pin gets back to high due to pull-up-resistor
	_delay_us(80);
	if (!DHT11_GET_IN())		// no presence detect
	{
		//printf("Fel 2\n");
		return 0;
	}
	while (DHT11_GET_IN());
	_delay_us(30);
	while (!DHT11_GET_IN());
	uint8_t wordcounter = 0;
	uint8_t bitcounter = 0;
	_delay_us(50);
	for (wordcounter = 0; wordcounter < 5; wordcounter++) {
		for (bitcounter = 0; bitcounter < 8; bitcounter++) {
			word[wordcounter] = word[wordcounter] << 1;
			if (DHT11_GET_IN()) {
				word[wordcounter] |= 1;
				while (DHT11_GET_IN());
			} else {
				word[wordcounter] &= 0xfe;
			}
			while (!DHT11_GET_IN());
			_delay_us(50);
		}
	}
	if (word[0]+word[1]+word[2]+word[3] != word[4]) {
		return 0;
	}
	//printf("Data was %x %x %x %x %x\n", word[0], word[1], word[2], word[3], word[4]);
	return 1;
}


uint8_t dht11_start(void)
{
	DHT11_OUT_LOW(); // disable internal pull-up (maybe on from parasite)
	DHT11_DIR_OUT(); // pull OW-Pin low for 18ms
	return 1;
}
