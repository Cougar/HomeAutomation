#ifndef TC1047_H_
#define TC1047_H_

/*-----------------------------------------------
 * Includes
 * ---------------------------------------------*/
#include <stdlib.h>
#include <inttypes.h>

/*-----------------------------------------------
 * Defines
 * ---------------------------------------------*/

/* 
 * If using ATmega88 PDIP enable this feature,
 * since PDIP doesn't have ADC7 it will use the ADC0 instead
 *
 */
#define PDIP 1

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

#endif
