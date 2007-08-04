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
 * Define which ADC that will be used, ADC0 to ADC7.
 * If using ATmega88 PDIP ADC7 wont be aviable, only on TFQP.
 * (syntax: TC_ADCx where x is the number och the ADC used)
 */
#define ADC5

/* 
 * For internal Vref 1.1 volt set VREF_INT, else  AVcc or Aref = 5 volt
 * will be used.
 *
 */
//#define VREF_INT

/*-----------------------------------------------
 * Functions
 * ---------------------------------------------*/
void adcTemperatureInit();
uint32_t getTC1047temperature();
#else
    #error Library still not compatible with others than mega88
#endif
#endif
