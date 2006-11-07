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

#ifdef USE_UART
	#include <uart.h>
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
	#endif

	tickInit();

	

	while(1)
	{	
	}

}


#pragma interruptlow HighISR
void HighISR(void)
{


	#ifdef USE_UART
		uartISR();
	#endif

	#ifdef USE_CAN
		canISR();
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
	int i=0;
	
	uartPutc(UART_START_BYTE);
	uartPutc((BYTE)(cm.ident));
	uartPutc((BYTE)(cm.ident>>8));
	uartPutc((BYTE)(cm.ident>>16));
	uartPutc((BYTE)(cm.ident>>24));
	uartPutc(cm.extended);
	uartPutc(cm.remote_request);
	uartPutc(cm.data_length);
	for(i=0;i<8;i++) uartPutc(cm.data[i]);
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
	BYTE i;
	unsigned int wait = 0;
	CAN_MESSAGE cm;	
	static BOOL waitingMessage = FALSE;
	static TICK timeout=0;
	static BYTE count=0;

	if (waitingMessage==TRUE && (tickGet()-timeout)>TICK_SECOND/20)
	{
		waitingMessage=FALSE;
	}


	if (waitingMessage==TRUE)
	{

		timeout=tickGet();


		// UART END
		if(count>=15)
		{
			if (c==UART_END_BYTE)
			{
				LED0_IO=~LED0_IO;
				//canParse(cm);
				while(!canSendMessage(cm,PRIO_HIGEST));
			}
			waitingMessage=FALSE;
			return;
		}

		// data
		if(count>=7)
		{
			cm.data[count-7]=c;
			count++;
			return;
		}

		// data length
		if(count>=6)
		{
			cm.data_length=c;
			count++;
			return;
		}

		// remote request
		if(count>=5)
		{
			cm.remote_request=c;
			count++;
			return;
		}

		// extended
		if(count>=4)
		{
			cm.extended=c;
			count++;
			return;
		}

		// ident
		if(count>=0)
		{
			cm.ident+=((DWORD)c<<(count*8));
			count++;
			return;
		}
	
	}


	if (c==UART_START_BYTE && waitingMessage==FALSE)
	{
			waitingMessage=TRUE;
			timeout=tickGet();
			count=0;
			cm.ident=0;
			return;	
	}


}
#endif
