/******************************************************************************
  gpio.h

  CoreCard GPIO Hardware Abstraction Layer

******************************************************************************/

#ifndef GPIO_H_
#define GPIO_H_

#include <inttypes.h>
#include <avr/io.h>

/* Hack to fix lack of PBx, PCx defines in /usr/lib/avr/include/avr/portpins.h */
/* http://savannah.nongnu.org/bugs/?25930 http://www.mail-archive.com/avr-libc-dev@nongnu.org/msg03306.html */
#include <drivers/mcu/portpins.h>

/******************************************************************************
* Port output functions
* void gpio_set_pin() sets GPIOX pin to logical 1 level (when output)
* void gpio_clr_pin() clears GPIOX pin to logical 0 level (when output)
* void gpio_toggle_pin() toggles level of output (when output)
* uint8_t gpio_get_output_state() returns the output_state
*
* Port input functions
* void gpio_set_pullup() makes GPIOX pulled high (when input)
* void gpio_clr_pullup() removes pullup on GPIOX (when input)
* uint8_t gpio_get_state() returns logical level GPIOX pin
* 
* Direction functions
* void gpio_set_in() makes GPIOX pin as input
* void gpio_set_out() makes GPIOX pin as output
* uint8_t gpio_get_direction() returns the choosen direction
* 
*******************************************************************************/
  static inline __attribute__ ((always_inline)) void gpio_set_pin(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr, uint8_t pcint) {*port |= (1 << nr);}
  static inline __attribute__ ((always_inline)) void gpio_clr_pin(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr, uint8_t pcint) {*port &= ~(1 << nr);}
  static inline __attribute__ ((always_inline)) void gpio_toggle_pin(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr, uint8_t pcint) {*port ^= (1 << nr);}
  static inline __attribute__ ((always_inline)) uint8_t gpio_get_output_state(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr, uint8_t pcint) {return (*port & (1 << nr)) != 0;}

  static inline __attribute__ ((always_inline)) void gpio_set_pullup(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr, uint8_t pcint) {*port |= (1 << nr);}
  static inline __attribute__ ((always_inline)) void gpio_clr_pullup(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr, uint8_t pcint) {*port &= ~(1 << nr);}
  static inline __attribute__ ((always_inline)) uint8_t gpio_get_state(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr, uint8_t pcint) {return (*pin & (1 << nr)) != 0;}

  static inline __attribute__ ((always_inline)) void gpio_set_out(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr, uint8_t pcint) {*ddr |= (1 << nr);}
  static inline __attribute__ ((always_inline)) void gpio_set_in(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr, uint8_t pcint) {*ddr &= ~(1 << nr);}
  static inline __attribute__ ((always_inline)) uint8_t gpio_get_direction(volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr, uint8_t pcint) {return (*ddr & (1 << nr)) != 0;}

#define GPIO_PIN_HIGH		1
#define GPIO_PIN_LOW		0
#define GPIO_PORT_INPUT		0
#define GPIO_PORT_OUTPUT	1
#define GPIO_PORT_HIGH		1
#define GPIO_PORT_LOW		0

// generic ports:
#define GPIO_D0			&PORTD,&PIND,&DDRD,PD0,16
#define GPIO_D0_PCINT_vect	PCINT2_vect
#define PORT_GPIO_D0	PORTD
#define PIN_GPIO_D0		PIND
#define DDR_GPIO_D0		DDRD
#define NR_GPIO_D0		PD0

#define GPIO_D1			&PORTD,&PIND,&DDRD,PD1,17
#define GPIO_D1_PCINT_vect	PCINT2_vect
#define PORT_GPIO_D1	PORTD
#define PIN_GPIO_D1		PIND
#define DDR_GPIO_D1		DDRD
#define NR_GPIO_D1		PD1

#define GPIO_D2			&PORTD,&PIND,&DDRD,PD2,18
#define GPIO_D2_PCINT_vect	PCINT2_vect
#define PORT_GPIO_D2	PORTD
#define PIN_GPIO_D2		PIND
#define DDR_GPIO_D2		DDRD
#define NR_GPIO_D2		PD2

