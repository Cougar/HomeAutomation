
/*-----------------------------------------------
 * Includes
 * ---------------------------------------------*/
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include <config.h>
#include "adc.h"

#define BV(x) (1<<(x))

/*----------------------------------------------
 * Functions
 * --------------------------------------------*/
 
 static uint8_t currentChannel; //Used to keep the channel of the last measurement. This is used to determine if the channel needs to be changed.

uint8_t ADC_Init(void)
{
    /* Enable AVcc as Voltage Reference */
    ADMUX |= (1<<REFS0);
    ADMUX &= ~(1<<REFS1);

    /* Right adjust the result */
    ADMUX &= ~(1<<ADLAR);

    /* Wake up ADC and enable it */
    PRR &= ~(1<<PRADC);
    ADCSRA |= (1<<ADEN);

    /* Make sure that the next ADC_Get will do an empty measurement first by setting the channel to an impossible value */
    currentChannel = 10; 
    
    return 0;
}

/*
 * Start reading adc value.
 */
uint16_t ADC_Get(uint8_t channel)
{
	uint16_t adcdata;
	
	if (currentChannel != channel) //Did we use this channel the last time?
	{
		/* Set to 0 all mux registers */
		ADMUX &= ~( BV(MUX3) | BV(MUX2) | BV(MUX1) | BV(MUX0) );
		/* Select channel, only first 8 channel modes are supported for now */
		ADMUX |= (channel & 0x07);
		
		currentChannel = channel;	//Update the channel for the last measurement to the one we want this time
		ADC_Get(channel);	//And do a dummy-read
	}

	/* Start measurement (takes 13 ADC clock cycles) */
	ADCSRA |= (1<<ADSC);

	while( ADCSRA & (1<<ADSC) ){ /* Wait for conversion to complete */ }

	/* Get the result and return */
	adcdata = ADCW; //>>2;

    return adcdata;
	
}
