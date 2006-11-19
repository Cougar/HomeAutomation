/**
 * CAN Interface. This is a general high-level interface for CAN communication.
 * Underlying CAN controllers are abstracted by this interface, so the
 * application needs not work directly against the CAN controllers.
 * 
 * @date	2006-11-19
 * 
 * @author	Jimmy Myhrman
 * @author	Martin Thomas
 *   
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <string.h>
#include <can.h>
#include "timebase.h"


#ifdef CAN_CONTROLLER_MCP2515
	#include "mcp2515.h"
#else
	#error No CAN controller specified! Edit can_cfg.h !
#endif


/*-----------------------------------------------------------------------------
 * Functions
 *---------------------------------------------------------------------------*/

/**
 * Initializes the CAN interface. Edit can_cfg.h to choose bitrate.
 * 
 * @return
 *		CAN_OK if initialization was successful.
 *		CAN_FAILINIT otherwise.
 */
Can_Return_t Can_Init() {
	Can_Bitrate_t bitrate = CAN_BITRATE;
#ifdef CAN_CONTROLLER_MCP2515
	if (MCP2515_Init(bitrate) != MCP2515_OK) {
		return CAN_FAILINIT;
	}
	if (MCP2515_SetCanCtrlMode(MODE_NORMAL) != MCP2515_OK) {
		return CAN_FAILINIT;
	}
	return CAN_OK;
#endif

	return CAN_FAILINIT;
	
}


/**
 * Sends a CAN message. If the CAN controller is busy, the message will be
 * copied to an internal transmission queue and transmitted later.
 *
 * @param msg
 *		Pointer to the CAN message storage buffer.
 * @return
 *		CAN_OK if the message was successfully sent (or put in queue).
 *		CAN_FAILTX if the controller is busy AND the transmission queue is full.
 */
Can_Return_t Can_Send(Can_Message_t* msg) {

#ifdef CAN_CONTROLLER_MCP2515
	uint8_t res, txbuf_n;
	res = MCP2515_GetNextFreeTXBuf(&txbuf_n); // info = addr.
	if (res == MCP_ALLTXBUSY) {
		return CAN_FAILTX;
	}
	MCP2515_WriteCanMsg(txbuf_n, msg);
	MCP2515_StartTransmit(txbuf_n);
	return CAN_OK;
#endif

}


/**
 * Pulls a message from the internal CAN interface reception queue. Unless this queue
 * is empty, a CAN message is moved from the queue into the message storage provided
 * by the calling function.
 * 
 * @param msg
 *		Pointer to the message storage buffer.
 * @return
 *		CAN_OK if a received message was successfully copied into the buffer.
 *		CAN_NOMSG if there are no received messages available.
 */
Can_Return_t Can_Receive(Can_Message_t *msg) {
	
#ifdef CAN_CONTROLLER_MCP2515
	uint8_t stat, res;
	stat = MCP2515_ReadStatus();
	if (stat & MCP_STAT_RX0IF) {
		// Msg in Buffer 0
		MCP2515_ReadCanMsg( MCP_RXBUF_0, msg);
		MCP2515_ModifyRegister(MCP_CANINTF, MCP_RX0IF, 0);
		res = CAN_OK;
	}
	else if (stat & MCP_STAT_RX1IF) {
		// Msg in Buffer 1
		MCP2515_ReadCanMsg( MCP_RXBUF_1, msg);
		MCP2515_ModifyRegister(MCP_CANINTF, MCP_RX1IF, 0);
		res = CAN_OK;
	}
	else {
		res = CAN_NOMSG;
	}	
	return res;
#endif

}


/**
 * Checks if there are any received CAN messages waiting in the reception queue.
 * 
 * @return
 *		CAN_MSGAVAIL if there are messages available.
 *		CAN_NOMSG if the queue is empty.
 */
Can_Return_t Can_ReceiveAvailable(void) {
	
#ifdef CAN_CONTROLLER_MCP2515
	uint8_t res;
	res = MCP2515_ReadStatus(); // RXnIF in Bit 1 and 0
	if ( res & MCP_STAT_RXIF_MASK ) {
		return CAN_MSGAVAIL;
	}
	else {
		return CAN_NOMSG;
	}
#endif

}


/*
 * Checks Controller-Error-State.
 * 
 * @return
 * 		CAN_OK if the controller is OK.
 *		CAN_CTRLERROR if errors have occured.
 * @todo
 * 		Styr upp denna funktion lite bättre.
 */
Can_Return_t Can_CheckError(void) {
	
#ifdef CAN_CONTROLLER_MCP2515
	uint8_t eflg = MCP2515_ReadRegister(MCP_EFLG);
	if (eflg & MCP_EFLG_ERRORMASK) {
		return CAN_CTRLERROR;
	}
	else {
		return CAN_OK;
	}
#endif

}