#define GPIO_D4			&PORTD,&PIND,&DDRD,PD4,20
#define GPIO_D4_PCINT_vect	PCINT2_vect
#define PORT_GPIO_D4	PORTD
#define PIN_GPIO_D4		PIND
#define DDR_GPIO_D4		DDRD
#define NR_GPIO_D4		PD4

#define GPIO_D5			&PORTD,&PIND,&DDRD,PD5,21
#define GPIO_D5_PCINT_vect	PCINT2_vect
#define PORT_GPIO_D5	PORTD
#define PIN_GPIO_D5		PIND
#define DDR_GPIO_D5		DDRD
#define NR_GPIO_D5		PD5

#define GPIO_D6			&PORTD,&PIND,&DDRD,PD6,22
#define GPIO_D6_PCINT_vect	PCINT2_vect
#define PORT_GPIO_D6	PORTD
#define PIN_GPIO_D6		PIND
#define DDR_GPIO_D6		DDRD
#define NR_GPIO_D6		PD6

#define GPIO_D7			&PORTD,&PIND,&DDRD,PD7,23
#define GPIO_D7_PCINT_vect	PCINT2_vect
#define PORT_GPIO_D7	PORTD
#define PIN_GPIO_D7		PIND
#define DDR_GPIO_D7		DDRD
#define NR_GPIO_D7		PD7


#define GPIO_B0			&PORTB,&PINB,&DDRB,PB0,0
#define GPIO_B0_PCINT_vect	PCINT0_vect
#define PORT_GPIO_B0	PORTB
#define PIN_GPIO_B0		PINB
#define DDR_GPIO_B0		DDRB
#define NR_GPIO_B0		PB0

#define GPIO_B1			&PORTB,&PINB,&DDRB,PB1,1
#define GPIO_B1_PCINT_vect	PCINT0_vect
#define PORT_GPIO_B1	PORTB
#define PIN_GPIO_B1		PINB
#define DDR_GPIO_B1		DDRB
#define NR_GPIO_B1		PB1

#define GPIO_B2			&PORTB,&PINB,&DDRB,PB2,2
#define GPIO_B2_PCINT_vect	PCINT0_vect
#define PORT_GPIO_B2	PORTB
#define PIN_GPIO_B2		PINB
#define DDR_GPIO_B2		DDRB
#define NR_GPIO_B2		PB2

#define GPIO_B7			&PORTB,&PINB,&DDRB,PB7,7
#define GPIO_B7_PCINT_vect	PCINT0_vect
#define PORT_GPIO_B7	PORTB
#define PIN_GPIO_B7		PINB
#define DDR_GPIO_B7		DDRB
#define NR_GPIO_B7		PB7


#define GPIO_C0			&PORTC,&PINC,&DDRC,PC0,8
#define GPIO_C0_PCINT_vect	PCINT1_vect
#define PORT_GPIO_C0	PORTC
#define PIN_GPIO_C0		PINC
#define DDR_GPIO_C0		DDRC
#define NR_GPIO_C0		PC0

#define GPIO_C1			&PORTC,&PINC,&DDRC,PC1,9
#define GPIO_C1_PCINT_vect	PCINT1_vect
#define PORT_GPIO_C1	PORTC
#define PIN_GPIO_C1		PINC
#define DDR_GPIO_C1		DDRC
#define NR_GPIO_C1		PC1

#define GPIO_C2			&PORTC,&PINC,&DDRC,PC2,10
#define GPIO_C2_PCINT_vect	PCINT1_vect
#define PORT_GPIO_C2	PORTC
#define PIN_GPIO_C2		PINC
#define DDR_GPIO_C2		DDRC
#define NR_GPIO_C2		PC2

#define GPIO_C3			&PORTC,&PINC,&DDRC,PC3,11
#define GPIO_C3_PCINT_vect	PCINT1_vect
#define PORT_GPIO_C3	PORTC
#define PIN_GPIO_C3		PINC
#define DDR_GPIO_C3		DDRC
#define NR_GPIO_C3		PC3

#define GPIO_C4			&PORTC,&PINC,&DDRC,PC4,12
#define GPIO_C4_PCINT_vect	PCINT1_vect
#define PORT_GPIO_C4	PORTC
#define PIN_GPIO_C4		PINC
#define DDR_GPIO_C4		DDRC
#define NR_GPIO_C4		PC4

