/**
 * Serial communication software module.
 * 
 * @author	Jimmy Myhrman
 * 
 * @date	2006-11-22
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <config.h>
#include <drivers/uart/serial.h>
#include <drivers/uart/uart.h>


/*-----------------------------------------------------------------------------
 * Private Function Prototypes
 *---------------------------------------------------------------------------*/
static int serial_putchar(char c, FILE *stream);


/*-----------------------------------------------------------------------------
 * Variables
 *---------------------------------------------------------------------------*/
static FILE myStdOut = FDEV_SETUP_STREAM(serial_putchar, NULL,_FDEV_SETUP_WRITE);


/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

/**
 * Initializes the serial communication. Sets the baudrate and redirects
 * STDOUT to the serial communication channel.
 */
void Serial_Init() {
	uart_init((UART_BAUD_SELECT((SERIAL_BAUDRATE),F_CPU)));
	
	stdout = &myStdOut;
}


/*-----------------------------------------------------------------------------
 * Private Functions
 *---------------------------------------------------------------------------*/

static int serial_putchar(char c, FILE *stream) {
	if (c == '\n') serial_putchar('\r', stream);
	uart_putc(c);
	return 0;
}
