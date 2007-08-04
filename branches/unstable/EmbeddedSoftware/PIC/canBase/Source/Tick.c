/*********************************************************************
 *
 *                  Ticker module service
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/

#define TICK_INCLUDE

#include <Tick.h>


static TICK TickCount = 0;	// 10ms/unit


/*
*	Function: TickInit
*
*	Input:	none
*	Output: Tick manager is initialized.
*	Pre-conditions: TickInit
*	Affects: none
*	Depends: none
*/

void tickInit(void)
{
    T0CON = 0b10000000 | TICK_PRESCALE;
    TMR0H = (BYTE)((TICK_COUNTER & 0xFF00)>>8);
    TMR0L = (TICK_COUNTER & 0xFF);

    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
	INTCON2bits.TMR0IP = 1;

	TickCount = 0;
}

/*
*	Function: TickGet
*
*	Input:	none
*	Output: Current tick value is given, 1 tick represents approximately 10ms
*	Pre-conditions: TickInit
*	Affects: none
*	Depends: none
*/
TICK tickGet(void)
{
    return TickCount;
}


/*
*	Function: tickUpdate
*
*	Input:	none
*	Output: none
*	Pre-conditions: TickInit
*	Affects: none
*	Depends: none
*/
void tickUpdate(void)
{
    if(INTCONbits.TMR0IF==1 && INTCONbits.TMR0IE==1)
    {
    	TMR0H = (BYTE)((TICK_COUNTER & 0xFF00)>>8);
    	TMR0L = (TICK_COUNTER & 0xFF);

        TickCount++;

        INTCONbits.TMR0IF = 0;
    }
}
