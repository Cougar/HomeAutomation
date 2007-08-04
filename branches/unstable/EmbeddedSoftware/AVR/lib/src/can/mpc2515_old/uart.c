/**
 * @file	uart.c
 * 			Routines for interrupt based USART communication on the ATmega8.
 * @author	Jimmy Myhrman
 * @date	2005-08-20
 */

/* -----------------------------------------------------------------------------
 * Includes
 * ---------------------------------------------------------------------------*/
#include "uart.h"

/* -----------------------------------------------------------------------------
 * Defines
 * ---------------------------------------------------------------------------*/
#define TX_BUFSIZE	256			/* must be power of 2 */
#define RX_BUFSIZE	2			/* must be power of 2 */

#define TX_MASK		(TX_BUFSIZE-1)
#define RX_MASK		(RX_BUFSIZE-1)

#define RX_BUFLEN	(rx_tail - rx_head)
#define TX_BUFLEN	(tx_tail - tx_head)

/* -----------------------------------------------------------------------------
 * Variables
 * ---------------------------------------------------------------------------*/
volatile unsigned char tx_buf[TX_BUFSIZE];
volatile unsigned char rx_buf[RX_BUFSIZE];

volatile uint8_t tx_head;
volatile uint8_t tx_tail;

volatile uint8_t rx_head;
volatile uint8_t rx_tail;

/* -----------------------------------------------------------------------------
 * Functions
 * ---------------------------------------------------------------------------*/

void uartInit(unsigned char baudrate) {
	UBRRH = (unsigned char) (baudrate>>8);
	UBRRL = (unsigned char) baudrate;
	/* 8 Databits, receive and transmit enabled, receive complete interrupt enabled */
	UCSRB = (1<<RXCIE)|(1<<TXEN)|(1<<RXEN);
}

int uartPutChar(char c) {
	// Fills the transmit buffer, if it is full wait
	//TODO: if interrupts are currently disabled (which is the case when this
	//function is called from within an interrupt routine), skip the while-loop
	//below. otherwise, use it
	//while ((TX_BUFSIZE - TX_BUFLEN) <= 2) {
		//if uartPutchar is called while global ints are disabled, we are
		//stuck here!
	//}
	if ((TX_BUFSIZE - TX_BUFLEN) <= 2) {
		return(1);
	}
	// Add data to the transmit buffer, move the tail and enable TXCIE
	// while tx_tail will count from 0 to 255... AND'ing it with 31 will
	//continually wrap the index 0 to 31
	tx_buf[tx_tail & TX_MASK] = c;
	tx_tail++;
	// enable data register empty interrupt
	UCSRB |= (1<<UDRIE);
	return(0);
}


int uartGetChar (void) {
	unsigned char c;
	// this will sit here and wait for SIG_UART0_RECV to get a character and
	//place it into rx0_buff
	while (RX_BUFLEN == 0) {
	}
	// rx_head will count from 0 to 255... AND'ing it with 31 will continually
	//wrap the index 0 to 31
	c = rx_buf[rx_head & RX_MASK];
	rx_head++;
	return(c);
}


// USART receive complete interrupt
SIGNAL(SIG_UART_RECV) {
	char c;
	// Get the received character
	c = UDR;
	// Echo the character back
	uartPutChar(c);
	// place the received character into the receive buffer and move the tail
	// rx_tail will count from 0 to 255... AND'ing it with 31 will continually
	//wrap the index 0 to 31
	rx_buf[rx_tail & RX_MASK] = c;
	rx_tail++;
}


// USART0 data register empty interrupt... another char can be transmitted if (tx0_head != tx0_tail)
SIGNAL(SIG_UART_DATA) {
	// tx_head will count from 0 to 255... AND'ing it with 31 will continually
	//wrap the index 0 to 31
	if (tx_head != tx_tail) {
		UDR = tx_buf[tx_head & TX_MASK];
		tx_head++;
	}
	else {
	    // "~" is a one's compliment so UCSR0B = (UCSR0B AND 11011111) which disables udr empty interrupt
		UCSRB &= ~(1<<UDRIE);
	}
}


//string must be null-terminated
void uartPutString(char* str) {
	while (*str != 0) {
		uartPutChar(*str);
		str++;
	}
}


void uartPutUInt8(uint8_t number, uint8_t base) {
	uint8_t a;
	if ((a = number/base) != 0) {
		uartPutUInt8(a, base);
	}
	uartPutChar("0123456789ABCDEF"[(uint8_t)(number%base)]);
}


void uartPutUInt16(uint16_t number, uint8_t base) {
	uint16_t a;
	if ((a = number/base) != 0) {
		uartPutUInt16(a, base);
	}
	uartPutChar("0123456789ABCDEF"[(uint8_t)(number%base)]);
}


void uartNewLine(void) {
	uartPutString("\n\r");
}
