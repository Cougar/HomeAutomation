/******************************************************************************
  gpio.h

  CoreCard GPIO Hardware Abstraction Layer

******************************************************************************/

#ifndef GPIO_H_
#define GPIO_H_

#include <inttypes.h>
#include <avr/io.h>

/******************************************************************************
* Port output functions
* void gpio_set_out() makes GPIOX pin as output
* void gpio_set_pin() sets GPIOX pin to logical 1 level (when output)
* void gpio_clr_pin() clears GPIOX pin to logical 0 level (when output)
* void gpio_toggle_pin() toggles level of output (when output)
*
* Port input functions
* void gpio_set_in() makes GPIOX pin as input
* void gpio_set_pullup() makes GPIOX pulled high (when input)
* void gpio_clr_pullup() removes pullup on GPIOX (when input)
* uint8_t gpio_get_state() returns logical level GPIOX pin
*******************************************************************************/
  static inline __attribute__ ((always_inline)) void gpio_set_pin(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr) {*port |= (1 << nr);}
  static inline __attribute__ ((always_inline)) void gpio_clr_pin(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr) {*port &= ~(1 << nr);}
  static inline __attribute__ ((always_inline)) void gpio_set_out(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr) {*ddr |= (1 << nr);}
  static inline __attribute__ ((always_inline)) void gpio_set_in(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr) {*ddr &= ~(1 << nr);}
  static inline __attribute__ ((always_inline)) void gpio_set_pullup(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr) {*port |= (1 << nr);}
  static inline __attribute__ ((always_inline)) void gpio_clr_pullup(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr) {*port &= ~(1 << nr);}
  static inline __attribute__ ((always_inline)) void gpio_toggle_pin(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr) {*port ^= (1 << nr);}
  static inline __attribute__ ((always_inline)) uint8_t gpio_get_state(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr) {return (*pin & (1 << nr)) != 0;}

// generic ports:
#define GPIO_D0			&PORTD,&PIND,&DDRD,PD0
#define PORT_GPIO_D0	PORTD
#define PIN_GPIO_D0		PIND
#define DDR_GPIO_D0		DDRD
#define NR_GPIO_D0		PD0

#define GPIO_D1			&PORTD,&PIND,&DDRD,PD1
#define PORT_GPIO_D1	PORTD
#define PIN_GPIO_D1		PIND
#define DDR_GPIO_D1		DDRD
#define NR_GPIO_D1		PD1

#define GPIO_D2			&PORTD,&PIND,&DDRD,PD2
#define PORT_GPIO_D2	PORTD
#define PIN_GPIO_D2		PIND
#define DDR_GPIO_D2		DDRD
#define NR_GPIO_D2		PD2

#define GPIO_D4			&PORTD,&PIND,&DDRD,PD4
#define PORT_GPIO_D4	PORTD
#define PIN_GPIO_D4		PIND
#define DDR_GPIO_D4		DDRD
#define NR_GPIO_D4		PD4

#define GPIO_D5			&PORTD,&PIND,&DDRD,PD5
#define PORT_GPIO_D5	PORTD
#define PIN_GPIO_D5		PIND
#define DDR_GPIO_D5		DDRD
#define NR_GPIO_D5		PD5

#define GPIO_D6			&PORTD,&PIND,&DDRD,PD6
#define PORT_GPIO_D6	PORTD
#define PIN_GPIO_D6		PIND
#define DDR_GPIO_D6		DDRD
#define NR_GPIO_D6		PD6

#define GPIO_D7			&PORTD,&PIND,&DDRD,PD7
#define PORT_GPIO_D7	PORTD
#define PIN_GPIO_D7		PIND
#define DDR_GPIO_D7		DDRD
#define NR_GPIO_D7		PD7


#define GPIO_B0			&PORTB,&PINB,&DDRB,PB0
#define PORT_GPIO_B0	PORTB
#define PIN_GPIO_B0		PINB
#define DDR_GPIO_B0		DDRB
#define NR_GPIO_B0		PB0

#define GPIO_B1			&PORTB,&PINB,&DDRB,PB1
#define PORT_GPIO_B1	PORTB
#define PIN_GPIO_B1		PINB
#define DDR_GPIO_B1		DDRB
#define NR_GPIO_B1		PB1

#define GPIO_B2			&PORTB,&PINB,&DDRB,PB2
#define PORT_GPIO_B2	PORTB
#define PIN_GPIO_B2		PINB
#define DDR_GPIO_B2		DDRB
#define NR_GPIO_B2		PB2

#define GPIO_B7			&PORTB,&PINB,&DDRB,PB7
#define PORT_GPIO_B7	PORTB
#define PIN_GPIO_B7		PINB
#define DDR_GPIO_B7		DDRB
#define NR_GPIO_B7		PB7


#define GPIO_C0			&PORTC,&PINC,&DDRC,PC0
#define PORT_GPIO_C0	PORTC
#define PIN_GPIO_C0		PINC
#define DDR_GPIO_C0		DDRC
#define NR_GPIO_C0		PC0

