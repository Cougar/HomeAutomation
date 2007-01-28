/*********************************************************************
 *
 *                  Main application
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/


#include <compiler.h>
#include <stackTasks.h>
#include <Tick.h>
#include <funcdefs.h>
#include <CAN.h>


#ifdef USE_ADC
	#include <adc.h>
#endif

static void mainInit(void);


void main()
{
	static TICK t = 0;
	CAN_MESSAGE outCm;

	// Inits
	mainInit();
	canInit();

	#ifdef USE_ADC
		adcInit(TRUE,0b1011);
	#endif


	while(1)
	{
		static TICK t = 0;
		static TICK temperature = 0;
		static BYTE lastTemperature = TEMPERATURE_INSIDE;

		
		if ((tickGet()-t)>TICK_SECOND)
		{
			LED0_IO=~LED0_IO;
			t = tickGet();
		}

		#ifdef USE_ADC
		if ((tickGet()-temperature)>TICK_SECOND*2) // Each 2 second, read temperature.
		{
			if (lastTemperature==TEMPERATURE_INSIDE) lastTemperature=TEMPERATURE_OUTSIDE; else lastTemperature=TEMPERATURE_INSIDE;
			adcConvert(lastTemperature);
			temperature = tickGet();
		}
		#endif


		#ifdef USE_ADC
		if (adcDone()) // Has temperature, send it to the bus
		{
			BYTE decimal;
			signed char tenth;
			temperatureRead(&tenth,&decimal);

			outCm.funct 					= FUNCT_SENSORS;
			outCm.funcc 					= (lastTemperature==TEMPERATURE_INSIDE?FUNCC_SENSORS_TEMPERATURE_INSIDE:FUNCC_SENSORS_TEMPERATURE_OUTSIDE);
			outCm.data_length 				= 2;
			outCm.data[1]					= tenth; //  signed 10 an 1 decimal.
			outCm.data[0]					= decimal; //  Decimal value, 0-9
			while(!canSendMessage(outCm,PRIO_HIGH));
		}
		#endif

	}
}


#pragma interrupt high_isr
void high_isr(void)
{
	canISR();
}


#pragma interrupt low_isr
void low_isr(void)
{
	#ifdef USE_ADC
		adcISR();
	#endif
}



/*
*	Function: mainInit
*
*	Input:	none
*	Output: none
*	Pre-conditions: none
*	Affects: Se code
*	Depends: none.
*/
void mainInit()
{
	LED0_TRIS=0;	

	// Enable Interrupts
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;

	// Enable interrupt prirority
	RCONbits.IPEN=1;
	

}

/*
*	Function: canParse
*
*	Input:	Received can message
*	Output: none
*	Pre-conditions: canInit and received packet.
*	Affects: Sensors/actuators/etc. See code.
*	Depends: none.
*/

void canParse(CAN_MESSAGE cm)
{
	CAN_MESSAGE outCm;

}


// Below are mandantory when using bootloader
// define DEBUG_MODE to use without bootloader.

#ifndef DEBUG_MODE
extern void _startup (void); 
#pragma code _RESET_INTERRUPT_VECTOR = 0x001000
void _reset (void)
{
    _asm goto _startup _endasm
}
#pragma code
#endif

#pragma code _HIGH_INTERRUPT_VECTOR = 0x001008
void _high_ISR (void)
{
	_asm GOTO high_isr _endasm
}
#pragma code

#pragma code _LOW_INTERRUPT_VECTOR = 0x001018
void _low_ISR (void)
{
    _asm GOTO low_isr _endasm
}
#pragma code
