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

#ifndef PCA95XX_H_
#define PCA95XX_H_

#include <config.h>

#ifndef PCA95XX_NUM_CALLBACKS
#define PCA95XX_NUM_CALLBACKS 0
#endif

#ifndef PCA_INT_VECTOR
#define PCA_INT_VECTOR	INT0_vect
#endif


#if defined(__AVR_ATmega8__)
	#define INT1_REG GICR
#elif defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
	#define INT1_REG EIMSK
#else
#error AVR device not supported!
#endif

#if PCA_INT_VECTOR==INT0_vect
	#define PCA_INT_ENABLE()	INT1_REG |= _BV(INT0)
	#define PCA_INT_DISABLE()	INT1_REG &= ~(_BV(INT0))
#else
	#define PCA_INT_ENABLE()	INT1_REG |= _BV(INT1)
	#define PCA_INT_DISABLE()	INT1_REG &= ~(_BV(INT1))
#endif


/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <stdio.h>
#include <drivers/mcu/TWI_Master.h>

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define PCA95xx_REG_INPUT0	0
#define PCA95xx_REG_INPUT1	1
#define PCA95xx_REG_OUTPUT0	2
#define PCA95xx_REG_OUTPUT1	3
#define PCA95xx_REG_POLINV0	4
#define PCA95xx_REG_POLINV1	5
#define PCA95xx_REG_CONF0	6
#define PCA95xx_REG_CONF1	7

#define PCA95xx_I2C_DEV_ADDR	0x40

/*-----------------------------------------------------------------------------
 * Public Types
 *---------------------------------------------------------------------------*/

/**
 * @brief Type of the callback function pointer
 */
typedef void (*pca95xxCallback_t)(uint16_t);

/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/

/**
 * @brief Poll the PCA95xx for the input status. 
 * 
 * Reads the input status of the PCA95xx and returns the data.
 * 
 * @param address
 * 		The address used when communicating with the device.
 */ 
void Pca95xx_Init(uint8_t address);

/**
 * @brief Stores a callback for an io interrupt. 
 * 
 * @param callback
 * 		Pointer to optional callback function on the form 
 * 		void callback(uint16_t inputs). The callback function is called with the
 * 		port status as the argument whenever the interrupt is thrown. It is called
 * 		from the interrupt handler and must be very short. 
 */ 
void Pca95xx_SetCallback(pca95xxCallback_t callback);

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
 * @param outputs
 * 		The data to be set as outputs.
 * @param mask
 * 		Passing a mask makes it possible to set one or more output without
 * 		affecting the other ports. Set bit to 1 to set the corresponding
 * 		port bit to the value of the Output parameter bit.
 */ 
void Pca95xx_SetOutputs(uint16_t outputs, uint16_t mask);

/**
 * @brief Sets the PCA95xx port direction. 
 * 
 * @param direction
 * 		The data to be used for setting port direction.
 * @param mask
 * 		Passing a mask makes it possible to set direction for one or more 
 * 		ports without affecting the others. Set bit to 1 to set the 
 * 		corresponding direction bit to the value of the Direction parameter 
 * 		bit.
 */ 
void Pca95xx_SetDirection(uint16_t direction, uint16_t mask);


/**@}*/
#endif /*PCA95XX_H_*/
