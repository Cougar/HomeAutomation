/** 
 * @defgroup pca95xx PCA95xx Library
 * @code #include <drivers/io/pca95xx.h> @endcode
 * 
 * @brief Functions for using a pca95xx I2C IO-expander. 
 *
 * ...
 *
 * @author	Anders Runeson
 * @date	2010-04-01
 */

/**@{*/

#ifndef PCA9XX_H_
#define PCA9XX_H_

#include <config.h>
#ifndef PCA9XX_NUM_CALLBACKS
#define PCA9XX_NUM_CALLBACKS 1
#endif

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <stdio.h>

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 * Public Types
 *---------------------------------------------------------------------------*/

/**
 * @brief Type of the callback function pointer
 */
typedef void (*pca9xxCallback_t)(uint8_t, uint8_t);

/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/

/**
 * @brief Stores a callback for an io interrupt. 
 * 
 * @param callback
 * 		Pointer to optional callback function on the form 
 * 		void callback(uint16_t inputs). The callback function is called with the
 * 		port status as the argument whenever the interrupt is thrown. It is called
 * 		from the interrupt handler and must be very short. 
 */ 
void Pca95xx_SetCallback(pca9xxCallback_t callback);

/**
 * @brief Poll the PCA95xx for the input status. 
 * 
 * Reads the input status of the PCA95xx and returns the data.
 * 
 * @retval
 * 		The input registers.
 */ 
uint16_t Pca95xx_GetInputs(void);

/**
 * @brief Sets the PCA95xx outputs. 
 * 
 * @param Outputs
 * 		The data to be set as outputs.
 * @param Mask
 * 		Passing a mask makes it possible to set one or more output without
 * 		affecting the other ports. Set bit to 1 to set the corresponding
 * 		port bit to the value of the Output parameter bit.
 */ 
void Pca95xx_SetOutputs(uint16_t Outputs, uint16_t Mask);

/**
 * @brief Sets the PCA95xx port direction. 
 * 
 * @param Direction
 * 		The data to be used for setting port direction.
 * @param Mask
 * 		Passing a mask makes it possible to set direction for one or more 
 * 		ports without affecting the others. Set bit to 1 to set the 
 * 		corresponding direction bit to the value of the Direction parameter 
 * 		bit.
 */ 
void Pca95xx_SetDirection(uint16_t Direction, uint16_t Mask);


/**@}*/
#endif /*PCA9XX_H_*/
