/*********************************************************************
 *
 *   Functions for reading ADC
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/
#include <adc.h>

#ifdef USE_ADC

static unsigned long TEMP_VALUE;
static unsigned int TEMP_COUNT;

BOOL TEMP_DONE = FALSE;

/*
*	Function: adcInit
*
*	Input:	True or false to use external voltage reference or not. portCfg is portconfiguration.
*	Output: none.
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
void adcInit(BOOL useExtRef,BYTE portCfg)
{
	// Setup ADC to use low interrupt prior and external ref or not.

	ADCON1 = 0b00000000 | ((1 & useExtRef)<<4) | (0xF & portCfg);
	ADCON0 = 0b00000000;
	ADCON2 = 0b10101011;
	//useExtRef
	ADCON0bits.ADON = 1;
	PIR1bits.ADIF = 0;
	PIE1bits.ADIE = 1;
	IPR1bits.ADIP = 0; // Low priority
}


/*
*	Function: adcISR
*
*	Input:	ADC interrupt rotune. Need to be called from LOW ISR.
*	Output: none.
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
void adcISR(void)
{
	if (PIE1bits.ADIE && PIR1bits.ADIF)
	{
		PIR1bits.ADIF = 0;

		TEMP_VALUE+=((int)ADRESH)<<8 | ADRESL;
		TEMP_COUNT++;

		if (TEMP_COUNT>=TEMP_SAMPLES)
		{
			TEMP_VALUE = TEMP_VALUE/TEMP_COUNT;
			TEMP_DONE = TRUE;
		}
		else
		{
			Delay10TCYx(100);
			ADCON0bits.GO = 1;
		}

	}
}


/*
*	Function: adcConvert
*
*	Input:	Channel to start convert. Between 0 and 15
*	Output: none.
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
BOOL adcConvert(BYTE channel)
{
	if (channel>15) return FALSE;

	TEMP_VALUE = 0;
	TEMP_COUNT = 0;

	ADCON0 = (channel << 2)  | (ADCON0 & 0b11000011);

	TEMP_DONE = FALSE;

	ADCON0bits.GO = 1;

	return TRUE;
}


/*
*	Function: adcRead
*
*	Input:	none.
*	Output: Raw read data.
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
int adcRead(void)
{
	TEMP_DONE=FALSE;
	return TEMP_VALUE;
}


/*
*	Function: temperatureRead
*
*	Input:	none.
*	Output: XX.Y C, where tenth is XX and decimal is Y
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
void temperatureRead(signed char *tenth, BYTE *decimal) 
{
	// assuming 2.5V ref and TC47
	signed int i = 0, v11,v01,tfloat;

	tfloat =((TEMP_VALUE * 39)>>4) - 499;
	
	v11 = tfloat/10;
	v01 = tfloat-v11*10;

	     if (v01>=0 && v01<=2) v01 = 0;
	else if (v01>=3 && v01<=7) v01 = 5;
	else  { v01 = 0; v11++;}

	tenth = (signed char *)v11;
	decimal = (BYTE *)v01;


		TEMP_DONE=FALSE;
}


/*
*	Function: adcDone
*
*	Input:	none.
*	Output: If adc convertion is done or not.
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
BOOL adcDone(void)
{
	return TEMP_DONE;
}

#endif
