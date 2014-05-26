#include <inttypes.h>
//#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <drivers/can/avr_internal/avr_internal.h>

/* Hack to fix lack of PBx, PCx defines in /usr/lib/avr/include/avr/portpins.h */
/* http://savannah.nongnu.org/bugs/?25930 http://www.mail-archive.com/avr-libc-dev@nongnu.org/msg03306.html */
#include <drivers/mcu/portpins.h>

#include <vectors.h>
#include <drivers/can/avr_internal/can_lib.h>

static Can_Message_t msgbuf;

Can_Return_t Can_Receive(Can_Message_t *msg);
/*
ISR(MCP_INT_VECTOR) {
	// Get first available message from controller and pass it to
	// application handler. If both RX buffers contain messages
	// we will get another interrupt as soon as this one returns.
	if (Can_Receive(&msgbuf) == CAN_OK) {
		// Callbacks are run with global interrupts disabled but
		// with controller flag cleared so another msg can be
		// received while this one is processed.
#if MCP_CAN_PROCESS_RENAMED == 1
		// when running a gateway application which uses stdcan
		Can_Process_USART(&msgbuf);
#else
		Can_Process(&msgbuf);
#endif
	}
}
*/
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
Can_Return_t Can_Init(void) {
	can_init(0);
	return CAN_OK;
}

/**
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
Can_Return_t Can_Send(Can_Message_t *msg) {
	#define DATA_BUFFER_SIZE 8 // Up to 8 bytes Max
	uint8_t tx_remote_buffer[DATA_BUFFER_SIZE];
	st_cmd_t tx_remote_msg; // 
	tx_remote_msg.pt_data = &tx_remote_buffer[0]; // Point Remote Tx MOb to first element of buffer
	tx_remote_msg.status = 0; // clear status

	// Configure Remote Tx MOb
	for(uint8_t i=0; i<DATA_BUFFER_SIZE; i++) {tx_remote_buffer[i]=msg->Data.bytes[i];} // set data
	tx_remote_msg.id.ext = msg->Id; // 
	tx_remote_msg.ctrl.ide = msg->ExtendedFlag; // This message object sends extended (2.0A) CAN frames
	tx_remote_msg.ctrl.rtr = msg->RemoteFlag; // 
	tx_remote_msg.dlc = msg->DataLength; // Number of data bytes (8 max) 
	tx_remote_msg.cmd = CMD_TX_DATA; // 

	while(can_cmd(&tx_remote_msg) != CAN_CMD_ACCEPTED); // Wait for MOb to configure (Must re-configure MOb for every transaction) and send request

	while(can_get_status(&tx_remote_msg) == CAN_STATUS_NOT_COMPLETED); // Wait for Tx to complete

	return CAN_OK;
}


/**
 * Receives a CAN message from the CAN controller hardware.
 * 
 * @param msg
 *		Pointer to the message storage buffer into which the message should be copied.
 *
 * @return
 *		CAN_OK if a received message was successfully copied into the buffer.
 *		CAN_NO_MSG_AVAILABLE if there is no message available in the controller.
 */
Can_Return_t Can_Receive(Can_Message_t *msg) {


	return CAN_OK;
}
