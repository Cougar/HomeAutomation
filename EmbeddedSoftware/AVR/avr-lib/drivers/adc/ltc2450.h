/** 
 * @defgroup adc ADC driver
 * @code #include <drivers/adc/adc.h> @endcode
 * 
 * @brief Simple driver for external LTC2450 delta/sigma ADC.
 *
 * This is a slightly hard-coded driver that assumes an
 * LTC2450 ADC is connected via USART in SPI-mode, using
 * pins PD4, PD0 and CS on PD7. Later to be improved and
 * more generalized!
 *
 * @author	Jimmy Myhrman
 * @date	2010-05-17
 */

/**@{*/

#ifndef LTC2450_H_
#define LTC2450_H_

/*-----------------------------------------------
 * Definitions
 * ---------------------------------------------*/

// LTC2450 pins
#define SPI_PORT	PORTD
#define SPI_PIN		PIND
#define SPI_DDR		DDRD
#define SPI_SCK		PD4
#define SPI_MISO	PD0
#define SPI_CS		PD7

#define LTC2450_SELECT()   ( SPI_PORT &= ~(1<<SPI_CS) )
#define LTC2450_UNSELECT() ( SPI_PORT |=  (1<<SPI_CS) )


/*-----------------------------------------------
 * Inclusions
 * ---------------------------------------------*/
#include <stdlib.h>
#include <inttypes.h>


/*-----------------------------------------------
 * Function Prototypes
 * ---------------------------------------------*/

/**
 * @brief Initialize LTC2450 connection.
 * 
 * Initializes the USART module on AVR to be used in SPI-mode.
 * A single read operation is also performed in order to clear
 * out any old sample in the LTC2450 ADC.
 */ 
void ltc2450_init(void);

/**
 * @brief Reads the latest ADC value.
 * 
 * Reads the previously sampled value from the LTC2450 and
 * immediately starts a new conversion. Each conversion takes
 * approx 30ms. In case this function is called at a higher
 * frequency, the returned data will be 0xFFFF. This indicates
 * that ADC is busy, and user is responsible for reading until
 * valid sample has been retrieved.
 * 
 * @retval 
 * 		The latest sample. 0xFFFF in case ADC is busy.
 */ 
uint16_t ltc2450_read(void);
  
/**@}*/
#endif

