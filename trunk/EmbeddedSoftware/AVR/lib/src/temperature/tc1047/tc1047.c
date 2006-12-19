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

#include <tc1047.h>


/*----------------------------------------------
 * Functions
 * --------------------------------------------*/

/* 
 * Initiate ADC for reading temperature from sensor.
 */
void adcTemperatureInit()
{
#if PDIP
    /* Enable ADC0 (for PDIP package) */
    ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2)|(1<<MUX3));
#else
    /* Enable ADC7 (for TQFP package) */
    ADMUX |= (1<<MUX0)|(1<<MUX1)|(1<<MUX2);
    ADMUX &= ~(1<<MUX3);
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
    getTC1047temperature();
}

/*
 * Start reading and return the temperature value.
 */
uint32_t getTC1047temperature()
{
    uint32_t temperatureData;

    /* Start measurement (takes 13 ADC clock cycles) */
    ADCSRA |= (1<<ADSC);

    while( ADCSRA & (1<<ADSC) ){ /* Wait for conversion to complete */ }

    /* Get the result, convert and return */
    temperatureData = ADCW;

    temperatureData = temperatureData * VREF;
    temperatureData = (temperatureData * 100 / 1024) - 50;

    return temperatureData;
}
