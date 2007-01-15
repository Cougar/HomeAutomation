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
#ifdef ADC0
    /* Enable ADC0 */
    ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX2)|(1<<MUX3));
#elif ADC1
    /* Enable ADC1 */
    ADMUX |= (1<<MUX0);
    ADMUX &= ~((1<<MUX1)|(1<<MUX2)|(1<<MUX3));
#elif ADC2
    /* Enable ADC2 */
    ADMUX |= (1<<MUX1)|;
    ADMUX &= ~((1<<MUX0)|(1<<MUX2)|(1<<MUX3));
#elif ADC3
    /* Enable ADC3 */
    ADMUX |= (1<<MUX0)|(1<<MUX1);
    ADMUX &= ~((1<<MUX2)|(1<<MUX3));
#elif ADC4
    /* Enable ADC4 */
    ADMUX |= (1<<MUX2);
    ADMUX &= ~((1<<MUX0)|(1<<MUX1)|(1<<MUX3));
#elif ADC5
    /* Enable ADC5 */
    ADMUX |= (1<<MUX0)(1<<MUX2);
    ADMUX &= ~((1<<MUX0)|(1<<MUX3));
#elif ADC6
    /* Enable ADC6 */
    ADMUX |= (1<<MUX1)|(1<<MUX2);
    ADMUX &= ~((1<<MUX0)|(1<<MUX3));
#elif ADC7
    /* Enable ADC7 (only for TQFP package) */
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

#if VREF_INT
    /* If VREF_INT 1 use 1.1 volt as reference */
    temperatureData = temperatureData * 11;
    temperatureData = (temperatureData * 10 / 1024) - 50;
#else
    /* If VREF_INT 0 use 5 volt as reference */
    temperatureData = temperatureData * 5;
    temperatureData = (temperatureData * 100 / 1024) - 50;
#endif
    return temperatureData;
}
