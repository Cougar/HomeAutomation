/*********************************************************************
 *
 *                  UART
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/

#include <uart.h>
#include <stackTasks.h>

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
		TXSTA=0b00100000;
		#ifdef HIGH_BRG
			TXSTAbits.BRGH=1; // High BRG speed
			BAUDCONbits.BRG16=1;
			SPBRGH = SPBRGH_VAL;
			SPBRG = SPBRG_VAL;
		#else
			TXSTAbits.BRGH=0;  // Low BRG speed
			SPBRG = SPBRG_VAL;
		#endif
		
		RCSTA=0b10010000;
		
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
		if (RCSTAbits.OERR) { RCSTAbits.CREN=0; RCSTAbits.CREN=1; }

		uartParse(uartGet());
		PIR1bits.RCIF=0;
	}
}


/*
*	Function: uartDataRedy
*
*	Input:	none.
*	Output: If usart data are redy to be read.
*	Pre-conditions: call to uartInit()
*	Affects: none
*	Depends: none
*/
char uartDataRedy(void)
{
	if(RCSTAbits.OERR)
	{
		RCSTAbits.CREN = 0;
		RCSTAbits.CREN = 1;
	}
  return PIR1bits.RCIF;
}

#endif
