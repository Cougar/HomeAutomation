/**
 * Configuration file for the UART software module.
 */

#ifndef UART_CFG_H_
#define UART_CFG_H_

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/

/** UART baudrate */
#define BAUD 115200
/** Size of the circular receive buffer, must be power of 2 */
#define UART_RX_BUFFER_SIZE 32
/** Size of the circular transmit buffer, must be power of 2 */
#define UART_TX_BUFFER_SIZE 32

#endif /*UART_CFG_H_*/
