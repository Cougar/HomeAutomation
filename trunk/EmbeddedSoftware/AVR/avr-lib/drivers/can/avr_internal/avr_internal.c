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

#define DATA_BUFFER_SIZE 8 // Up to 8 bytes Max

static Can_Message_t msgbuf;
static uint8_t receive_buffer[DATA_BUFFER_SIZE]; // buffer to hold payload from Sensor Nodes
static st_cmd_t receive_msg; // Response Mob

Can_Return_t Can_Receive(Can_Message_t *msg);

Can_Return_t Can_ActivateRxMOb(void)
{
	receive_msg.pt_data = &receive_buffer[0]; // Point Response MOb to first element of buffer
	receive_msg.status = 0; // clear status
	for(uint8_t i=0; i<DATA_BUFFER_SIZE; i++) {receive_buffer[i] = 0;} // clear message object data buffer
	//response_msg.id.ext = 0; 
	receive_msg.ctrl.ide = 1; // This message object accepts extended (2.0A) CAN frames
	receive_msg.ctrl.rtr = 0; // this message object is not requesting a remote node to transmit data back
	//response_msg.dlc = DATA_BUFFER_SIZE; // Number of bytes (8 max) of data to expect
	receive_msg.cmd = CMD_RX_DATA; // assign this as a "Receive Standard (2.0A) CAN frame" message object

	while(can_cmd(&receive_msg) != CAN_CMD_ACCEPTED); // Wait for MOb to configure (Must re-configure MOb for every transaction)

	return CAN_OK;
}

// /usr/lib/avr/include/avr/iom64c1.h
ISR(CAN_INT_vect) {
	// Get first available message from controller and pass it to
	// application handler. If both RX buffers contain messages
	// we will get another interrupt as soon as this one returns.
	if (Can_Receive(&msgbuf) == CAN_OK) {
		// Callbacks are run with global interrupts disabled but
		// with controller flag cleared so another msg can be
		// received while this one is processed.
		Can_Process(&msgbuf);
		
	}
	//???
	Can_ActivateRxMOb();
	/* Clear interrupt flag */
	CANGIT |= (1<<BXOK);
}


Can_Return_t Can_EnableRxInterrupt(void) {
	CANGIE |= (1<<ENIT|1<<ENRX);
	
	return CAN_OK;
}


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
	Can_ActivateRxMOb();
	Can_EnableRxInterrupt();
	
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
	if (can_get_status(&receive_msg) == CAN_STATUS_COMPLETED) {
		msg->Id = receive_msg.id.ext;
		msg->DataLength = receive_msg.dlc;
		msg->RemoteFlag = receive_msg.ctrl.rtr;
		for(uint8_t i=0; i<DATA_BUFFER_SIZE; i++) {msg->Data.bytes[i]=receive_buffer[i];} // set data
		
		return CAN_OK;
	}

	return CAN_NO_MSG_AVAILABLE;
}
