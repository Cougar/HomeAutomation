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

static BOOL heartBeatEnabled = TRUE;

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
	#endif

	tickInit();

	while(1)
	{
		if ((tickGet()-t)>=5*TICK_SECOND && heartBeatEnabled==TRUE)
		{
			CAN_MESSAGE cm;

			t = tickGet();
		
			// Send heartbeat
			cm.ident=0x00000666;
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
	
	uartPutc(UART_START_BYTE);
	uartPutc((BYTE)(cm.ident));
	uartPutc((BYTE)(cm.ident>>8));
	uartPutc((BYTE)(cm.ident>>16));
	uartPutc((BYTE)(cm.ident>>24));
	uartPutc(cm.extended);
	uartPutc(cm.data_length);
	for(i=0;i<8;i++) uartPutc(cm.data[cm.data_length-i-1]);
	uartPutc(UART_END_BYTE);

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
		cm.ident=0x00000123;
		cm.extended=FALSE;
		cm.data_length=4;
		cm.data[0]='!';
		cm.data[1]='j';
		cm.data[2]='e';
		cm.data[3]='H';

		while(!canSendMessage(cm));
	}

	if (c=='2')
	{
		CAN_MESSAGE cm;
		cm.ident=0x0ABCDEF0;
		cm.extended=TRUE;
		cm.data_length=8;
		cm.data[0]='!';
		cm.data[1]='g';
		cm.data[2]='a';
		cm.data[3]='d';
		cm.data[4]=' ';
		cm.data[5]='d';
		cm.data[6]='o';
		cm.data[7]='G';


		while(!canSendMessage(cm));
	}

	if (c=='3')
	{
		CAN_MESSAGE cm;
		cm.ident=0x00000010;
		cm.extended=FALSE;
		cm.data_length=6;
		cm.data[0]='0';
		cm.data[1]='1';
		cm.data[2]='2';
		cm.data[3]='3';
		cm.data[4]='4';
		cm.data[5]='5';

		while(!canSendMessage(cm));
	}

	if (c=='d')
	{
		//uartPutc('D');
	}

	if (c=='D')
	{
		//uartPutrs("Debug? Debug!");
	}

	if (c=='T')
	{
		heartBeatEnabled=(heartBeatEnabled==TRUE?FALSE:TRUE);
	}

	if (c=='4')
	{
		/*
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
		*/
	}

}
#endif
