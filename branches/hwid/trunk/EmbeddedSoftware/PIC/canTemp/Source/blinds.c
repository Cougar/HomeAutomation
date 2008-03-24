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

static WORD timerVal = 50536;
static WORD turnOffIO = 0;
static BYTE currentPrecent = 0;
static BYTE dutyOffCounter = 0;

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
	TMR1H=(BYTE)((timerVal&0xFF00)>>8);
	TMR1L=(timerVal&0xFF);
	PIR1bits.TMR1IF = 0;
	PIE1bits.TMR1IE = 1;
	IPR1bits.TMR1IP = 1; // high prior
}

/*
*	Function: blindsGetAngle
*
*	Input:	none.
*	Output: angle.
*	Pre-conditions: none
*	Affects: none.
*	Depends: none.
*/
BYTE blindsGetPrecent()
{
	return currentPrecent;
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
void blindsTurn(BYTE precent)
{
	
	// 65536-TIME/0,0000001
	// -90 0.5 ms = 60536 (5000)
	// 0   1.5 ms = 50536 (15000)
	// 90  2.5 ms = 40536 (25000)

	BLINDS0P_IO=1;
	turnOffIO = IO_TIMEOUT;

	currentPrecent = precent;
	if (precent>100) { currentPrecent = 100; }
	if (precent<0)   { currentPrecent =   0; }
	dutyOffCounter = 0;

	timerVal=60536-(unsigned int)((90+(-1.05*(unsigned int)currentPrecent+60))*111.111);
	
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
		
		WORD timerValBuffer = timerVal;
		
		dutyOffCounter++;

		if (BLINDS0_IO==0 && dutyOffCounter>4)
		{
			BLINDS0_IO=1;
			dutyOffCounter=0;	
		}
		else if (BLINDS0_IO==0 && dutyOffCounter<=4)
		{
			timerValBuffer = DUTY_OFF_DELAY;
		}
		else if (BLINDS0_IO==1)
		{
			BLINDS0_IO=0;
		}
		
		TMR1H=(BYTE)((timerValBuffer&0xFF00)>>8);
		TMR1L=(timerValBuffer&0xFF);

		
		if (turnOffIO--<1)
		{
			BLINDS0P_IO=0;
		}
		

		PIR1bits.TMR1IF = 0;
	}
}

#endif
