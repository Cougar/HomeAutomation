/******************************************************************************
 * Copyright (C) 2005 Martin THOMAS, Kaiserslautern, Germany
 * <eversmith@heizung-thomas.de>
 * http://www.siwawi.arubi.uni-kl.de/avr_projects
 *****************************************************************************
 *
 * File    : termio.c
 * Version : 0.9
 * 
 * Summary : Terminal Input/Output-Functions
 *
 *****************************************************************************/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <inttypes.h>
#include <stdlib.h>

#include "termio.h"
#include "uart.h"

#define putc_loc uart_putc

unsigned char term_timeout_flag;

void putc_cr(char c)
{
	if (c == '\n')	putc_loc('\r');
	putc_loc(c);
}

void term_putc(const char c)
{
	putc_loc(c);
}

uint16_t term_getc()
{
	// uint16_t res;
	
	/* TODO - timeout */
	
	return uart_getc();
	
	
}

void term_put_cr(void)
{
	putc_cr('\n');
}

void term_puts(const char* tx_data)
{
	char c;
	while( (c=*tx_data++) ) putc_cr(c); 
}

void term_puts_p(const char *progmem_tx_data)
{   
   uint8_t c=0;  
   // uint8_t sreg;
   
   // sreg=SREG;
   // cli();
   while( (c=pgm_read_byte(progmem_tx_data++)) ) putc_cr(c);
   // SREG=sreg;
}

void term_puti(const int num)
{
	char buffer[8 * sizeof (int) + 1];
	itoa(num,buffer,10);
	term_puts(buffer);
}

void term_puthex_byte(const uint8_t val)
{
	uart_puthex_byte(val);
}
	
void term_putbin_byte(const uint8_t val) 
{
	uart_putbin_byte(val);
}

