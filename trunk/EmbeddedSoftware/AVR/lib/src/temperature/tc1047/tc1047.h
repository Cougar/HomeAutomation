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
#define PDIP 0

/* 
 * For internal Vref use 1.1. AVcc or Aref use 5 
 * TODO testa libbet med VREF 1.1
 *
 */
#define VREF 5

/*-----------------------------------------------
 * Functions
 * ---------------------------------------------*/
void adcTemperatureInit();
uint32_t getTC1047temperature();

#endif
