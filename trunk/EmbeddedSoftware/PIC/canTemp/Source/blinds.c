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

static unsigned int timerVal = 50536;

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
	TMR1H=((timerVal&0xFF00)>>8);
	TMR1L=(timerVal&0xFF);
	PIR1bits.TMR1IF = 0;
	PIE1bits.TMR1IE = 1;
	IPR1bits.TMR1IP = 1; // high prior?
}

/*
*	Function: blindsTurn
*
*	Input:	Angle to turn to.
*	Output: none.
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
void blindsTurn(signed char angle)
{
	// 65536-TIME/0,0000001
	// -90 0.5 ms = 60536 (5000)
	// 0   1.5 ms = 50536 (15000)
	// 90  2.5 ms = 40536 (25000)

	if (angle<-90) timerVal=60536;
	else if (angle>90) timerVal=40536;
	else
	{
		timerVal=60536-(unsigned int)((90+(signed int)angle)*111.111);
	}
}


/*
*	Function: blindsISR
*
*	Input:	none.
*	Output: none.
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
void blindsISR(void)
{
	if (PIR1bits.TMR1IF == 1 && PIE1bits.TMR1IE == 1)
	{
		TMR1H=((timerVal&0xFF00)>>8);
		TMR1L=(timerVal&0xFF);

		BLINDS0_IO=~BLINDS0_IO;

		PIR1bits.TMR1IF = 0;
	}
}

#endif
