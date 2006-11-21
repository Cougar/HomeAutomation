/**
 * CAN communication interface. This is a general, high-level interface for CAN
 * communication. Underlying CAN controllers are abstracted by this interface,
 * so the application needs not work directly with the CAN controllers.
 * 
 * The interface is fully queued in both directions. Both queues can configured
 * in terms of size, and can also be disabled completely to save both RAM and
 * FLASH space (the queue routines are removed by preprocessor when queues
 * are disabled). When queues are enabled, the application has to call
 * Can_Service as often as possible, so messages can be moved between the CAN
 * controller and the message queues. This might be handled by interrupts in
 * future. When queues are disabled, on the other hand, the application has
 * to call Can_Receive as often as possible in order not to loose any messages.
 * 
 * @date	2006-11-21
 * 
 * @author	Jimmy Myhrman
 *   
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <string.h>
#include <can.h>
#include <mcu.h>
#include <stdio.h>
#include <timebase.h>
#include <assert.h>
#if CAN_CONTROLLER == CAN_CONTROLLER_MCP2515
	#include "mcp2515.h"
#endif


/*-----------------------------------------------------------------------------
 * Prerequisites
 *---------------------------------------------------------------------------*/
#ifndef CAN_CONTROLLER
	#error CAN_CONTROLLER not specified! Edit can_cfg.h !
#endif

#ifndef CAN_BITRATE
	#error CAN_BITRATE not defined! Edit can_cfg.h !
#endif

#ifndef CAN_QUEUE_SIZE_RX
	#error CAN_QUEUE_SIZE_RX not defined! Edit can_cfg.h !
#endif

#ifndef CAN_QUEUE_SIZE_TX
	#error CAN_QUEUE_SIZE_TX not defined! Edit can_cfg.h !
#endif

#ifndef CAN_CONTROLLER_NR_TX_BUFFERS
	#error CAN_CONTROLLER_NR_TX_BUFFERS not defined! Edit can_cfg.h !
#endif

#ifndef CAN_CONTROLLER_NR_RX_BUFFERS
	#error CAN_CONTROLLER_NR_RX_BUFFERS not defined! Edit can_cfg.h !
#endif


/*-----------------------------------------------------------------------------
 * Type Definitions
 *---------------------------------------------------------------------------*/

#if (CAN_QUEUE_SIZE_TX > 0) || (CAN_QUEUE_SIZE_RX > 0)
/**
 * CAN Message Queue Type. Stores and manages several CAN messages
 * in a FIFO structure.
 */
typedef struct {
	Can_Message_t *dataPtr; /* pointer to the data storage */
	uint8_t head;		/* current OUTPUT position */
	uint8_t tail;		/* current INPUT position */
	uint8_t nrElements;	/* nr of elements currently queued */
	uint8_t size;		/* total size of the queue */
} Can_MessageQueue_t;
#endif


/*-----------------------------------------------------------------------------
 * Private (static) variables
 *---------------------------------------------------------------------------*/

#if CAN_QUEUE_SIZE_TX > 0
	/* TX queue object and static memory storage buffer for this queue */
	static Can_MessageQueue_t canTxQueue;
	static Can_Message_t canTxQueueDataStorage[CAN_QUEUE_SIZE_TX];
#endif
#if CAN_QUEUE_SIZE_RX > 0
	/* RX queue object and static memory storage buffer for this queue */
	static Can_MessageQueue_t canRxQueue;
	static Can_Message_t canRxQueueDataStorage[CAN_QUEUE_SIZE_RX];
#endif


/*-----------------------------------------------------------------------------
 * Private Function Prototypes
 *---------------------------------------------------------------------------*/

/* functions that deal with the controller hardware */
static Can_Return_t Can_SendImmediately(Can_Message_t *msg);
static Can_Return_t Can_ReceiveFromController(Can_Message_t *msg, uint8_t rxBuffer);
static uint8_t Can_MessagesAvailableInController(void);
static Can_Return_t Can_ControllerCheckError(void);

