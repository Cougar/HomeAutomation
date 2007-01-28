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

static void mainInit(void);


/*
	Features (support upto 8 dimmers).

	need to define funcc to each channel.
	need to define funcc to all in this node.

	channel = 0 to 7
	value = 0 to 255
	place = 1 to 10

	NO FADE
	set channel to value.
	set channel off.
	set channel last.
	set channel on.
	set all to value.
	set all off.
	set all last.
	set all on.

	FADE
	fade channel to value.
	fade channel off.
	fade channel last.
	fade channel on.
	fade all to value.
	fade all off.
	fade all last.
	fade all on.

	MEMORY:
	Save current values to M[place].
	Set current values to M[place].
	Fade current values to M[place].


	settings:
	buttons, default fadespeed, stepping. (Get and set)
*/



void main()
{
	static TICK t = 0;
	CAN_MESSAGE outCm;

	// Inits
	mainInit();

	canInit();


	while(1)
	{
		static TICK t = 0;

		if ((tickGet()-t)>TICK_SECOND)
		{
			LED0_IO=~LED0_IO;
			t = tickGet();
		}
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
