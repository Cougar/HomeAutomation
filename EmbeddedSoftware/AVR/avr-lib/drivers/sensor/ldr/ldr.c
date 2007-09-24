/*
 * LDR, Light Dependent Resistor
 *
 * Using ADC for reading sensor value.
 *
 * Author: Erik Larsson
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
/*----------------------------------------------
 * Functions
 * --------------------------------------------*/

uint8_t LDR_SensorInit(void)
{
#if ADCHAN == 0
    /* Enable ADC0 */
    ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2)|(1<<MUX3));
#elif ADCHAN == 1
    /* Enable ADC1 */
    ADMUX |= (1<<MUX0);
    ADMUX &= ~((1<<MUX1)|(1<<MUX2)|(1<<MUX3));
#elif ADCHAN == 2
    /* Enable ADC2 */
    ADMUX |= (1<<MUX1)|;
    ADMUX &= ~((1<<MUX0)|(1<<MUX2)|(1<<MUX3));
#elif ADCHAN == 3
    /* Enable ADC3 */
    ADMUX |= (1<<MUX0)|(1<<MUX1);
    ADMUX &= ~((1<<MUX2)|(1<<MUX3));
#elif ADCHAN == 4
    /* Enable ADC4 */
    ADMUX |= (1<<MUX2);
    ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX3));
#elif ADCHAN == 5
    /* Enable ADC5 */
    ADMUX |= (1<<MUX0)|(1<<MUX2);
    ADMUX &= ~((1<<MUX1)|(1<<MUX3));
//#error yo helt fel
#elif ADCHAN == 6
    /* Enable ADC6 */
    ADMUX |= (1<<MUX1)|(1<<MUX2);
    ADMUX &= ~((1<<MUX0)|(1<<MUX3));
#elif ADCHAN == 7
    /* Enable ADC7 (only for TQFP package) */
    ADMUX |= (1<<MUX0)|(1<<MUX1)|(1<<MUX2);
    ADMUX &= ~(1<<MUX3);
#else
    #error No adc defined
#endif

    /* Enable AVcc as Voltage Reference */
    ADMUX |= (1<<REFS0);
    ADMUX &= ~(1<<REFS1);

    /* Right adjust the result */
    ADMUX &= ~(1<<ADLAR);

    /* Wake up ADC and enable it */
    PRR &= ~(1<<PRADC);
    ADCSRA |= (1<<ADEN);

    /* Make the first conversion (takes 25 ADC clock cycles) and throw away */
    LDR_GetData(0);
    
    return 0;
}

/*
 * Start reading sensor value
 */
uint32_t LDR_GetData(uint8_t sensor)
{
	uint32_t lightData;

	/* Start measurement (takes 13 ADC clock cycles) */
	ADCSRA |= (1<<ADSC);

	while( ADCSRA & (1<<ADSC) ){ /* Wait for conversion to complete */ }

	/* Get the result and return */
	lightData = ADCW; //>>2;

    return lightData;
	
}
