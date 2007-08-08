#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <config.h> // All configuration parameters
#include <bios.h>   // BIOS interface declarations, including CAN structure and ID defines.
#include "channel.h"

#define APP_TYPE    0xf001
#define APP_VERSION 0x0002

// A simple message "queue", with space for one message only.
// These are declared volatile to tell the compiler not to optimize away accesses.
volatile Can_Message_t rxMsg; // Message storage
volatile uint8_t rxMsgFull;   // Synchronization flag

uint16_t dim_val = 0x7070;

// CAN message reception callback.
// This function runs with interrupts disabled, keep it as short as possible.
void can_receive(Can_Message_t *msg) {
    if (!rxMsgFull) {
        memcpy((void*)&rxMsg, msg, sizeof(rxMsg));
        rxMsgFull = 1;
    }
}

void Channel_Callback_Value( uint8_t id, uint16_t value ) {
    dim_val = value;
}

int main(void)
{
    uint16_t val = 0;
    
    Can_Message_t txMsg;

    DDRD |= (1<<PORTD4);
    // Enable interrupts as early as possible
    sei();

    txMsg.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) | (NODE_ID << CAN_SHIFT_NMT_SID);
    txMsg.DataLength = 4;
    txMsg.RemoteFlag = 0;
    txMsg.ExtendedFlag = 1;
    txMsg.Data.words[0] = APP_TYPE;
    txMsg.Data.words[1] = APP_VERSION;
    
    // Set up callback for CAN reception, this is optional if only sending is required.
    BIOS_CanCallback = &can_receive;
    // Send CAN_NMT_APP_START
    BIOS_CanSend(&txMsg);

    Channel_Init();

    Channel_Bind( 0, 3 );
    Channel_SetValue( 0, 0x3333 );

    for(;;) {
        val+=27;
        if( val<dim_val ) {
            PORTD |= (1<<PORTD4);
        } else {
            PORTD &= ~(1<<PORTD4);
        }
        if (rxMsgFull) {
            Channel_HandleMsg( (Can_Message_t *)&rxMsg );
            rxMsgFull = 0;
        }
    }
    
    return 0;
}
