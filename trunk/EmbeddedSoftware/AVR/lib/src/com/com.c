/**
 * Communication services.
 *
 * @date	2006-10-29
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
static void Com_ParseReceivedMessage(Can_Message_t *msg) {
#if 0
	printf("Com_ParseReceivedMessage: {ID=%x, DLC=%u, EXT=%u, RTR=%u}\n\r", msg->Id, msg->DataLength, msg->ExtendedFlag, msg->RemoteFlag); 
#endif
	
	//TODO: pack

}
