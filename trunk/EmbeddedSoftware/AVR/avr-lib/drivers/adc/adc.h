/** 
 * @defgroup adc ADC driver
 * @code #include <drivers/adc/adc.h> @endcode
 * 
 * @brief Simple low level ADC driver. 
 *
 * This driver is a simple ADC implementation.
 * 
 *
 * @author	Erik Larsson
 * @author	Anders Runeson
 * @date	2007-04-26
 */

/**@{*/

#ifndef ADC_H_
#define ADC_H_

#if !defined(__AVR_ATmega88__) && !defined(__AVR_ATmega168__)
#error only tested on ATmega88/168
#endif

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

/**
 * @brief Initialize ADC.
 * 
 * Initializes the ADC channel specified by ADCHAN, currently this driver 
 * have the ability to enable only one ADC channel. The voltage reference 
 * is set to Avcc. 
 * 
 */ 
uint8_t ADC_Init(void);

/**
 * @brief Get an ADC value.
 * 
 * Starts an ADC conversion and returns the result right adjusted, 10 bits.
 * 
  * @param channel
 * 		Channel to get ADC data from, currently not implemented.
 * @retval 
 * 		The ADC data
 */ 
uint16_t ADC_Get(uint8_t channel);
  
/**@}*/
#endif
