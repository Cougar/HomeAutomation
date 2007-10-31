/**
 * High level functions for bus communication and standard node behaviour.
 * 
 * @author	Andreas Fritiofson
 * 
 * @date	2007-10-24
 */

#include <config.h>
#include "stdcan.h"
#if defined(_AVRLIB_BIOS_)
#include <bios.h>
#define Can_Send BIOS_CanSend
#else
#include <drivers/can/mcp2515/mcp2515.h>
#endif

#if (STDCAN_TX_QUEUE_SIZE > 1)
#error StdCan: Tx queue size longer than one msg not yet implemented.
#endif

/**
 * The receive queue.
 */
StdCan_Msg_t RxQ[STDCAN_RX_QUEUE_SIZE];
unsigned char RxQ_Rd_idx; /**< Receive queue read index. */
unsigned char RxQ_Wr_idx; /**< Receive queue write index. */
unsigned char RxQ_Len; /**< Number of messages in receive queue. */

#if (STDCAN_TX_QUEUE_SIZE > 1)
/**
 * The transmit queue.
 */
StdCan_Msg_t TxQ[STDCAN_TX_QUEUE_SIZE];
unsigned char TxQ_Rd_idx; /**< Transmit queue read index. */
unsigned char TxQ_Wr_idx; /**< Transmit queue write index. */
unsigned char TxQ_Len; /**< Number of messages in transmit queue. */
#endif

/**
 * CAN message callback.
 * Callback for when a message is received from the lower
 * layer (BIOS or CAN driver).
 */
void Can_Process(Can_Message_t* msg)
{
	unsigned char n;
	/* Check if there is room on the queue. */
	if (RxQ_Len < STDCAN_RX_QUEUE_SIZE) {
		/* Copy the message from lower layer into the queue. */
		RxQ[RxQ_Wr_idx].Id     = msg->Id;
		RxQ[RxQ_Wr_idx].Length = msg->DataLength;
		//TODO: This should be guarded against invalid DataLength.
		for (n = 0; n < RxQ[RxQ_Wr_idx].Length; n++) {
			RxQ[RxQ_Wr_idx].Data[n] = msg->Data.bytes[n];
		}
		/* Increment write index and queue length. */
		if (++RxQ_Wr_idx >= STDCAN_RX_QUEUE_SIZE) RxQ_Wr_idx = 0;
		RxQ_Len++;
	} else {
		/* Overflow, just drop the new message. In the future, some
		 * form of priority scheme could be used to drop another
		 * message in the queue.
		 */
	}
}

StdCan_Ret_t StdCan_Init(Node_Desc_t* node_desc)
{
	StdCan_Ret_t retval;
	
	/* Reset all queue variables. */
	RxQ_Rd_idx = 0;
	RxQ_Wr_idx = 0;
	RxQ_Len    = 0;
	TxQ_Rd_idx = 0;
	TxQ_Wr_idx = 0;
	TxQ_Len    = 0;
	
#if defined(_AVRLIB_BIOS_)
	/* Initialize BIOS' interface for CAN. */
	BIOS_CanCallback = Can_Process;
	retval = StdCan_Ret_OK;
#else
	/* Initialize CAN driver. */
	if (Can_Init() == CAN_OK)
		retval = StdCan_Ret_OK;
	else
		retval = StdCan_Ret_Fail;
#endif
	
	//TODO: Do something with the Node Descriptor.
	
	return retval;
}

StdCan_Ret_t StdCan_Get(StdCan_Msg_t* msg)
{
	unsigned char n;
	
	/* Check if there's a message waiting. */
	if (RxQ_Len) {
		
		/* Copy message to user buffer. */
		msg->Id     = RxQ[RxQ_Rd_idx].Id;
		msg->Length = RxQ[RxQ_Rd_idx].Length;
		//TODO: Consider a mempcy() of the entire message.
		for (n = 0; n < RxQ[RxQ_Rd_idx].Length; n++) {
			msg->Data[n] = RxQ[RxQ_Rd_idx].Data[n];
		}
		
		/* Increment read index and decrease queue length. */
		if (++RxQ_Rd_idx >= STDCAN_RX_QUEUE_SIZE) RxQ_Rd_idx = 0;
		RxQ_Len--;
		
		return StdCan_Ret_OK;
	} else {
		/* Queue is empty. */
		return StdCan_Ret_Empty;
	}
}

StdCan_Ret_t StdCan_Put(StdCan_Msg_t* msg)
{
	Can_Message_t Can_Msg;
	unsigned char n;
	
	/* Validate message. */
	if ((unsigned)msg->Length > 8) return StdCan_Ret_DataErr;
	
	/* Copy message directly to lower layer until a proper
	 * queue has been implemented.
	 * TODO: Implement a proper queue. This requires TX interrupt
	 * support in the driver and BIOS.
	 */
	Can_Msg.ExtendedFlag = 1;
	Can_Msg.RemoteFlag = 0;
	Can_Msg.Id = msg->Id;
	Can_Msg.DataLength = msg->Length;
	for (n = 0; n < msg->Length; n++) {
		Can_Msg->Data.bytes[n] = msg->Data[n];
	}
	
	if (Can_Send(&Can_Msg) == CAN_OK)
		return StdCan_Ret_OK;
	else
		return StdCan_Ret_Full;
}

void StdCan_SendHeartbeat(uint8_t n)
{
	/* TODO: When a Tx queue is implemented, the heartbeat should
	 * be sent via StdCan_Put instead of directly to lower layer.
	 */
	Can_Message_t Heartbeat;
	
	/* Set up Heartbeat message format. */
	Heartbeat.ExtendedFlag = 1;
	Heartbeat.RemoteFlag = 0;
	Heartbeat.DataLength = 0;
	Heartbeat.Id = (CAN_NMT << CAN_SHIFT_CLASS)
				 | (CAN_NMT_HEARTBEAT << CAN_SHIFT_NMT_TYPE)
				 | (NODE_ID << CAN_SHIFT_NMT_SID);
	
	/* Try to send it. */
	Can_Send(&Heartbeat);
}