#if (CAN_QUEUE_SIZE_TX > 0) || (CAN_QUEUE_SIZE_RX > 0)
/* functions that deal with the message queues */
static void Can_QueueInit(Can_MessageQueue_t *q, uint8_t size, Can_Message_t *dataPtr);
static Can_Message_t* Can_QueueInsert(Can_MessageQueue_t *q);
static Can_Message_t* Can_QueueReadTailPtr(Can_MessageQueue_t *q);
static void Can_QueueRemoveTail(Can_MessageQueue_t *q);
#endif


/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

/**
 * Initializes the CAN interface. Edit can_cfg.h to choose bitrate.
 * 
 * @return
 *		CAN_OK if initialization was successful.
 * 		CAN_INIT_FAIL_SET_BITRATE if bitrate could not be set correctly.
 * 		CAN_INIT_FAIL_SET_MODE if the controller could not be set to normal operation mode.
 * 		CAN_INIT_FAIL in case of general error.
 */
Can_Return_t Can_Init() {
	#if CAN_QUEUE_SIZE_TX > 0
		/* initialize TX queue */
		Can_QueueInit(&canTxQueue, CAN_QUEUE_SIZE_TX, canTxQueueDataStorage);
	#endif
	#if CAN_QUEUE_SIZE_RX > 0
		/* initialize RX queue */
		Can_QueueInit(&canRxQueue, CAN_QUEUE_SIZE_RX, canRxQueueDataStorage);
	#endif
	
	#if CAN_CONTROLLER == CAN_CONTROLLER_NULL
		/*
		 * Initialize null device.
		 */
		return CAN_OK;
	#endif
	
	#if CAN_CONTROLLER == CAN_CONTROLLER_MCP2515
		/*
		 * Initialize MCP2515 device.
		 */
		if (MCP2515_Init(CAN_BITRATE) != MCP2515_OK) {
			return CAN_FAIL;
		}
		if (MCP2515_SetCanCtrlMode(MODE_NORMAL) != MCP2515_OK) {
			return CAN_FAIL;
		}
		return CAN_OK;
	#endif
	
	return CAN_FAIL;
}


/**
 * Sends a CAN message. The message will be put into the transmission queue,
 * and hence, it might not be sent immediately. If the queue is full, the
 * message will not be taken care of, and CAN_FAIL is returned.
 *
 * @param msg
 * 		Pointer to the CAN message.
 * 
 * @return
 * 		CAN_OK if the message was successfully enqueued.
 * 		CAN_FAIL if the transmission queue is full.
 */
Can_Return_t Can_Send(Can_Message_t *msg) {
	#if CAN_QUEUE_SIZE_TX == 0
		/* TX queue is disabled, so send immediately */
		return Can_SendImmediately(msg);
	#else
		#if 0
		printf("Can_Send()\n\r");
		printf("    queue size before = %u\n\r", canTxQueue.nrElements);
		#endif
		/* check if there is room available in the tx queue */
		if (canTxQueue.nrElements >= canTxQueue.size) {
			#if 0
			printf("    TX queue full!\n\r");
			#endif
			return CAN_FAIL;
		}
		/* put message in queue */
	    Can_Message_t *ptr = Can_QueueInsert(&canTxQueue);
	    memcpy(ptr, msg, sizeof(Can_Message_t));
	    #if 0
		printf("    queue size after = %u\n\r", canTxQueue.nrElements);
		#endif
		return CAN_OK;
	#endif
}


/**
 * Services the CAN subsystem. Messages waiting in the transmission queue
 * will be transmitted if the controller is not busy, and any messages
 * received by the controller will be moved from the controller buffers
 * into the internal reception queue. 
 */
