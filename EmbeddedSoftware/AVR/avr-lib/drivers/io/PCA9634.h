/** 
 * @defgroup io PWM driver
 * @code #include <drivers/io/PCA9634.h> @endcode
 * 
 * @brief Simple driver for PCA9634 PWM driver.
 *
 *
 * @author	Anders Runeson
 * @date	2011-02-13
 */

/**@{*/

#ifndef PCA9634_H_
#define PCA9634_H_

/*-----------------------------------------------
 * Definitions
 * ---------------------------------------------*/

typedef enum 
{
	PCA9634_OP_OFF		=	0,
	PCA9634_OP_ON		=	1,
	PCA9634_OP_IND		=	2,
	PCA9634_OP_INDGRP	=	3
} pca9634_OpMode_t;

#define PCA9634_MODE1_SLEEP (0<<4)
#define PCA9634_MODE1_SUB1 (0<<3)
#define PCA9634_MODE1_SUB2 (0<<2)
#define PCA9634_MODE1_SUB3 (0<<1)
#define PCA9634_MODE1_ALL (0<<0)

#define PCA9634_REG_MODE1		0
#define PCA9634_REG_MODE2		1
#define PCA9634_REG_PWM0		2
#define PCA9634_REG_PWM1		3
#define PCA9634_REG_PWM2		4
#define PCA9634_REG_PWM3		5
#define PCA9634_REG_PWM4		6
#define PCA9634_REG_PWM5		7
#define PCA9634_REG_PWM6		8
#define PCA9634_REG_PWM7		9
#define PCA9634_REG_GRPPWM		10
#define PCA9634_REG_GRPFREQ		11
#define PCA9634_REG_LEDOUT0		12
#define PCA9634_REG_LEDOUT1		13
#define PCA9634_REG_SUBADR1		14
#define PCA9634_REG_SUBADR2		15
#define PCA9634_REG_SUBADR3		16
#define PCA9634_REG_ALLCALLADR	17

/*-----------------------------------------------
 * Inclusions
 * ---------------------------------------------*/
#include <stdlib.h>
#include <inttypes.h>


/*-----------------------------------------------
 * Function Prototypes
 * ---------------------------------------------*/

/**
 * @brief Initialize PCA9634 connection.
 * 
 */ 
void pca9634_init(uint8_t address);

/**
 * @brief Sets the duty cycle for a PWM output.
 * 
 * 
 */ 
void pca9634_setDuty(uint8_t id, uint8_t step);

/**
 * @brief Sets the output mode for a PWM output.
 * 
 * 
 */ 
void pca9634_setOpMode(uint8_t id, pca9634_OpMode_t mode);
  
/**@}*/
#endif

