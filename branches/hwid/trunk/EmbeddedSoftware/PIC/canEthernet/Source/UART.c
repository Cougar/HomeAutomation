/*********************************************************************
 *
 *     UART access routines for C18 and C30
 *
 *********************************************************************
 * FileName:        UART.c
 * Dependencies:    UART.h
 * Processor:       PIC18, PIC24F/H, dsPIC30F, dsPIC33F
 * Complier:        Microchip C18 v3.03 or higher
 * Complier:        Microchip C30 v2.01 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * This software is owned by Microchip Technology Inc. ("Microchip") 
 * and is supplied to you for use exclusively as described in the 
 * associated software agreement.  This software is protected by 
 * software and other intellectual property laws.  Any use in 
 * violation of the software license may subject the user to criminal 
 * sanctions as well as civil liability.  Copyright 2006 Microchip
 * Technology Inc.  All rights reserved.
 *
 * This software is provided "AS IS."  MICROCHIP DISCLAIMS ALL 
 * WARRANTIES, EXPRESS, IMPLIED, STATUTORY OR OTHERWISE, NOT LIMITED 
 * TO MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
 * INFRINGEMENT.  Microchip shall in no event be liable for special, 
 * incidental, or consequential damages.
 *
 *
 * Author               Date   		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder		4/04/06		Copied from dsPIC30 libraries
 * Howard Schlunder		6/16/06		Added PIC18
********************************************************************/
#define THIS_IS_UART

#include "..\Include\Compiler.h"
#include "..\Include\StackTsk.h"
#include "..\Include\UART.h"

#if 1//defined(STACK_USE_UART)

BYTE ReadStringUART(BYTE *Dest, BYTE BufferLen)
{
	BYTE c;
	BYTE count = 0;

	while(BufferLen--)
	{
		*Dest = '\0';

		while(!DataRdyUART());
		c = ReadUART();

		if(c == '\r' || c == '\n')
			break;

		count++;
		*Dest++ = c;
	}

	return count;
}


#if defined(__dsPIC33F__) || defined(__dsPIC30F__) || defined(__PIC24H__) || defined(__PIC24F__)


/***************************************************************************
* Function Name     : putsUART2                                            *
* Description       : This function puts the data string to be transmitted *
*                     into the transmit buffer (till NULL character)       * 
* Parameters        : unsigned int * address of the string buffer to be    *
*                     transmitted                                          *
* Return Value      : None                                                 *  
***************************************************************************/

void putsUART2(unsigned int *buffer)
{
    char * temp_ptr = (char *) buffer;

    /* transmit till NULL character is encountered */

    if(U2MODEbits.PDSEL == 3)        /* check if TX is 8bits or 9bits */
    {
        while(*buffer != '\0') 
        {
            while(U2STAbits.UTXBF); /* wait if the buffer is full */
            U2TXREG = *buffer++;    /* transfer data word to TX reg */
        }
    }
    else
    {
        while(*temp_ptr != '\0')
        {
            while(U2STAbits.UTXBF);  /* wait if the buffer is full */
            U2TXREG = *temp_ptr++;   /* transfer data byte to TX reg */
        }
    }
}


/******************************************************************************
* Function Name     : getsUART2                                               *
* Description       : This function gets a string of data of specified length * 
*                     if available in the UxRXREG buffer into the buffer      *
*                     specified.                                              *
* Parameters        : unsigned int length the length expected                 *
*                     unsigned int *buffer  the received data to be           * 
*                                  recorded to this array                     *
*                     unsigned int uart_data_wait timeout value               *
* Return Value      : unsigned int number of data bytes yet to be received    * 
******************************************************************************/

unsigned int getsUART2(unsigned int length,unsigned int *buffer,
                       unsigned int uart_data_wait)

{
    unsigned int wait = 0;
    char *temp_ptr = (char *) buffer;

    while(length)                         /* read till length is 0 */
    {
        while(!DataRdyUART2())
        {
            if(wait < uart_data_wait)
                wait++ ;                  /*wait for more data */
            else
                return(length);           /*Time out- Return words/bytes to be read */
        }
        wait=0;
        if(U2MODEbits.PDSEL == 3)         /* check if TX/RX is 8bits or 9bits */
            *buffer++ = U2RXREG;          /* data word from HW buffer to SW buffer */
		else
            *temp_ptr++ = U2RXREG & 0xFF; /* data byte from HW buffer to SW buffer */

        length--;
    }

    return(length);                       /* number of data yet to be received i.e.,0 */
}


/*********************************************************************
* Function Name     : DataRdyUart2                                   *
* Description       : This function checks whether there is any data *
*                     that can be read from the input buffer, by     *
*                     checking URXDA bit                             *
* Parameters        : None                                           *
* Return Value      : char if any data available in buffer           *
*********************************************************************/

char DataRdyUART2(void)
{
    return(U2STAbits.URXDA);
}


/*************************************************************************
* Function Name     : BusyUART2                                          *
* Description       : This returns status whether the transmission       *  
*                     is in progress or not, by checking Status bit TRMT *
* Parameters        : None                                               *
* Return Value      : char info whether transmission is in progress      *
*************************************************************************/

char BusyUART2(void)
{  
    return(!U2STAbits.TRMT);
}


/***************************************************************************
* Function Name     : ReadUART2                                            *
* Description       : This function returns the contents of UxRXREG buffer *
* Parameters        : None                                                 *  
* Return Value      : unsigned int value from UxRXREG receive buffer       * 
***************************************************************************/

unsigned int ReadUART2(void)
{
    if(U2MODEbits.PDSEL == 3)
        return (U2RXREG);
    else
        return (U2RXREG & 0xFF);
}


/*********************************************************************
* Function Name     : WriteUART2                                     *
* Description       : This function writes data into the UxTXREG,    *
* Parameters        : unsigned int data the data to be written       *
* Return Value      : None                                           *
*********************************************************************/

void WriteUART2(unsigned int data)
{
    if(U2MODEbits.PDSEL == 3)
        U2TXREG = data;
    else
        U2TXREG = data & 0xFF;  
}


#elif defined(__18CXX)
//
// PIC18
//


char BusyUSART(void)
{
	return !TXSTAbits.TRMT;
}

void CloseUSART(void)
{
  RCSTA &= 0b01001111;  // Disable the receiver
  TXSTAbits.TXEN = 0;   // and transmitter

  PIE1 &= 0b11001111;   // Disable both interrupts
}

char DataRdyUSART(void)
{
	if(RCSTAbits.OERR)
	{
		RCSTAbits.CREN = 0;
		RCSTAbits.CREN = 1;
	}
  return PIR1bits.RCIF;
}

char ReadUSART(void)
{
  return RCREG;                     // Return the received data
}

void WriteUSART(char data)
{
  TXREG = data;      // Write the data byte to the USART
}

void getsUSART(char *buffer, unsigned char len)
{
  char i;    // Length counter
  unsigned char data;

  for(i=0;i<len;i++)  // Only retrieve len characters
  {
    while(!DataRdyUSART());// Wait for data to be received

    data = getcUART();    // Get a character from the USART
                           // and save in the string
    *buffer = data;
    buffer++;              // Increment the string pointer
  }
}

void putsUSART( char *data)
{
  do
  {  // Transmit a byte
    while(BusyUSART());
    putcUART(*data);
  } while( *data++ );
}

void putrsUSART(const rom char *data)
{
  do
  {  // Transmit a byte
    while(BusyUSART());
    putcUART(*data);
  } while( *data++ );
}

#endif


#endif	//STACK_USE_UART
