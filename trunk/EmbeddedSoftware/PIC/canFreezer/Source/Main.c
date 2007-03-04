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

static void mainInit(void);


void main()
{
	// Inits
	mainInit();
	canInit();

	#ifdef USE_ADC
		adcInit(TRUE,0b1011);
	#endif

	while(1)
	{
		TICK currentTick = tickGet();
		static TICK tick_led = 0;
		static TICK tick_temperature = 0;
		static TICK tick_FreezerDoor = 0;
		static TICK tick_RefrigeratorDoor = 0;
		static TICK bounce_FreezerDoor = 0;
		static TICK bounce_RefrigeratorDoor = 0;

		static BYTE lastTemperature = TEMPERATURE_FREEZER;
		static BYTE lastFreezerDoor = DOOR_CLOSED;
		static BYTE lastRefrigeratorDoor = DOOR_CLOSED;

		static BOOL getNewTemperature = TRUE;


		#ifdef USE_DOOR_SW
		if ((currentTick-bounce_FreezerDoor)>(3*TICK_100MS) && lastFreezerDoor != DOOR_FREEZER_IO)
		{
			lastFreezerDoor = DOOR_FREEZER_IO;
			tick_FreezerDoor = 0;
			bounce_FreezerDoor = currentTick;
		}

		if ((currentTick-bounce_RefrigeratorDoor)>(3*TICK_100MS) && lastRefrigeratorDoor != DOOR_REFRIGERATOR_IO)
		{
			lastRefrigeratorDoor = DOOR_REFRIGERATOR_IO;
			tick_RefrigeratorDoor = 0;
			bounce_RefrigeratorDoor = currentTick;
		}

		if ((currentTick-tick_FreezerDoor)>(3*TICK_1S))
		{
			CAN_PACKET outCp;	
			outCp.type 		= ptPGM;
			outCp.pgm.class = pcSENSOR;
			outCp.pgm.id 	= pstDOOR_FREEZER;
			outCp.length 	= 1;
			outCp.data[0]	= (DOOR_FREEZER_IO==DOOR_CLOSED?0:1);
			while(!canSendMessage(outCp,PRIO_HIGH));

			tick_FreezerDoor = currentTick;
		}

		if ((currentTick-tick_RefrigeratorDoor)>(3*TICK_1S))
		{
			CAN_PACKET outCp;	
			outCp.type 		= ptPGM;
			outCp.pgm.class = pcSENSOR;
			outCp.pgm.id 	= pstDOOR_REFRIGERATOR;
			outCp.length 	= 1;
			outCp.data[0]	= (DOOR_REFRIGERATOR_IO==DOOR_CLOSED?0:1);
			while(!canSendMessage(outCp,PRIO_HIGH));

			tick_RefrigeratorDoor = currentTick;
		}
		#endif

		if ((currentTick-tick_led)>TICK_1S)
		{
			LED0_IO=~LED0_IO;
			tick_led = currentTick;
		}

		#ifdef USE_ADC
		if ((currentTick-tick_temperature)>(2*TICK_1S) && getNewTemperature==TRUE) // Each 2 second, read temperature.
		{
			if (lastTemperature==TEMPERATURE_FREEZER) lastTemperature=TEMPERATURE_REFRIGERATOR; else lastTemperature=TEMPERATURE_FREEZER;
			adcConvert(lastTemperature);
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
			
			outCp.type 		= ptPGM;
			outCp.pgm.class = pcSENSOR;
			outCp.pgm.id 	= (lastTemperature==TEMPERATURE_FREEZER?pstTEMP_FREEZER:pstTEMP_REFRIGERATOR);
			outCp.length 	= 2;
			outCp.data[1]	= tenth; //  signed 10 an 1 decimal.
			outCp.data[0]	= decimal; //  Decimal value, 0-9
			while(!canSendMessage(outCp,PRIO_HIGH));
			
			tick_temperature = currentTick;
			getNewTemperature = TRUE;
		}
		#endif

	}
}


#pragma interrupt high_isr
void high_isr(void)
{
	canISR();
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

	DOOR_FREEZER_TRIS=1;
	DOOR_REFRIGERATOR_TRIS=1;

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
void canParse(CAN_PACKET cp)
{

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