void Can_Service() {
	#if CAN_QUEUE_SIZE_TX > 0
		/* try to transmit messages waiting in the transmission queue */
		for (uint8_t i=0; i<CAN_CONTROLLER_NR_TX_BUFFERS; i++) {
			if (canTxQueue.nrElements > 0) {
				#if 0
				printf("msg in TX queue. trying to send...\n");
				#endif
				/* ensure the queue operations are atomic */
				Mcu_DisableIRQ();
				Can_Message_t *txMsg = Can_QueueReadTailPtr(&canTxQueue);
				if (Can_SendImmediately(txMsg) == CAN_OK) {
					/* transmission OK, remove from queue */
					Can_QueueRemoveTail(&canTxQueue);
				}
				Mcu_EnableIRQ();
			}
			else {
				/* quit the loop if the queue is empty */
				break;
			}
		}
	#endif
	
	#if CAN_QUEUE_SIZE_RX > 0
		/* move all received messages from CAN controller to reception queue */
		Can_Message_t rxMsg;
		/* all RX buffers have to be checked (and in correct order) */
		for (uint8_t buf=0; buf<CAN_CONTROLLER_NR_RX_BUFFERS; buf++) {
			/* if buffer contains a message, handle it */
			if (Can_ReceiveFromController(&rxMsg, buf) == CAN_OK) {
				/* ensure the queue operations are atomic */
				Mcu_DisableIRQ();
				/* try to get access to the queue */
				Can_Message_t *destPtr = Can_QueueInsert(&canRxQueue);
				/* if we can insert into queue, copy the message */
				if (destPtr != 0) {
					memcpy(destPtr, &rxMsg, sizeof(Can_Message_t));
				}
				else {
					/* otherwise, the received message will be discarded... */
				}
				Mcu_EnableIRQ();
			}
		}
	#endif
	
	/* TODO: check error status in controller */
}


/**
 * Receives a CAN message that is waiting in the reception queue. If no
 * messages have been received, CAN_NO_MSG_AVAILABLE is returned.
 * Otherwise, a CAN message is copied into the specified message buffer
 * and deleted from the internal message reception queue. In this case,
 * CAN_OK is returned.
 * 
 * @param msg
 *          Pointer to the message storage buffer into which the message
 * 			should be copied.
 * 
 * @return
 * 			CAN_OK if a received message was successfully copied into the buffer.
 * 			CAN_NO_MSG_AVAILABLE if no messages are available.
 */ 
Can_Return_t Can_Receive(Can_Message_t *msg) {
    
    #if CAN_QUEUE_SIZE_RX > 0
	    /*
	     * RX queue is enabled, so try to get message from queue.
	     */
	    Can_Message_t *pSrc;	/* Source pointer */
		/* Check if there is anything available in the RxQueue */
	    if (canRxQueue.nrElements == 0) {
	    	return CAN_NO_MSG_AVAILABLE;
	    }
	    /* ensure the queue operations are atomic */
	    Mcu_DisableIRQ();
	    pSrc = Can_QueueReadTailPtr(&canRxQueue);
	    /* copy frame from queue, and then remove from queue */
		memcpy(msg, pSrc, sizeof(Can_Message_t));
		Can_QueueRemoveTail(&canRxQueue);
		Mcu_EnableIRQ();
		/* a message has successfully been copied and removed from queue */
		return CAN_OK;
	#else
		/*
		 * RX queue is disabled, so try to get message from controller.
		 * We don't know which buffer to get the message from, so best
		 * we can do is to get from buf0 first time, and then increase
		 * buffer number at each call, finally wrapping around at
		 * CAN_CONTROLLER_NR_RX_BUFFERS.
		 */
		static uint8_t rxBuffer = 0;
		/* worst case is that we need to check all available rx buffers in order to find a message */
		for (uint8_t i=0; i<CAN_CONTROLLER_NR_RX_BUFFERS; i++) {
			/* is there a message available in this buffer? */
			if (Can_ReceiveFromController(msg, rxBuffer) == CAN_OK) {
				/* increase buffer number and return the message */
				rxBuffer = (rxBuffer + 1) % CAN_CONTROLLER_NR_RX_BUFFERS;
				return CAN_OK;
			}
			/* increase buffer number so we can check next buffer */
			rxBuffer = (rxBuffer + 1) % CAN_CONTROLLER_NR_RX_BUFFERS;
		}
		/* all buffers were checked, but no message found */
		return CAN_NO_MSG_AVAILABLE;
	#endif
}



