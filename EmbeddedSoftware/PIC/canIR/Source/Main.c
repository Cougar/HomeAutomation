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


#ifdef USE_IREC
	#include <IRec.h>
#endif

static void mainInit(void);


void main()
{
	// Inits
	mainInit();
	canInit();

	#ifdef USE_IREC
		irecInit();
	#endif


	while(1)
	{	
		TICK currentTick = tickGet();

		static TICK tick_led = 0;


		if ((currentTick-tick_led)>(1*TICK_1S))
		{
			LED0_IO=~LED0_IO;
			tick_led = currentTick;
		}
	}
}


#pragma interrupt high_isr
void high_isr(void)
{
	canISR();

	#ifdef USE_IREC
		irecISR();
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



/*
*	Function: irecParse
*
*	Input:	Received IR message
*	Output: none
*	Pre-conditions: irecInit
*	Affects: Sensors/actuators/etc. See code.
*	Depends: none.
*/
#ifdef USE_IREC
void irecParse(IR_TYPE type, BYTE toggle, BYTE addr, BYTE data)
{
	CAN_PACKET outCp;

	outCp.type 		= ptPGM;
	outCp.pgm.class = pcSENSOR;
	outCp.pgm.id 	= pstIR;
	outCp.length 	= 4;
	outCp.data[3]	= type;
	outCp.data[2]	= toggle;
	outCp.data[1]	= addr;
	outCp.data[0]	= data;
	while(!canSendMessage(outCp,PRIO_HIGH));


}
#endif

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
