/**
 * Communication services.
 *
 * @date	2006-12-17
 * @author	Jimmy Myhrman
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <com.h>
#include <can.h>
#include <stdio.h>


/*-----------------------------------------------------------------------------
 * Variables
 *---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 * Private Function Prototypes
 *---------------------------------------------------------------------------*/
static void Com_ParseReceivedMessage(Can_Message_t *msg);


/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

/**
 * Initializes the communication subsystem.
 */
void Com_Init() {
	//TODO
}


/**
 * Services the communication subsystems.
 */ 
void Com_Service() {
	Can_Message_t msgBuf;
	
	/*
	 * Empty the CAN receive FIFO and parse the messages.
	 * TODO: don't empty the whole FIFO. best flow is probably obtained if
	 * 		 we pull the same number of messages as the number of RX buffers
	 * 		 the used CC has.
	 */
	while (Can_Receive(&msgBuf)) {
        Com_ParseReceivedMessage(&msgBuf);
    }
    
    /*
	 * Service the CAN drivers. Receive and transmit pending CAN messages.
	 */
	Can_Service();
}


/*-----------------------------------------------------------------------------
 * Private Functions
 *---------------------------------------------------------------------------*/

/**
 * Parses a received CAN message in accordance with the protocol.
 * 
 * @param msg
 * 			Pointer to the CAN message structure.
 */
static void Com_ParseReceivedMessage(Can_Message_t *canMsg) {
	/*
	 * Prepare a COM message.
	 */
	Com_Message_t comMsg;
	comMsg.Funct =	(canMsg->Id & 0x1E000000L) >> 25;	/* bit[25..28] */
	comMsg.Funcc =	(canMsg->Id & 0x01FF8000L) >> 15;	/* bit[15..24] */
	comMsg.Nid =	(canMsg->Id & 0x00007E00L) >> 9;	/* bit[9..14] */
	comMsg.Sid =	(canMsg->Id & 0x000001FFL) >> 0;	/* bit[0..8] */
	
	/*
	 * Parse the COM message.
	 */
	//TODO
}
