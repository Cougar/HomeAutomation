/** 
 * @defgroup stdcan StdCan Library
 * @code #include <drivers/can/stdcan.h> @endcode
 * 
 * @brief High level functions for bus communication and standard node behaviour. 
 *
 * This library provides a higher level interface to the CAN bus than the raw driver/BIOS
 * interface, as well as functions for easy implementation of required node behaviour.
 * 
 * It implements receive and transmit FIFOs of configurable lengths, Application Startup
 * message at init, and Heartbeat transmission.
 *
 * @author	Andreas Fritiofson
 * @date	2007-10-24
 */

/**@{*/

//#error StdCan: For discussion only! Do not use in application code.
#warning StdCan: Not thourhly tested yet.

#ifndef STDCAN_H_
#define STDCAN_H_

#include <config.h>

#include <inttypes.h>

#ifndef STDCAN_FILTER
#define STDCAN_FILTER 1
#endif
#ifndef STDCAN_RX_QUEUE_SIZE
#define STDCAN_RX_QUEUE_SIZE 8
#endif
#ifndef STDCAN_TX_QUEUE_SIZE
#define STDCAN_TX_QUEUE_SIZE 1
#endif

/**
 * @brief Return values.
 * 
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
 * @brief Message structure.
 * 
 * Stores a CAN message frame. Flags that are unsupported have been
 * excluded (RTR and standard id).
 */
typedef struct {
	unsigned long Id; /**< CAN extended ID (29 bits). */
	char Length; /**< Data length [0,8]. */
	unsigned char Data[8]; /**< Data array. Only the first \c Length elements are valid. */
#if (STDCAN_FILTER)
	unsigned char Match; /**< Filter id that matched this message. */ 
#endif
} StdCan_Msg_t;

/**
 * @brief Node descriptor.
 * 
 * Describes the node and the application running on it.
 * TODO: Discuss the purpose and layout of Node_Desc_t.
 */
typedef struct {
	unsigned short Type; /**< Application type (documented elsewhere). */
	unsigned short Version; /**< Application version number (application defined). */
	unsigned char Id; /**< Node ID. */
} Node_Desc_t;

/**
 * @brief Initialize StdCan.
 * 
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
 * @brief Get a message.
 * 
 * Retrieves a message from the receive queue.
 * 
 * @param[out] msg
 * 		Pointer to message stucture to be filled with the next message from
 * 		the queue.
 */ 
StdCan_Ret_t StdCan_Get(StdCan_Msg_t* msg);

/**
 * @brief Queue size.
 * 
 * Returns the number of messages pending in the receive queue.
 * 
 * @retval
 * 		Number of messages in the receive queue.
 */ 
unsigned char StdCan_Get_Pending(void);

/**
 * @brief Send a message.
 * 
 * Puts a message on the transmit queue.
 * 
 * @param[in] msg
 * 		Pointer to message to send.
 */ 
StdCan_Ret_t StdCan_Put(StdCan_Msg_t* msg);

/**
 * @brief Send a Heartbeat.
 * 
 * Puts a Heartbeat message on the transmit queue. Takes an \c uint8_t and
 * returns \c void in order to be able to use it as a \c Timer callback
 * directly, i.e. 
 * @code
 * Timer_SetTimeout(APP_HEARTBEAT_TIMER, STDCAN_HEARTBEAT_PERIOD, TimerTypeFreeRunning, StdCan_SendHeartbeat);
 * @endcode
 * If the transmit queue is full this function will silently fail.
 * 
 * @param n
 * 		Dummy value.
 */
void StdCan_SendHeartbeat(uint8_t n);

/**
 * @brief Enable a message acceptance filter.
 * 
 * Allows incoming messages to be accepted or rejected by matching their id.
 * Message is accepted if @code (message_id ^ id) & mask @endcode is zero for any
 * of the active filters. 
 * 
 * @param filter
 * 		Filter slot to activate (filter < STDCAN_NUM_FILTERS).
 * @param id
 * 		Match if id matches the message id in all bit locations that are 
 * 		not masked.
 * @param mask
 * 		Each bit specifies if the corresponding id bit must match (mask[n] = 1) 
 * 		or is Don't Care (mask[n] = 0).
 */
StdCan_Ret_t StdCan_EnableFilter(unsigned char filter, unsigned long id, unsigned long mask);

/**
 * @brief Disable a message acceptance filter.
 * 
 * Disables a filter (marks it at not matching any messages).
 * 
 * @param filter
 * 		Filter slot to disable (filter < STDCAN_NUM_FILTERS).
 */
StdCan_Ret_t StdCan_DisableFilter(unsigned char filter);

/**@}*/

#endif /*STDCAN_H_*/