#define GPIO_C1			&PORTC,&PINC,&DDRC,PC1
#define PORT_GPIO_C1	PORTC
#define PIN_GPIO_C1		PINC
#define DDR_GPIO_C1		DDRC
#define NR_GPIO_C1		PC1

#define GPIO_C2			&PORTC,&PINC,&DDRC,PC2
#define PORT_GPIO_C2	PORTC
#define PIN_GPIO_C2		PINC
#define DDR_GPIO_C2		DDRC
#define NR_GPIO_C2		PC2

#define GPIO_C3			&PORTC,&PINC,&DDRC,PC3
#define PORT_GPIO_C3	PORTC
#define PIN_GPIO_C3		PINC
#define DDR_GPIO_C3		DDRC
#define NR_GPIO_C3		PC3

#define GPIO_C4			&PORTC,&PINC,&DDRC,PC4
#define PORT_GPIO_C4	PORTC
#define PIN_GPIO_C4		PINC
#define DDR_GPIO_C4		DDRC
#define NR_GPIO_C4		PC4

#define GPIO_C5			&PORTC,&PINC,&DDRC,PC5
#define PORT_GPIO_C5	PORTC
#define PIN_GPIO_C5		PINC
#define DDR_GPIO_C5		DDRC
#define NR_GPIO_C5		PC5

// for corecard:
#define EXP_A	&PORTB,&PINB,&DDRB,PB7
#define PORT_EXP_A	PORTB
#define PIN_EXP_A	PINB
#define DDR_EXP_A	DDRB
#define NR_EXP_A	PB7

#define EXP_B	&PORTB,&PINB,&DDRB,PB2
#define PORT_EXP_B	PORTB
#define PIN_EXP_B	PINB
#define DDR_EXP_B	DDRB
#define NR_EXP_B	PB2

#define EXP_C	&PORTB,&PINB,&DDRB,PB1
#define PORT_EXP_C	PORTB
#define PIN_EXP_C	PINB
#define DDR_EXP_C	DDRB
#define NR_EXP_C	PB1

#define EXP_D	&PORTB,&PINB,&DDRB,PB0
#define PORT_EXP_D	PORTB
#define PIN_EXP_D	PINB
#define DDR_EXP_D	DDRB
#define NR_EXP_D	PB0

#define EXP_E	&PORTD,&PIND,&DDRD,PD7
#define PORT_EXP_E	PORTD
#define PIN_EXP_E	PIND
#define DDR_EXP_E	DDRD
#define NR_EXP_E	PD7

#define EXP_F	&PORTD,&PIND,&DDRD,PD6
#define PORT_EXP_F	PORTD
#define PIN_EXP_F	PIND
#define DDR_EXP_F	DDRD
#define NR_EXP_F	PD6

#define EXP_G	&PORTD,&PIND,&DDRD,PD5
#define PORT_EXP_G	PORTD
#define PIN_EXP_G	PIND
#define DDR_EXP_G	DDRD
#define NR_EXP_G	PD5

#define EXP_H	&PORTD,&PIND,&DDRD,PD4
#define PORT_EXP_H	PORTD
#define PIN_EXP_H	PIND
#define DDR_EXP_H	DDRD
#define NR_EXP_H	PD4

#define EXP_I	&PORTD,&PIND,&DDRD,PD2
#define PORT_EXP_I	PORTD
#define PIN_EXP_I	PIND
#define DDR_EXP_I	DDRD
#define NR_EXP_I	PD2

#define EXP_J	&PORTD,&PIND,&DDRD,PD1
#define PORT_EXP_J	PORTD
#define PIN_EXP_J	PIND
#define DDR_EXP_J	DDRD
#define NR_EXP_J	PD1

#define EXP_K	&PORTD,&PIND,&DDRD,PD0
#define PORT_EXP_K	PORTD
#define PIN_EXP_K	PIND
#define DDR_EXP_K	DDRD
#define NR_EXP_K	PC0

#define EXP_L	&PORTC,&PINC,&DDRC,PC5
#define PORT_EXP_L	PORTC
#define PIN_EXP_L	PINC
#define DDR_EXP_L	DDRC
#define NR_EXP_L	PC5

#define EXP_M	&PORTC,&PINC,&DDRC,PC4
#define PORT_EXP_M	PORTC
#define PIN_EXP_M	PINC
#define DDR_EXP_M	DDRC
#define NR_EXP_M	PC4

#define EXP_N	&PORTC,&PINC,&DDRC,PC2
#define PORT_EXP_N	PORTC
#define PIN_EXP_N	PINC
#define DDR_EXP_N	DDRC
#define NR_EXP_N	PC2

#define EXP_O	&PORTC,&PINC,&DDRC,PC1
#define PORT_EXP_O	PORTC
#define PIN_EXP_O	PINC
#define DDR_EXP_O	DDRC
#define NR_EXP_O	PC1

#define EXP_P	&PORTC,&PINC,&DDRC,PC0
#define PORT_EXP_P	PORTC
#define PIN_EXP_P	PINC
#define DDR_EXP_P	DDRC
#define NR_EXP_P	PC0

#endif
//eof gpio.h
