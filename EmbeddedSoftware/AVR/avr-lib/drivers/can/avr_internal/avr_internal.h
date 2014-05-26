/** 
 * @defgroup avrinternal avr internal Library
 * @code #include <drivers/can/avr_internal/avr_internal.h> @endcode
 * 
 * @brief Functions for can controller communication. 
 * Exposes functions for initialization of controller and sending a message.
 * For receiving messages, an interrupt callback must be set up at the user of 
 * this driver.
 * 
 * 
 * @author	Jimmy Myhrman, Andreas Fritiofson, Anders Sandblad
 * @date	2005-01-01
 */

/**@{*/

#ifndef AVRINTERNAL_H_
#define AVRINTERNAL_H_

#include <drivers/can/can.h>

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
#if MCP_CAN_PROCESS_RENAMED == 1
void Can_Process_USART(Can_Message_t* msg);
#else
void Can_Process(Can_Message_t* msg);
#endif

/**@}*/
#endif
