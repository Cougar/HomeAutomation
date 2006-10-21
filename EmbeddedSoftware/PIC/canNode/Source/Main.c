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

static BOOL heartBeatEnabled = FALSE;

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
	BYTE i;
	unsigned int wait = 0;
	CAN_MESSAGE cm;	
	static BOOL waitingMessage = FALSE;
	static TICK timeout;
	static BYTE count;

	if (waitingMessage==TRUE && (tickGet()-timeout)>TICK_SECOND/20)
	{
		waitingMessage=FALSE;
	}

	if (waitingMessage==TRUE)
	{
		timeout=tickGet();

		// UART END
		if(count>=14)
		{
			if (c==UART_END_BYTE)
			{
				while(!canSendMessage(cm));
			}
			waitingMessage=FALSE;
			return;
		}

		// data
		if(count>=6)
		{
			cm.data[count-6]=c;
			count++;
			return;
		}

		// data length
		if(count>=5)
		{
			cm.data_length=c;
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
	


/*			
			for(i=0;i<4;i++)
			{
				wait = 0;
				while(!uartDataRedy())
        		{
            		if(wait < UART_READ_TIMEOUT) wait++;                 
            		else return;           
        		}
				cm.ident += uartGet()<<(i*8);
			}


			wait = 0;
			while(!uartDataRedy())
        	{
            	if(wait < UART_READ_TIMEOUT) wait++ ;                 
            	else return;           
        	}
			cm.extended= uartGet();



			wait = 0;
			while(!uartDataRedy())
        	{
            	if(wait < UART_READ_TIMEOUT) wait++ ;                 
            	else return;           
        	}
			cm.data_length= uartGet();

			for(i=0;i<4;i++)
			{
				wait = 0;
				while(!uartDataRedy())
        		{
            		if(wait < UART_READ_TIMEOUT) wait++ ;                 
            		else return;           
        		}
				cm.data[i]=uartGet();
			}

			wait = 0;
			while(!uartDataRedy())
        	{
            	if(wait < UART_READ_TIMEOUT) wait++ ;                 
            	else return;           
        	}
			if (uartGet()!=UART_END_BYTE) return;
*/
			/*	
			if (uartGets(&cm.ident,4,UART_READ_TIMEOUT)) return;
			if (uartGets(&cm.extended,1,UART_READ_TIMEOUT)) return;
			if (uartGets(&cm.data_length,1,UART_READ_TIMEOUT)) return;
			if (uartGets(cm.data,8,UART_READ_TIMEOUT)) return;
			if (uartGets(&i,1,UART_READ_TIMEOUT)) return;
			if (i!=UART_END_BYTE) return;		
			*/
		//	while(!canSendMessage(cm));
//	}


	
	if(c=='1' && waitingMessage==FALSE) 
	{	
			cm.ident=0x00000123;
			cm.extended=FALSE;
			cm.data_length=4;
			cm.data[0]='!';
			cm.data[1]='j';
			cm.data[2]='e';
			cm.data[3]='H';

			while(!canSendMessage(cm));
	}

	if(c=='2' && waitingMessage==FALSE)  
	{
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

	if(c=='3' && waitingMessage==FALSE) 
	{
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

	

}
#endif
