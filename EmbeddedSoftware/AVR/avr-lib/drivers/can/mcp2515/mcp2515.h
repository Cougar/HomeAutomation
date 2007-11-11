/** 
 * @defgroup mcp2515 mcp2515 Library
 * @code #include <drivers/can/mcp2515/mcp2515.h> @endcode
 * 
 * @brief Functions for can controller communication. 
 * Exposes functions for initialization of controller and sending a message.
 * For receiving messages, an interrupt callback must be set up at the user of 
 * this driver.
 * 
 * Based on Martin Thomas' code which was based on Kvaser's 
 * MCP2510 sample-application.
 * 
 * @author	Jimmy Myhrman, Andreas Fritiofson
 * @date	2005-01-01
 */

/**@{*/

#ifndef MCP2515_H_
#define MCP2515_H_

#include <drivers/can/mcp2515/mcp2515_defs.h>
#include <drivers/can/mcp2515/mcp2515_cfg.h>
#include <drivers/can/can.h>

#define MCP_N_TXBUFFERS (3)

#define MCP_RXBUF_0 (MCP_RXB0SIDH)
#define MCP_RXBUF_1 (MCP_RXB1SIDH)
#define MCP_TXBUF_0 (MCP_TXB0SIDH)
#define MCP_TXBUF_1 (MCP_TXB1SIDH)
#define MCP_TXBUF_2 (MCP_TXB2SIDH)

#define MCP2515_SELECT()   ( MCP_CS_PORT &= ~(1<<MCP_CS_BIT) )
#define MCP2515_UNSELECT() ( MCP_CS_PORT |=  (1<<MCP_CS_BIT) )

#define MCP2515_OK         (0)
#define MCP2515_FAIL       (1)
#define MCP_ALLTXBUSY      (255)


/**
 * @brief Initialize CAN.
 * Initializes CAN interface and the mcp2515 can controller 
 * 
 * @return
 *		CAN_OK if initialization was successful.
 * 		CAN_INIT_FAIL in case of general error.
 */ 
Can_Return_t Can_Init(void);

/**
 * @brief Send a CAN message.
 * Sends a CAN message immediately with the controller hardware. If the CAN
 * controller is busy, the function will return CAN_FAIL.
 *
 * @param msg
 *		Pointer to the CAN message storage buffer.
 * 
 * @return
 *		CAN_OK if the message was successfully sent to the controller.
 *		CAN_FAIL if the controller is busy.
 */
Can_Return_t Can_Send(Can_Message_t* msg);

/**
 * @brief Callback from driver interrupt service routine.
 * Users of mcp2515 drivers must implement this function to receive 
 * CAN messages.
 * 
 * @param msg
 *		Pointer to the CAN message storage buffer.
 */
void Can_Process(Can_Message_t* msg);  

#endif