#define GPIO_C5			&PORTC,&PINC,&DDRC,PC5,13
#define GPIO_C5_PCINT_vect	PCINT1_vect
#define PORT_GPIO_C5	PORTC
#define PIN_GPIO_C5		PINC
#define DDR_GPIO_C5		DDRC
#define NR_GPIO_C5		PC5

// for corecard:
#define EXP_A	&PORTB,&PINB,&DDRB,PB7,7
#define EXP_D_PCINT_vect	PCINT0_vect
#define PORT_EXP_A	PORTB
#define PIN_EXP_A	PINB
#define DDR_EXP_A	DDRB
#define NR_EXP_A	PB7

#define EXP_B	&PORTB,&PINB,&DDRB,PB2,2
#define EXP_D_PCINT_vect	PCINT0_vect
#define PORT_EXP_B	PORTB
#define PIN_EXP_B	PINB
#define DDR_EXP_B	DDRB
#define NR_EXP_B	PB2

#define EXP_C	&PORTB,&PINB,&DDRB,PB1,1
#define EXP_D_PCINT_vect	PCINT0_vect
#define PORT_EXP_C	PORTB
#define PIN_EXP_C	PINB
#define DDR_EXP_C	DDRB
#define NR_EXP_C	PB1

#define EXP_D	&PORTB,&PINB,&DDRB,PB0,0
#define EXP_D_PCINT_vect	PCINT0_vect
#define PORT_EXP_D	PORTB
#define PIN_EXP_D	PINB
#define DDR_EXP_D	DDRB
#define NR_EXP_D	PB0

#define EXP_E	&PORTD,&PIND,&DDRD,PD7,23
#define EXP_E_PCINT_vect	PCINT2_vect
#define PORT_EXP_E	PORTD
#define PIN_EXP_E	PIND
#define DDR_EXP_E	DDRD
#define NR_EXP_E	PD7

#define EXP_F	&PORTD,&PIND,&DDRD,PD6,22
#define EXP_F_PCINT_vect	PCINT2_vect
#define PORT_EXP_F	PORTD
#define PIN_EXP_F	PIND
#define DDR_EXP_F	DDRD
#define NR_EXP_F	PD6

#define EXP_G	&PORTD,&PIND,&DDRD,PD5,21
#define EXP_G_PCINT_vect	PCINT2_vect
#define PORT_EXP_G	PORTD
#define PIN_EXP_G	PIND
#define DDR_EXP_G	DDRD
#define NR_EXP_G	PD5

#define EXP_H	&PORTD,&PIND,&DDRD,PD4,20
#define EXP_H_PCINT_vect	PCINT2_vect
#define PORT_EXP_H	PORTD
#define PIN_EXP_H	PIND
#define DDR_EXP_H	DDRD
#define NR_EXP_H	PD4

#define EXP_I	&PORTD,&PIND,&DDRD,PD2,18
#define EXP_I_PCINT_vect	PCINT2_vect
#define PORT_EXP_I	PORTD
#define PIN_EXP_I	PIND
#define DDR_EXP_I	DDRD
#define NR_EXP_I	PD2

#define EXP_J	&PORTD,&PIND,&DDRD,PD1,17
#define EXP_J_PCINT_vect	PCINT2_vect
#define PORT_EXP_J	PORTD
#define PIN_EXP_J	PIND
#define DDR_EXP_J	DDRD
#define NR_EXP_J	PD1

#define EXP_K	&PORTD,&PIND,&DDRD,PD0,16
#define EXP_K_PCINT_vect	PCINT2_vect
#define PORT_EXP_K	PORTD
#define PIN_EXP_K	PIND
#define DDR_EXP_K	DDRD
#define NR_EXP_K	PC0

#define EXP_L	&PORTC,&PINC,&DDRC,PC5,13
#define EXP_L_PCINT_vect	PCINT1_vect
#define PORT_EXP_L	PORTC
#define PIN_EXP_L	PINC
#define DDR_EXP_L	DDRC
#define NR_EXP_L	PC5

