/*
 * LDR, Light Dependent Resistor
 *
 * Using ADC driver for reading sensor value.
 *
 * Author: Erik Larsson, Anders Runeson
 * Date: 2007-04-26
 *
 */

/*-----------------------------------------------
 * Includes
 * ---------------------------------------------*/
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include "ldr.h"
#include <config.h>
#include <drivers/adc/adc.h>
/*----------------------------------------------
 * Functions
 * --------------------------------------------*/

uint8_t LDR_SensorInit(void)
{   
    return ADC_Init();
}

/*
 * Start reading sensor value
 * returns the lightvalue in promille (0.1% per bit)
 */
uint16_t LDR_GetData(uint8_t sensor)
{
	uint16_t ldr = ADC_Get(sensor);
	if (ldr>1000) {
		ldr=1000;
	}
    return ldr;
}
