/**
 * Configuration file for the CAN.C software module.
 */

#ifndef CAN_CFG_H_
#define CAN_CFG_H_

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/

/**
 * CAN controller. Determines which CAN controller hardware is used. The
 * supported controllers are listed in can.h.
 */
#define CAN_CONTROLLER		CAN_CONTROLLER_MCP2515
//#define CAN_CONTROLLER		CAN_CONTROLLER_NULL


/**
 * CAN bitrate. Possible bitrates are listed in can.h.
 */
#define CAN_BITRATE			CAN_BITRATE_125K


/**
 * Size of the CAN message reception queue (in terms of number of CAN messages).
 * Set to 0 to completely disable the TX queue mechanism (messages will be sent
 * directly to the controller).
 */
#define CAN_QUEUE_SIZE_RX	0


/**
 * Size of the CAN message transmission queue (in terms of number of CAN messages).
 * Set to 0 to completely disable the RX queue mechanism (messages are received
 * directly from the controller rather from a reception queue).
 */
#define CAN_QUEUE_SIZE_TX	0


/**
 * Number of transmit buffers available in the CAN controller hardware. This
 * information is needed in order to optimize the loop that empties the TX
 * queue in each service call.
 */
#define CAN_CONTROLLER_NR_TX_BUFFERS	3


/**
 * Number of receive buffers available in the CAN controller hardware. This
 * information is needed in order to optimize the loop that fetches new
 * messages from the controller and puts them into the RX queue.
 */
#define CAN_CONTROLLER_NR_RX_BUFFERS	2


#endif /*CAN_CFG_H_*/
