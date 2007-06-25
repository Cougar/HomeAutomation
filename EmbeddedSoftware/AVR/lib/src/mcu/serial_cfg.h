/**
 * Configuration file for the Serial Communication software module.
 */

#ifndef SERIAL_CFG_H_
#define SERIAL_CFG_H_


/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/

/** Baudrate */
#define SERIAL_BAUDRATE			9600

/** Size of the circular receive buffer, must be power of 2 */
#define SERIAL_RX_BUFFER_SIZE	2

/** Size of the circular transmit buffer, must be power of 2 */
#define SERIAL_TX_BUFFER_SIZE	64


#endif /*SERIAL_CFG_H_*/
