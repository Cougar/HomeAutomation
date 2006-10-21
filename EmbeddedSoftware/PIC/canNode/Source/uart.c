/*********************************************************************
 *
 *                  UART
 *
 *********************************************************************
 * FileName:        uart.c
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Johan Böhlin     21-10-06 	Original        (Rev 1.0)
 ********************************************************************/

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
*	Function: uartGets
*
*	Input:	Byte array to read, number of bytes to read and timeout.
*	Output: Number of read bytes.
*	Pre-conditions: call to uartInit()
*	Affects: none
*	Depends: none
*/
BYTE uartGets(BYTE *buffer, BYTE length,unsigned int uart_data_wait)
{
 	unsigned int wait = 0;
	BYTE data;

    while(length)
    {
        while(!uartDataRedy())
        {
            if(wait < uart_data_wait)
                wait++ ;                  /*wait for more data */
            else
                return(length);           /*Time out- Return words/bytes to be read */
        }
        wait=0;
        data = uartGet();
    	*buffer = data;
    	buffer++;              // Increment the string pointer
        length--;
    }

    return(length);                       /* number of data yet to be received i.e.,0 */






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
