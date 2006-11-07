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


#pragma interruptlow HighISR
void HighISR(void)
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
#pragma code highVector=0x2008
void HighVector (void)
{
    _asm goto HighISR _endasm
}
#pragma code // Return to default code section



#pragma interruptlow LowISR
void LowISR(void)
{

}
#pragma code lowVector=0x2018
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

