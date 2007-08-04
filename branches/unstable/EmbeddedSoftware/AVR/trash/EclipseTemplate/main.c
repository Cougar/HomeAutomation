#define F_CPU 1000000UL
#define F_BAUD  31250UL

#include <inttypes.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

static int uart_putchar(char c, FILE *stream);
static int uart_getchar(FILE *stream);

static FILE mystdio = FDEV_SETUP_STREAM(uart_putchar, uart_getchar,
                                         _FDEV_SETUP_RW);

static int
    uart_putchar(char c, FILE *stream)
    {
      loop_until_bit_is_set(UCSRA, UDRE);
      UDR = c;
      return 0;
    }
    
static int
    uart_getchar(FILE *stream)
    {
      loop_until_bit_is_set(UCSRA, RXC);
      return UDR;
    }
    
void init(void)
{
  // Setup USART for debug channel
  UCSRB = _BV(RXEN)|_BV(TXEN);
  UBRRL = (F_CPU / 16 / F_BAUD) - 1;
  stdout = &mystdio;
  stdin = &mystdio;
}

int main(void)
{
	init();
	printf("Hello, World!\n");
	
	return 0;
}
