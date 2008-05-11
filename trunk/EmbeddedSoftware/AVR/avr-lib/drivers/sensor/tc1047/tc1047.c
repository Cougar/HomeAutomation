/*
 * TC1047 temperature sensor
 *
 * Using ADC for reading sensor value.
 *
 * Author: Erik Larsson
 * Date: 2006-12-19
 *
 */

/*-----------------------------------------------
 * Includes
 * ---------------------------------------------*/
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <config.h>

#include "tc1047.h"
#include <drivers/adc/adc.h>

/*----------------------------------------------
 * Functions
 * --------------------------------------------*/

/* 
 * Initiate ADC for reading temperature from sensor.
 */
uint8_t adcTemperatureInit()
{
    return ADC_Init();
}

/*
 * Start reading and return the temperature value.
 */
uint16_t getTC1047temperature(uint8_t sensor)
{
	uint32_t temperatureData;

	temperatureData = ADC_Get(sensor);

	/* Use 5 volt as reference */
	temperatureData = temperatureData * 5;
	temperatureData = (temperatureData * 100 / 1024) - 50;

	return temperatureData;
}
