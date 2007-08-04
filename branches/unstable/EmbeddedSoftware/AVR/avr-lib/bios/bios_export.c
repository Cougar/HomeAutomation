#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <config.h>
#include <drivers/can/can.h>
#include CAN_DRIVER_H

//---------------------------------------------------------------------------
// Function definitions for the exported interface
// These must all start with 'BIOS_' to be included in the library.

void BIOS_Reset(void) {
	// Just loop and let the watchdog reset
	cli(); 
	while(1);
}

Can_Return_t BIOS_CanSend(Can_Message_t* msg) {
	return Can_Send(msg);
}

