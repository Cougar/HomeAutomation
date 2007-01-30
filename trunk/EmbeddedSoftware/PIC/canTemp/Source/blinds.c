/*********************************************************************
 *
 *   Functions for controlling blinds
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/
#include <blinds.h>

#ifdef USE_BLINDS

typedef enum {OPEN=2,CLOSE=4} MODE;

MODE mode = OPEN;
WORD numSteps = 0;

BYTE minCounter = 0;

/*
*	Function: blindsInit
*
*	Input:	none.
*	Output: none.
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
void blindsInit()
{
	// setup timer one on 0.5ms.
	// 16 bit write, system clock, prescale 2 bit, oscillator off, 
	T1CON = 0b11000001;
	TMR1H=((TMR1_VAL&0xFF00)>>8);
	TMR1L=(TMR1_VAL&0xFF);
	PIR1bits.TMR1IF = 0;
	PIE1bits.TMR1IE = 1;
	IPR1bits.TMR1IP = 1; // high prior?
}

/*
*	Function: blindsClose
*
*	Input:	Number of steps to perform. 255 equals full close.
*	Output: none.
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
void blindsClose(WORD steps)
{
	mode = CLOSE;
	minCounter = 0;
	numSteps = steps;
}

/*
*	Function: blindsClose
*
*	Input:	Number of steps to perform. 255 equals full open.
*	Output: none.
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
void blindsOpen(WORD steps)
{
	mode = OPEN;
	minCounter = 0;
	numSteps = steps;
}

/*
*	Function: blindsStop
*
*	Input:	Stop blind at current position.
*	Output: none.
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
void blindsStop()
{
	numSteps = 0;
}

/*
*	Function: blindsStop
*
*	Input:	Stop blind at current position.
*	Output: none.
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
void blindsISR(void)
{
	if (PIR1bits.TMR1IF == 1 && PIE1bits.TMR1IE == 1)
	{
		TMR1H=((TMR1_VAL&0xFF00)>>8);
		TMR1L=(TMR1_VAL&0xFF);


		
		if (numSteps>1)
		{
			if ((--minCounter)<1)
			{
				numSteps--;
				BLINDS0_IO=~BLINDS0_IO;
				minCounter=mode;
			}	
		}

		PIR1bits.TMR1IF = 0;
	}
}

#endif
