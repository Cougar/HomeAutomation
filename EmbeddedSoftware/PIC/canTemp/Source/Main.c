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
#include "..\canBoot\Include\boot.h"

#ifdef USE_ADC
	#include <adc.h>
#endif

#ifdef USE_BLINDS
	#include <blinds.h>
#endif

static void mainInit(void);

void main()
{
	// Inits
	mainInit();

	canInit();

	#ifdef USE_ADC
		adcInit(TRUE,0b1011);
	#endif

	#ifdef USE_BLINDS
		blindsInit();
		blindsTurn(0);
	#else
		BLINDS0P_IO=0;
	#endif


	while(1)
	{
		TICK currentTick = tickGet();

		static TICK tick_led = 0;
		static TICK tick_temperature = 0;
		static TICK tick_blinds = 0;	

		static BOOL getNewTemperature = TRUE;
		static BYTE lastTemperature = TEMPERATURE_INSIDE;
		

		if ((currentTick-tick_led)>(1*TICK_1S))
		{
			LED0_IO=~LED0_IO;
			tick_led = currentTick;
		}

		
		#ifdef USE_BLINDS
		if ((currentTick-tick_blinds)>(3*TICK_1S))
		{
			CAN_PACKET outCp;
			outCp.type=ptPGM;
			outCp.pgm.class=pcSENSOR;
			outCp.pgm.id=pstBLIND_TR;
			outCp.length=1;
			outCp.data[0]=blindsGetPrecent();
			while(!canSendMessage(outCp,PRIO_HIGH));

			tick_blinds = currentTick;			
		}
		#endif
		

		#ifdef USE_ADC
		if ((currentTick-tick_temperature)>(2*TICK_1S) && getNewTemperature==TRUE) // Each 2 second, read temperature.
		{
			if (lastTemperature==TEMPERATURE_INSIDE) lastTemperature=TEMPERATURE_OUTSIDE; else lastTemperature=TEMPERATURE_INSIDE;
			adcConvert(lastTemperature);
			getNewTemperature=FALSE;
			tick_temperature = currentTick;
		}
		#endif


		#ifdef USE_ADC
		if (adcDone()) // Has temperature, send it to the bus
		{
			CAN_PACKET outCp;
			BYTE decimal;
			signed char tenth;

			temperatureRead(&tenth,&decimal);
			
			outCp.type=ptPGM;
			outCp.pgm.class	=pcSENSOR;
			outCp.pgm.id	=(lastTemperature==TEMPERATURE_INSIDE?pstTEMP_INSIDE:pstTEMP_OUTSIDE);
			outCp.length	=2;
			outCp.data[1]	= tenth; //  signed 10 an 1 decimal.
			outCp.data[0]	= decimal; //  Decimal value, 0-9
			while(!canSendMessage(outCp,PRIO_HIGH));

			getNewTemperature=TRUE;
			tick_temperature = currentTick;
		}
		#endif
	
	} // end while
	
} // end main


#pragma interrupt high_isr
void high_isr(void)
{
	canISR();

	#ifdef USE_BLINDS
		blindsISR();
	#endif

	#ifdef USE_ADC
		adcISR();
	#endif
}


#pragma interrupt low_isr
void low_isr(void)
{

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
	BLINDS0_TRIS=0;
	BLINDS0P_TRIS=0;

	// Enable interrupt prirority
	RCONbits.IPEN=1;

	// Enable Interrupts
	INTCONbits.GIEL = 1;
    INTCONbits.GIEH = 1;
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

void canParse(CAN_PACKET cp)
{
	#ifdef USE_BLINDS
	if (cp.pgm.class==pcACTUATOR && cp.pgm.id==patBLIND_TR)
	{
		blindsTurn(cp.data[0]);
	}
	#endif
}


// Below are mandantory when using bootloader
// define DEBUG_MODE to use without bootloader.



#ifndef DEBUG_MODE
extern void _startup (void); 
#pragma code _RESET_INTERRUPT_VECTOR = RM_RESET_VECTOR
void _reset (void)
{
    _asm goto _startup _endasm
}
#pragma code
#endif

#pragma code _HIGH_INTERRUPT_VECTOR = RM_HIGH_INTERRUPT_VECTOR
void _high_ISR (void)
{
	_asm GOTO high_isr _endasm
}
#pragma code

#pragma code _LOW_INTERRUPT_VECTOR = RM_LOW_INTERRUPT_VECTOR
void _low_ISR (void)
{
    _asm GOTO low_isr _endasm
}
#pragma code
