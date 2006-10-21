#include "../Include/stackTasks.h"
#include "../Include/uart.h"

#ifdef USE_UART

static char uartBusy(void);

/*
*	Function: uartInit
*
*	Input:	none
*	Output: none
*	Pre-conditions: none
*	Affects: none
*	Depends: none
*/
void uartInit()
{
		// Configure USART
		TRISCbits.TRISC7=1;
		TRISCbits.TRISC6=0;
		TXSTA=0b00100000; // Low BRG speed
		RCSTA=0b10010000;
		SPBRG = SPBRG_VAL;
		// USART RX interrupt
		PIR1bits.RCIF=0;
		PIE1bits.RCIE=1;
		// High interrupt
		IPR1bits.RCIP=1;
}


/*
*	Function: uartPutc
*
*	Input:	BYTE c to send on uart
*	Output: none
*	Pre-conditions: call to uartInit()
*	Affects: none
*	Depends: none
*/
void uartPutc(BYTE c)
{
	// Write the data byte to the USART
	TXREG = c;      
	while(uartBusy());
}


/*
*	Function: uartPuts
*
*	Input:	Byte array to send on uart
*	Output: none
*	Pre-conditions: call to uartInit()
*	Affects: none
*	Depends: none
*/
void uartPuts(BYTE c[])
{
	do
  	{  // Transmit a byte
    	while(uartBusy());
    	uartPutc(*c);
  	} while( *c++ );
}

/*
*	Function: uartPutrs
*
*	Input:	Byte array to send on uart (static)
*	Output: none
*	Pre-conditions: call to uartInit()
*	Affects: none
*	Depends: none
*/
void uartPutrs(const rom char *c)
{
	do
  	{  // Transmit a byte
    	while(uartBusy());
    	uartPutc(*c);
  	} while( *c++ );
}




/*
*	Function: uartBusy
*
*	Input:	none.
*	Output: If usart busy or not.
*	Pre-conditions: call to uartInit()
*	Affects: none
*	Depends: none
*/
char uartBusy(void)
{
	return !TXSTAbits.TRMT;
}


/*
*	Function: uartGet
*
*	Input:	none
*	Output: Byte read
*	Pre-conditions: call to uartInit()
*	Affects: none
*	Depends: none.
*/
BYTE uartGet(void)
{
	// Return the received data
	return RCREG;	
}


/*
*	Function: uartISR
*
*	Input:	none
*	Output: none
*	Pre-conditions: call to uartInit()
*	Affects: call to uartParse
*	Depends: none.
*/
void uartISR(void)
{
	if (PIR1bits.RCIF==1 && PIE1bits.RCIE==1)
	{
		uartParse(uartGet());
		PIR1bits.RCIF=0;
	}
}


#endif