/*-----------------------------------------------------------------------------
 * Private Functions
 *---------------------------------------------------------------------------*/  

/**
 * Sends a CAN message immediately with the controller hardware. If the CAN
 * controller is busy, the function will return CAN_SEND_FAIL_TX_BUSY.
 *
 * @param msg
 *		Pointer to the CAN message storage buffer.
 * 
 * @return
 *		CAN_OK if the message was successfully sent to the controller.
 *		CAN_FAIL if the controller is busy.
 */
static Can_Return_t Can_SendImmediately(Can_Message_t* msg) {
	#if CAN_CONTROLLER == CAN_CONTROLLER_NULL
		/*
		 * Send with null device.
		 */
		return CAN_OK;
	#endif

	#if CAN_CONTROLLER == CAN_CONTROLLER_MCP2515
		/*
		 * Send with MCP2515 device.
		 */
		uint8_t res, txbuf_n;
		res = MCP2515_GetNextFreeTXBuf(&txbuf_n); // info = addr.
		if (res == MCP_ALLTXBUSY) {
			return CAN_FAIL;
		}
		MCP2515_WriteCanMsg(txbuf_n, msg);
		MCP2515_StartTransmit(txbuf_n);
		return CAN_OK;
	#endif
}


/**
 * Receives a CAN message from the CAN controller hardware.
 * 
 * @param msg
 *		Pointer to the message storage buffer into which the message should be copied.
 *
 * @param rxBuffer
 * 		Identifies the RX buffer in the controller. Range is [0,CAN_CONTROLLER_NR_RX_BUFFERS-1].
 * 
 * @return
 *		CAN_OK if a received message was successfully copied into the buffer.
 *		CAN_NO_MSG_AVAILABLE if there are is no message available in the specified buffer.
 * 		CAN_FAIL if the rxBuffer parameter i out of range for the specified controller.
 */
static Can_Return_t Can_ReceiveFromController(Can_Message_t *msg, uint8_t rxBuffer) {
	#if CAN_CONTROLLER == CAN_CONTROLLER_NULL
		/*
		 * Receive from null device.
		 */
		return CAN_NO_MSG_AVAILABLE;
	#endif
	
	#if CAN_CONTROLLER == CAN_CONTROLLER_MCP2515
		/*
		 * Receive from MCP2515 device.
		 */
		uint8_t stat;
		stat = MCP2515_ReadStatus();
		if (rxBuffer == 0) {
			/* check BUF0 */
			if (stat & MCP_STAT_RX0IF) {
				/* Msg in Buffer 0 */
				MCP2515_ReadCanMsg(MCP_RXBUF_0, msg);
				MCP2515_ModifyRegister(MCP_CANINTF, MCP_RX0IF, 0);
				return CAN_OK;
			}
		}
		else if (rxBuffer == 1) {
			/* check BUF1 */
			if (stat & MCP_STAT_RX1IF) {
				/* Msg in Buffer 1 */
				MCP2515_ReadCanMsg(MCP_RXBUF_1, msg);
				MCP2515_ModifyRegister(MCP_CANINTF, MCP_RX1IF, 0);
				return CAN_OK;
			}
		}
		else {
			/* invalid parameters */
			return CAN_FAIL;
		}
		return CAN_NO_MSG_AVAILABLE;
	#endif
}


/**
 * Checks how many messages are available in the CAN controller hardware.
 * 
 * @return
 * 		The number of messages available (0 if none).
 */
static uint8_t Can_MessagesAvailableInController() {
	#if CAN_CONTROLLER == CAN_CONTROLLER_NULL
		/*
		 * Check for messages in null device.
		 */
		return 0;
	#endif
	
	#if CAN_CONTROLLER == CAN_CONTROLLER_MCP2515
		/*
		 * Check for message in MCP2515 device.
		 */
		uint8_t res;
		res = MCP2515_ReadStatus(); /* RXnIF in Bit 1 and 0 */
		if (res & MCP_STAT_RXIF_MASK) {
			//TODO: check how many messages are available
			return 1;	/* at least one message available */
		}
		else {
			return 0;	/* no messages available */
		}
	#endif
	
	return 0;
}


