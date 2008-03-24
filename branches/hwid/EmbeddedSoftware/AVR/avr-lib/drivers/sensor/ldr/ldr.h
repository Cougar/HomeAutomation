#ifndef LDR_H_
#define LDR_H_

#if defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) 

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
 * If using ATmega88/168 PDIP ADC7 wont be aviable, only on TFQP.
 * (syntax: ADCx where x is the number och the ADC used)
 * 
 * This should be defined in your application config.inc
 */
//#define ADCHAN ADC5


/*-----------------------------------------------
 * Functions
 * ---------------------------------------------*/
uint8_t LDR_SensorInit(void);
uint32_t LDR_GetData(uint8_t sensor);
  
#endif
#endif
