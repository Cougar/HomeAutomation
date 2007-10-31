/**
 * High level functions for bus communication and standard node behaviour.
 * 
 * @author	Andreas Fritiofson
 * 
 * @date	2007-10-24
 */

#error StdCan: For discussion only! Do not use in application code.

#ifndef STDCAN_H_
#define STDCAN_H_

/**
 * Return values.
 * Most StdCan functions return one of these values.
 */
typedef enum {
	StdCan_Ret_OK, /**< Operation completed successfully. */
	StdCan_Ret_Full, /**< Failed to send because transmit queue was full. */
	StdCan_Ret_Empty, /**< Failed to read because receive queue was empty. */
	StdCan_Ret_Fail, /**< General error. */
	StdCan_Ret_DataErr /**< Malformed data. */
} StdCan_Ret_t;

/**
 * Message structure.
 * Stores a CAN message frame. Flags that are unused at this level have been
 * excluded (RTR and standard id).
 */
typedef struct {
	unsigned long Id; /**< CAN extended ID (29 bits). */
	char Length; /**< Data length [0,8]. */
	unsigned char Data[8]; /**< Data array. Only the first \c Length elements are valid. */
} StdCan_Msg_t;

/**
 * Node descriptor structure.
 * Describes the node and the application running on it.
 * TODO: Discuss the purpose and layout of Node_Desc_t.
 */
typedef struct {
	unsigned short Type; /**< Application type (documented elsewhere). */
	unsigned short Version; /**< Application version number (application defined). */
	unsigned char Id; /**< Node ID. */
} Node_Desc_t;

/**
 * Initialize StdCan.
 * Initializes the StdCan and lower layers and sends an Application Startup
 * Message.
 * 
 * @param node_desc
 * 		Pointer to node descriptor, containing information about the type and
 * 		version of the application that should be announced in startup and
 * 		heartbeat messages.
 */ 
StdCan_Ret_t StdCan_Init(Node_Desc_t* node_desc);

/**
 * Get a message.
 * Retrieves a message from the receive queue.
 * 
 * @param[out] msg
 * 		Pointer to message stucture to be filled with the next message from
 * 		the queue.
 */ 
StdCan_Ret_t StdCan_Get(StdCan_Msg_t* msg);

/**
 * Send a message.
 * Puts a message on the transmit queue.
 * 
 * @param[in] msg
 * 		Pointer to message to send.
 */ 
StdCan_Ret_t StdCan_Put(StdCan_Msg_t* msg);

/**
 * Send a Heartbeat.
 * Puts a Heartbeat message on the transmit queue. Takes an \c uint8_t and
 * returns \c void in order to be able to use it as a \c Timer callback
 * directly, i.e. 
 * \code
 * 		Timer_SetTimeout(APP_HEARTBEAT_TIMER, STDCAN_HEARTBEAT_PERIOD, TimerTypeFreeRunning, StdCan_SendHeartbeat);
 * \endcode
 * If the transmit queue is full this function will silently fail.
 * 
 * @param n
 * 		Dummy value.
 */
void StdCan_SendHeartbeat(uint8_t n);

#endif /*STDCAN_H_*/