/*
 * Checks Controller-Error-State.
 * 
 * @return
 * 		CAN_OK if the controller is OK.
 *		CAN_FAIL if errors have occured.
 * 
 * @todo
 * 		Styr upp denna funktion lite bättre.
 */
static Can_Return_t Can_ControllerCheckError() {
	#if CAN_CONTROLLER == CAN_CONTROLLER_NULL
		/*
		 * Check for errors in null device.
		 */
		return CAN_OK;
	#endif
	
	#if CAN_CONTROLLER == CAN_CONTROLLER_MCP2515
		/*
		 * Check for errors in MCP2515 device.
		 */
		uint8_t eflg = MCP2515_ReadRegister(MCP_EFLG);
		if (eflg & MCP_EFLG_ERRORMASK) {
			return CAN_FAIL;	/* errors found */
		}
		else {
			return CAN_OK;		/* no errors found */
		}
	#endif
}


#if (CAN_QUEUE_SIZE_TX > 0) || (CAN_QUEUE_SIZE_RX > 0)
/**
 * Initializes a CAN queue.
 * 
 * @param q
 * 		Pointer to the queue object.
 * 
 * @param size
 * 		Size of the queue (number of messages that can be queued).
 * 
 * @param dataPtr
 * 		Pointer to the message storage buffer that should be used by the queue.
 */
static void Can_QueueInit(Can_MessageQueue_t *q, uint8_t size, Can_Message_t *dataPtr) {
	q->dataPtr = dataPtr;
	q->head = 0;
	q->tail = 0;
	q->nrElements = 0;
	q->size = size;
}


/**
 * Inserts a new element in a queue and returns a pointer to the new element.
 * If the queue is full, the last element (the tail) will be overwritten by
 * the new element.
 * 
 * @param q
 * 		Pointer to the queue object.
 * 
 * @return
 * 		Pointer to the new message element. Use this pointer to copy data into the queue.
 */
static Can_Message_t* Can_QueueInsert(Can_MessageQueue_t *q) {
	Can_Message_t *ptr;
	#if 0
	printf("INSERT: bef=%u, ", q->nrElements);
	#endif
	if ((q->nrElements+1) > q->size) {
		q->tail = (q->tail + 1) % q->size;
		q->nrElements--;
	#if 0
		printf("Can_QueueInsertViaPtr: QUEUE OVERRUN!\n\r");
	#endif
    }
    ptr = &(q->dataPtr[q->head]);
    #if 0
    if (q->handled[q->head] == 0) {
    	printf("Can_QueueInsertViaPtr: discarding non-handled msg!\n\r");
    }
    #endif
    q->head = (q->head + 1) % q->size;
	q->nrElements++;
	#if 0
	printf("aft=%u\n\n", q->nrElements);
	#endif
	return ptr;
}


/**
 * Returns a pointer to the last element in the queue (the tail). Use this pointer
 * to extract the message, and then call Can_QueueRemoveTail to actually remove
 * the element.
 * 
 * @param q
 * 		Pointer to the queue object.
 * 
 * @return
 * 		Pointer to the tail message element.
 */
static Can_Message_t* Can_QueueReadTailPtr(Can_MessageQueue_t *q) {
	if (q->nrElements > 0) {
		return &(q->dataPtr[q->tail]);
	}
	else {
		return 0;
	}
}


/**
 * Removes the last element in the queue (the tail). You should have extracted
 * the data using Can_QueueReadTailPtr before using this function.
 * 
 * @param q
 * 		Pointer to the queue object.
 */
static void Can_QueueRemoveTail(Can_MessageQueue_t *q) {
	if (q->nrElements > 0) {
		q->tail = (q->tail + 1) % q->size;
		q->nrElements--;
		#if 0
		printf("queue size = %u\n", q->nrElements);
		#endif
	}
}

#endif
