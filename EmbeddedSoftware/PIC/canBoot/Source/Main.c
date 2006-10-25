/*********************************************************************
 *
 *                  Main application
 *
 *********************************************************************
 * FileName:        Main.c
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Johan Böhlin     21-10-06 	Original        (Rev 1.0)
 ********************************************************************/

#include <compiler.h>
#include <stackTasks.h>

#ifdef USE_CAN
	#include <CAN.h>
#endif

static void mainInit(void);

void main()
{

	// Inits

	mainInit();

	#ifdef USE_CAN
		canInit();
	#endif


	while(1)
	{
	}
}


#pragma interruptlow HighISR
void HighISR(void)
{


	#ifdef USE_CAN
		canISR();
	#endif

}
#pragma code highVector=0x08
void HighVector (void)
{
    _asm goto HighISR _endasm
}
#pragma code // Return to default code section



#pragma interruptlow LowISR
void LowISR(void)
{

}
#pragma code lowVector=0x18
void LowVector (void)
{
    _asm goto LowISR _endasm
}
#pragma code // Return to default code section


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

    // Enable internal PORTB pull-ups
    INTCON2bits.RBPU = 0;
	
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
#ifdef USE_CAN
void canParse(CAN_MESSAGE cm)
{
	LED0_IO=~LED0_IO;
}
#endif

