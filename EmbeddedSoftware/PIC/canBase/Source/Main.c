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
#include <EEaccess.h>

#ifdef USE_CAN
	#include <CAN.h>
#endif

static void mainInit(void);


void main()
{
	static TICK t = 0;
	CAN_MESSAGE cm2;

	// Inits

	mainInit();

	#ifdef USE_CAN
		canInit();
	#endif

	tickInit();

	// Dummy bootmessage
	cm2.data[0]=0x91;
	cm2.data_length=1;
	canSendMessage(cm2,PRIO_HIGH);


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
	ClrWdt();

	#ifdef USE_CAN
		canISR();
	#endif

	tickUpdate();

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
#ifdef USE_CAN
void canParse(CAN_MESSAGE cm)
{
	ClrWdt();

	switch(cm.funct)
	{
		case FUNCT_BOOTLOADER:
			if (cm.funcc==FUNCC_BOOT_INIT) { Reset(); }
		break;
	}
}
#endif


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
