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
 * @author	Martin Norden
 * @date	2008-05-11
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
 * Functions
 * ---------------------------------------------*/

/**
 * @brief Initialize ADC.
 * 
 * Initializes the ADC-unit on the AVR. The voltage reference is set to Avcc. 
 * 
 */ 
uint8_t ADC_Init(void);

/**
 * @brief Get an ADC value.
 * 
 * Starts an ADC conversion and returns the result right adjusted, 10 bits.
 * If the channel is changed it does a dummy-read first to prevent garbage data.
 * 
 * @param channel
 * 		Channel to get ADC data from
 * @retval 
 * 		The ADC data
 */ 
uint16_t ADC_Get(uint8_t channel);
  
/**@}*/
#endif
