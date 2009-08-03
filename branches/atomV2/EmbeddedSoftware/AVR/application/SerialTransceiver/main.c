#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
#include <drivers/uart/serial.h>
#include <drivers/uart/uart.h>
#include <drivers/timer/timer.h>
#include <drivers/timer/timebase.h>
#include <drivers/can/can.h>
#include <drivers/can/mcp2515/mcp2515.h>

#define APP_TYPE    CAN_APPTYPES_SERIALTRANSCEIVER
#define APP_VERSION 0x0001

//Note: Erik Calissendorff, 2009-03-29
//Not sure how this is supposed to be used in the CAN message but it's part of the SNS id
#define SERIAL_ID_DATA	0

//What is the maximum time  that we will wait for addition data from the serial port
//before we send it in a package, the minimum time if data is available is max-1
#define RECEIVE_WAIT_TIME_MS 5

// A simple message "queue", with space for one message only.
// These are declared volatile to tell the compiler not to optimize away accesses.
volatile Can_Message_t rxMsg; // Message storage
volatile uint8_t rxMsgFull;   // Synchronization flag

// CAN message reception callback.
// This function runs with interrupts disabled, keep it as short as possible.
void can_receive(Can_Message_t *msg) {
	if (!rxMsgFull) {
		memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
		rxMsgFull = 1;
	}
}

int main(void)
{
	// Enable interrupts as early as possible
	sei();

	Timebase_Init();
	Serial_Init();

	Can_Message_t txMsg; // Message storage

	txMsg.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) |
		(NODE_ID << CAN_SHIFT_NMT_SID);
	txMsg.DataLength = 4;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.Data.words[0] = APP_TYPE;
	txMsg.Data.words[1] = APP_VERSION;

	// Set up callback for CAN reception, this is optional if only sending is required.
	BIOS_CanCallback = &can_receive;

	// Send CAN_NMT_APP_START for the transceiver
	BIOS_CanSend(&txMsg);

	//Buffer for incoming serial data
	uint8_t rxBuffer[SERIAL_RX_BUFFER_SIZE];

	//Ensure that we don't try to buffer more than the buffer size
	uint8_t maxReceiveData=8>SERIAL_RX_BUFFER_SIZE?SERIAL_RX_BUFFER_SIZE:8;

	//For loop variable
	uint8_t i;

	//How many bytes have been received from the serial port
	uint8_t received_size=0;

	//The last received data from the serial port
	unsigned int received_data;

	//Used to allow multiple bytes to be send in the same package
	uint32_t time = Timebase_CurrentTime();

	//Check if we got some data from the serial port
	uint8_t got_received_data=0;

	while (1) {

		if (rxMsgFull) {
			if ( ((rxMsg.Id & CAN_MASK_CLASS)>>CAN_SHIFT_CLASS) == CAN_ACT && ((uint16_t)((rxMsg.Id & CAN_MASK_ACT_TYPE) >> CAN_SHIFT_ACT_TYPE) == ACT_TYPE_SERIAL)) {
				//Unused uint8_t actid = (uint8_t)((rxMsg.Id & CAN_MASK_ACT_ID) >> CAN_SHIFT_ACT_ID);

				for(i=0;i<rxMsg.DataLength;i++)
				{
					uart_putc(rxMsg.Data.bytes[i]);
				}
			}

			// Flush the received message
			rxMsgFull = 0; //
		}

		//Receiver

		//Timer for how long we will wait for for more data before sending our packages
		time = Timebase_CurrentTime();

		//We don't like to wait for the time out unless we get some data.
		got_received_data=0;
		do
		{
			received_data=uart_getc();
			if(received_data != UART_NO_DATA)
			{
				got_received_data=1;
				//TODO: Erik Calissendorff, 2009-03-29
				//Error handling should be added to inform us of problems with the serial link

				rxBuffer[received_size++]=(uint8_t)(received_data & 0x00ff);
			}
		} while(received_size<maxReceiveData && got_received_data==1 && Timebase_CurrentTime()-time < RECEIVE_WAIT_TIME_MS);

		if(received_size > 0)
		{
			//TODO: Erik Calissendorff, 2009-03-29
			//We like to be able to accept more then one CAN package of data at a time

			txMsg.Id = ((CAN_SNS << CAN_SHIFT_CLASS) | (SNS_TYPE_SERIAL << CAN_SHIFT_SNS_TYPE) | (SERIAL_ID_DATA<<CAN_SHIFT_SNS_ID) | (NODE_ID << CAN_SHIFT_SNS_SID));
			for(i=0;i<received_size;i++)
			{
				txMsg.Data.bytes[i]=rxBuffer[i];
			}
			txMsg.DataLength = received_size;

			BIOS_CanSend(&txMsg);
		}
		//Ensure we are reseting the counter
		received_size=0;
	}
	return 0;
}

