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
#include <Tick.h>

#ifdef USE_CAN
	#include <CAN.h>
#endif

static void mainInit(void);

unsigned int heartcnt=0;

void main()
{
	static TICK t = 0;

	// Inits

	mainInit();

	#ifdef USE_CAN
		canInit();
	#endif

	tickInit();

	while(1)
	{
		#ifdef USE_CAN
		if ((tickGet()-t)>=5*TICK_SECOND)
		{
			CAN_MESSAGE cm;

			t = tickGet();
		
			// Send heartbeat
			cm.ident=0x00000666;
			cm.extended=FALSE;
			cm.data_length=5;
			cm.data[0]='H';
			cm.data[1]=(BYTE)((heartcnt & 0x000000FF));
			cm.data[2]=(BYTE)((heartcnt & 0x0000FF00)>>1);
			cm.data[3]=(BYTE)((heartcnt & 0x00FF0000)>>2);
			cm.data[4]=(BYTE)((heartcnt & 0xFF000000)>>3);

			heartcnt++;

			while(!canSendMessage(cm,PRIO_LOW));	
		}
		#endif
	}
}


#pragma interrupt high_isr
void high_isr(void)
{

	#ifdef USE_CAN
		canISR();
	#endif


	if (INTCONbits.INT0IE && INTCONbits.INT0IF)
	{
		static TICK t = 0;
		CAN_MESSAGE cm;

		if ((tickGet()-t)>TICK_SECOND/2)
		{

			LED0_IO=~LED0_IO;

			// Send heartbeat
			cm.ident=0x1F910091;
			cm.extended=TRUE;
			cm.remote_request=TRUE;
			cm.data_length=1;
			cm.data[0]=0x01;
			cm.data[1]=0x07;
			cm.data[2]=0x09;
			cm.data[3]=0x08;
			cm.data[4]=0x03;

			while(!canSendMessage(cm,PRIO_LOW));
			
			t=tickGet();
		}

		INTCONbits.INT0IF=0;
	}


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

    // Enable internal PORTB pull-ups
    INTCON2bits.RBPU = 0;
	
	// Enable Interrupts
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;


	// RB0 interrupt
	INTCON2bits.INTEDG0 = 1; //rising edge
	INTCONbits.INT0IE = 1;	

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
		if (cm.ident==0x00000304)
		{
			if (cm.data[0]==0x20) LED0_IO=1;
			if (cm.data[0]==0x40) LED0_IO=0;
			if (cm.data[0]==0x80) 
			{
				cm.ident=0x00000303;
				cm.extended=FALSE;
				cm.data_length=1;
				cm.data[0]=LED0_IO;
				while(!canSendMessage(cm,PRIO_LOW));	

			}
		}


}
#endif

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
