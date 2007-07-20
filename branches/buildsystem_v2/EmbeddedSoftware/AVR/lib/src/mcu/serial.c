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
#include <serial.h>
#include <uart.h>
#include <stdlib.h>
#include <stdio.h>


/*-----------------------------------------------------------------------------
 * Prerequisites
 *---------------------------------------------------------------------------*/
#ifndef SERIAL_BAUDRATE
	#error SERIAL_BAUDRATE not specified! Edit serial_cfg.h !
#endif

#ifndef SERIAL_RX_BUFFER_SIZE
	#error SERIAL_RX_BUFFER_SIZE not specified! Edit serial_cfg.h !
#endif

#ifndef SERIAL_TX_BUFFER_SIZE
	#error SERIAL_TX_BUFFER_SIZE not specified! Edit serial_cfg.h !
#endif


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
