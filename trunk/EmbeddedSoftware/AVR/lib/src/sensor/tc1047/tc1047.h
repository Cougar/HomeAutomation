#ifndef TC1047_H_
#define TC1047_H_

#if defined(__AVR_ATmega88__)

/*-----------------------------------------------
 * Includes
 * ---------------------------------------------*/
#include <stdlib.h>
#include <inttypes.h>

/*-----------------------------------------------
 * Defines
 * ---------------------------------------------*/

/* 
 * If using ATmega88 PDIP ADC7 wont be aviable, only on TFQP.
 *
 */
#define ADC5

/* 
 * For internal Vref 1.1 volt set VREF_INT 1. For  AVcc or Aref = 5 volt 
 * set VREF_INT = 0
 *
 */
#define VREF_INT 0

/*-----------------------------------------------
 * Functions
 * ---------------------------------------------*/
void adcTemperatureInit();
uint32_t getTC1047temperature();
#else
    #error Library still not compatible with others than mega88
#endif
#endif
