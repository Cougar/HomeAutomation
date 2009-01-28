/******************************************************************************
  gpio.h

  CoreCard GPIO Hardware Abstraction Layer

******************************************************************************/

#ifndef GPIO_H_
#define GPIO_H_

#include <inttypes.h>
#include <avr/io.h>

/******************************************************************************
* void GPIOX_set() sets GPIOX pin to logical 1 level.
* void GPIOX_clr() clears GPIOX pin to logical 0 level.
* void GPIOX_make_in makes GPIOX pin as input.
* void GPIOX_make_out makes GPIOX pin as output.
* uint8_t GPIOX_read() returns logical level GPIOX pin.
* uint8_t GPIOX_state() returns configuration of GPIOX port.
*******************************************************************************/
#define INLINE static inline __attribute__ ((always_inline))

#define HAL_ASSIGN_PIN(name, port, bit) \
INLINE void  GPIO_##name##_set(void)         {PORT##port |= (1 << bit);} \
INLINE void  GPIO_##name##_clr(void)         {PORT##port &= ~(1 << bit);} \
INLINE uint8_t  GPIO_##name##_read(void)     {return (PIN##port & (1 << bit)) != 0;} \
INLINE uint8_t  GPIO_##name##_state(void)    {return (DDR##port & (1 << bit)) != 0;} \
INLINE void  GPIO_##name##_make_out(void)    {DDR##port |= (1 << bit);} \
INLINE void  GPIO_##name##_make_in(void)     {DDR##port &= ~(1 << bit); PORT##port &= ~(1 << bit);} \
INLINE void  GPIO_##name##_make_pullup(void) {PORT##port |= (1 << bit);}\
INLINE void  GPIO_##name##_toggle(void)      {PORT##port ^= (1 << bit);} 

// the macros for the manipulation by EXP_A
HAL_ASSIGN_PIN(EXP_A, B, 7);
// the macros for the manipulation by EXP_B
HAL_ASSIGN_PIN(EXP_B, B, 2);
// the macros for the manipulation by EXP_C
HAL_ASSIGN_PIN(EXP_C, B, 1);
// the macros for the manipulation by EXP_D
HAL_ASSIGN_PIN(EXP_D, B, 0);
// the macros for the manipulation by EXP_E
HAL_ASSIGN_PIN(EXP_E, D, 7);
// the macros for the manipulation by EXP_F
HAL_ASSIGN_PIN(EXP_F, D, 6);
// the macros for the manipulation by EXP_G
HAL_ASSIGN_PIN(EXP_G, D, 5);
// the macros for the manipulation by EXP_H
HAL_ASSIGN_PIN(EXP_H, D, 4);
// the macros for the manipulation by EXP_I
HAL_ASSIGN_PIN(EXP_I, D, 2);
// the macros for the manipulation by EXP_J
HAL_ASSIGN_PIN(EXP_J,  D, 1);
// the macros for the manipulation by EXP_K
HAL_ASSIGN_PIN(EXP_K, D, 0);
// the macros for the manipulation by EXP_L
HAL_ASSIGN_PIN(EXP_L, C, 5);
// the macros for the manipulation by EXP_M
HAL_ASSIGN_PIN(EXP_M, C, 4);
// the macros for the manipulation by EXP_N
HAL_ASSIGN_PIN(EXP_N, C, 2);
// the macros for the manipulation by EXP_O
HAL_ASSIGN_PIN(EXP_O, C, 5);
// the macros for the manipulation by EXP_P
HAL_ASSIGN_PIN(EXP_P, C, 1);
// the macros for the manipulation by EXP_Q
HAL_ASSIGN_PIN(EXP_Q, C, 7);
// the macros for the manipulation by EXP_R
HAL_ASSIGN_PIN(EXP_R, C, 7);

// the macros for the manipulation by PORTB BIT0
HAL_ASSIGN_PIN(GPIO_B0, B, 0);
// the macros for the manipulation by PORTB BIT1
HAL_ASSIGN_PIN(GPIO_B1, B, 1);
// the macros for the manipulation by PORTB BIT2
HAL_ASSIGN_PIN(GPIO_B2, B, 2);
// the macros for the manipulation by PORTB BIT3	bit 3-5 are spi communication with mcp2515
//HAL_ASSIGN_PIN(GPIO_B3, B, 3);
// the macros for the manipulation by PORTB BIT4
//HAL_ASSIGN_PIN(GPIO_B4, B, 4);
// the macros for the manipulation by PORTB BIT5
//HAL_ASSIGN_PIN(GPIO_B5, B, 5);
// the macros for the manipulation by PORTB BIT6	bit 6 is external clock from mcp2515
//HAL_ASSIGN_PIN(GPIO_B6, B, 6);
// the macros for the manipulation by PORTB BIT7
HAL_ASSIGN_PIN(GPIO_B7, B, 7);

// the macros for the manipulation by PORTD BIT0
HAL_ASSIGN_PIN(GPIO_D0, D, 0);
// the macros for the manipulation by PORTD BIT1
HAL_ASSIGN_PIN(GPIO_D1, D, 1);
// the macros for the manipulation by PORTD BIT2
HAL_ASSIGN_PIN(GPIO_D2, D, 2);
// the macros for the manipulation by PORTD BIT3	bit 3 is interrup from mcp2515
//HAL_ASSIGN_PIN(GPIO_D3, D, 3);
// the macros for the manipulation by PORTD BIT4
HAL_ASSIGN_PIN(GPIO_D4, D, 4);
// the macros for the manipulation by PORTD BIT5
HAL_ASSIGN_PIN(GPIO_D5, D, 5);
// the macros for the manipulation by PORTD BIT6
HAL_ASSIGN_PIN(GPIO_D6, D, 6);
// the macros for the manipulation by PORTD BIT7
HAL_ASSIGN_PIN(GPIO_D7, D, 7);

// the macros for the manipulation by PORTC BIT0
HAL_ASSIGN_PIN(GPIO_C0, C, 0);
// the macros for the manipulation by PORTC BIT1
HAL_ASSIGN_PIN(GPIO_C1, C, 1);
// the macros for the manipulation by PORTC BIT2
HAL_ASSIGN_PIN(GPIO_C2, C, 2);
// the macros for the manipulation by PORTC BIT3	bit 3 is cs on corecard but could be used on nodes
HAL_ASSIGN_PIN(GPIO_C3, C, 3);
// the macros for the manipulation by PORTC BIT4
HAL_ASSIGN_PIN(GPIO_C4, C, 4);
// the macros for the manipulation by PORTC BIT5
HAL_ASSIGN_PIN(GPIO_C5, C, 5);
// the macros for the manipulation by PORTC BIT6	bit 6 is reset
//HAL_ASSIGN_PIN(GPIO_C6, C, 6);



#endif
//eof gpio.h