#define EXP_M	&PORTC,&PINC,&DDRC,PC4,12
#define EXP_M_PCINT_vect	PCINT1_vect
#define PORT_EXP_M	PORTC
#define PIN_EXP_M	PINC
#define DDR_EXP_M	DDRC
#define NR_EXP_M	PC4

#define EXP_N	&PORTC,&PINC,&DDRC,PC2,10
#define EXP_N_PCINT_vect	PCINT1_vect
#define PORT_EXP_N	PORTC
#define PIN_EXP_N	PINC
#define DDR_EXP_N	DDRC
#define NR_EXP_N	PC2

#define EXP_O	&PORTC,&PINC,&DDRC,PC1,9
#define EXP_O_PCINT_vect	PCINT1_vect
#define PORT_EXP_O	PORTC
#define PIN_EXP_O	PINC
#define DDR_EXP_O	DDRC
#define NR_EXP_O	PC1

#define EXP_P	&PORTC,&PINC,&DDRC,PC0,8
#define EXP_P_PCINT_vect	PCINT1_vect
#define PORT_EXP_P		PORTC
#define PIN_EXP_P		PINC
#define DDR_EXP_P		DDRC
#define NR_EXP_P		PC0

// for NodeEssential:
#define Essential_0		&PORTD,&PIND,&DDRD,PD2,18
#define Essential_0_PCINT_vect	PCINT2_vect
#define PORT_Essential_0	PORTD
#define PIN_Essential_0		PIND
#define DDR_Essential_0		DDRD
#define NR_Essential_0		PD2

#define Essential_1		&PORTD,&PIND,&DDRD,PD1,17
#define Essential_1_PCINT_vect	PCINT2_vect
#define PORT_Essential_1	PORTD
#define PIN_Essential_1		PIND
#define DDR_Essential_1		DDRD
#define NR_Essential_1		PD1

#define Essential_2		&PORTD,&PIND,&DDRD,PD0,16
#define Essential_2_PCINT_vect	PCINT2_vect
#define PORT_Essential_2	PORTD
#define PIN_Essential_2		PIND
#define DDR_Essential_2		DDRD
#define NR_Essential_2		PD0

#define Essential_3		&PORTD,&PIND,&DDRD,PD4,20
#define Essential_3_PCINT_vect	PCINT2_vect
#define PORT_Essential_3	PORTD
#define PIN_Essential_3		PIND
#define DDR_Essential_3		DDRD
#define NR_Essential_3		PD4

#define Essential_4		&PORTD,&PIND,&DDRD,PD5,21
#define Essential_4_PCINT_vect	PCINT2_vect
#define PORT_Essential_4	PORTD
#define PIN_Essential_4		PIND
#define DDR_Essential_4		DDRD
#define NR_Essential_4		PD5

#define Essential_5		&PORTD,&PIND,&DDRD,PD6,22
#define Essential_5_PCINT_vect	PCINT2_vect
#define PORT_Essential_5	PORTD
#define PIN_Essential_5		PIND
#define DDR_Essential_5		DDRD
#define NR_Essential_5		PD6

#define Essential_6		&PORTB,&PINB,&DDRB,PB0,0
#define Essential_6_PCINT_vect	PCINT0_vect
#define PORT_Essential_6	PORTB
#define PIN_Essential_6		PINB
#define DDR_Essential_6		DDRB
#define NR_Essential_6		PB0

#define Essential_7		&PORTB,&PINB,&DDRB,PB1,1
#define Essential_7_PCINT_vect	PCINT0_vect
#define PORT_Essential_7	PORTB
#define PIN_Essential_7		PINB
#define DDR_Essential_7		DDRB
#define NR_Essential_7		PB1

#define Essential_8		&PORTC,&PINC,&DDRC,PC4,12
#define Essential_8_PCINT_vect	PCINT1_vect
#define PORT_Essential_8	PORTC
#define PIN_Essential_8		PINC
#define DDR_Essential_8		DDRC
#define NR_Essential_8		PC4

#define Essential_9		&PORTC,&PINC,&DDRC,PC5,13
#define Essential_9_PCINT_vect	PCINT1_vect
#define PORT_Essential_9	PORTC
#define PIN_Essential_9		PINC
#define DDR_Essential_9		DDRC
#define NR_Essential_9		PC5
#endif
//eof gpio.h
