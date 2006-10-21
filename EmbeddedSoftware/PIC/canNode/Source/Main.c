#include "../Include/compiler.h"
#include "../Include/stackTasks.h"
#include "../Include/Tick.h"

#ifdef USE_CAN
	#include "../Include/CAN.h"
#endif

#ifdef USE_UART
	#include "../Include/uart.h"
#endif



static void mainInit(void);

void main()
{
	static TICK t = 0;

	// Inits

	mainInit();

	#ifdef USE_CAN
		canInit();
	#endif

	#ifdef USE_UART
		uartInit();
		uartPutrs("BOOT OK!");
	#endif

	tickInit();

	while(1)
	{
		if ((tickGet()-t)>=10*TICK_SECOND)
		{
			CAN_MESSAGE cm;

			t = tickGet();
		
			// Send heartbeat
			cm.ident=0x00000707;
			cm.extended=FALSE;
			cm.data_length=1;
			cm.data[0]='H';

			while(!canSendMessage(cm));

			
		}
	}
}


#pragma interruptlow HighISR
void HighISR(void)
{
	#ifdef USE_CAN
		canISR();
	#endif

	#ifdef USE_UART
		uartISR();
	#endif

	tickUpdate();
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
	int i=0;

	if (cm.extended==TRUE)
	{
		uartPutc((BYTE)(cm.ident>>24));
		uartPutc((BYTE)(cm.ident>>16));
	}
	uartPutc((BYTE)(cm.ident>>8));
	uartPutc((BYTE)(cm.ident));

	for(i=0;i<cm.data_length && i<8;i++)
		uartPutc(cm.data[cm.data_length-i-1]);

}
#endif


/*
*	Function: uartParse
*
*	Input:	Received uart message
*	Output: none
*	Pre-conditions: uartInit and received byte.
*	Affects: Sensors/actuators/etc. See code.
*	Depends: none.
*/
#ifdef USE_UART
void uartParse(BYTE c)
{


	if (c=='1')
	{
		CAN_MESSAGE cm;
		cm.ident=0x1FFF07FF;
		cm.extended=TRUE;
		cm.data_length=4;
		cm.data[0]='a';
		cm.data[1]='b';
		cm.data[2]='c';
		cm.data[3]='d';

		while(!canSendMessage(cm));
	}

	if (c=='d')
	{
		uartPutc('D');
	}

	if (c=='D')
	{
		uartPutrs("Debug? Debug!");
	}

	if (c=='2')
	{
		uartPutc(RXB0CON);
		uartPutc(RXB1CON);
		uartPutc(123);
		uartPutc(B0CON);
		uartPutc(B1CON);
		uartPutc(B2CON);
		uartPutc(123);	
		uartPutc(B3CON);	
		uartPutc(B4CON);	
		uartPutc(B5CON);
		uartPutc(123);	
		uartPutc(CANSTAT);	
		uartPutc(COMSTAT);	
		uartPutc(ECANCON);
		uartPutc(123);	
		uartPutc(TXB0CON);	
		uartPutc(TXB1CON);	
		uartPutc(TXB2CON);	
		uartPutc(123);
		uartPutc(PIR3);	
		uartPutc(PIE3);
		uartPutc(123);
		uartPutc(BRGCON1);
		uartPutc(BRGCON2);
		uartPutc(BRGCON3);
		uartPutc(123);
		uartPutc(RXFCON0);
		uartPutc(RXFCON1);
		uartPutc(123);
		uartPutc(RXFBCON0);
		uartPutc(RXFBCON1);
		uartPutc(RXFBCON2);
		uartPutc(RXFBCON3);
		uartPutc(RXFBCON4);
		uartPutc(RXFBCON5);
		uartPutc(RXFBCON6);
		uartPutc(RXFBCON7);
		uartPutc(123);
		uartPutc(TXBIE);
		uartPutc(BIE0);	
	}

}
#endif
