
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

#if (STDCAN_FILTER)
/**
 * The message acceptance filters.
 */
typedef struct {
	char Active; /**< True if this filter should be used. */
	unsigned long Id; /**< Match if id matches the message id in all bit locations that are not masked. */
	unsigned long Mask; /**< Each bit specifies if the corresponding id bit must match (mask[n] = 1) or is Don't Care (mask[n] = 0). */
} StdCan_Filter_t;

StdCan_Filter_t RxFilters[STDCAN_NUM_FILTERS];
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
		
#if (STDCAN_FILTER)
		/* Try to match each filter in turn. */
		for (n = 0; n < STDCAN_NUM_FILTERS; n++) {
			if (RxFilters[n].Active && !((msg->Id ^ RxFilters[n].Id) & RxFilters[n].Mask)) {
				RxQ[RxQ_Wr_idx].Match = n;
				break;
			}
		}
		
		if (n == STDCAN_NUM_FILTERS) return; // No match found, discard message.
#endif
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
#if (STDCAN_TX_QUEUE_SIZE > 1)
	TxQ_Rd_idx = 0;
	TxQ_Wr_idx = 0;
	TxQ_Len    = 0;
#endif
	
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
	//(why have constats passed as parameters? they are defined at compiletime /arune)
#if defined(_AVRLIB_BIOS_)
	/* TODO: When a Tx queue is implemented, the startup message should
	 * be sent via StdCan_Put instead of directly to lower layer.
	 */
	Can_Message_t Startup;
	
	/* Set up Startup message format. */
	Startup.ExtendedFlag = 1;
	Startup.RemoteFlag = 0;
	Startup.DataLength = 4;
	Startup.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) | (NODE_ID << CAN_SHIFT_NMT_SID);
	Startup.Data.bytes[1] = APP_TYPE&0xff;
	Startup.Data.bytes[0] = (APP_TYPE>>8)&0xff;
	Startup.Data.bytes[3] = APP_VERSION&0xff;
	Startup.Data.bytes[2] = (APP_VERSION>>8)&0xff;
	
	/* Try to send it. */
	Can_Send(&Startup);
#endif
	
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

unsigned char StdCan_Get_Pending(void)
{
	return RxQ_Len;
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
		Can_Msg.Data.bytes[n] = msg->Data[n];
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

#if (STDCAN_FILTER)
StdCan_Ret_t StdCan_EnableFilter(unsigned char filter, unsigned long id, unsigned long mask)
{
	if (filter < STDCAN_NUM_FILTERS) {
		RxFilters[filter].Id = id;
		RxFilters[filter].Mask = mask;
		RxFilters[filter].Active = 1;
		return StdCan_Ret_OK;
	} else {
		return StdCan_Ret_DataErr;
	}
}

StdCan_Ret_t StdCan_DisableFilter(unsigned char filter)
{
	if (filter < STDCAN_NUM_FILTERS) {
		RxFilters[filter].Active = 0;
		return StdCan_Ret_OK;
	} else {
		return StdCan_Ret_DataErr;
	}
}
#endif
