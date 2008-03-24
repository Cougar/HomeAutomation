#include <inttypes.h>
#include <bios.h>
#include <config.h>
#include "channel.h"

Channel_Info_t channel_defs[ CHANNEL_COUNT ];

uint8_t Channel_Init( void ) {
    Can_Message_t txMsg;

    uint8_t i;

    /* Reset channel_defs */
    for(i=0;i<CHANNEL_COUNT;i++) {
        channel_defs[i].channel = 0;
        channel_defs[i].value = 0;
    }

    /* CHANNEL_SUBCLASS_IDENTIFY */

    txMsg.Id    = (CAN_CHN                      << CAN_SHIFT_CLASS)
                | (CHANNEL_SUBCLASS_IDENTIFY    << CHANNEL_SHIFT_SUBCLASS)
                | (NODE_ID                      << CHANNEL_SHIFT_NODEID);
    txMsg.ExtendedFlag = 1;
    txMsg.RemoteFlag = 0;

    txMsg.DataLength = 1;
    txMsg.Data.bytes[0] = CHANNEL_COUNT;

    BIOS_CanSend( &txMsg );



    return CHANNEL_OK;
}

uint8_t Channel_HandleMsg(Can_Message_t *msg) {
    uint8_t class;
    uint8_t subclass;
    uint8_t node_id;

    uint16_t channel;
    uint16_t channel_mask;

    uint8_t i,j;

    if( msg->ExtendedFlag!=1) {
        /* Channels only availible in Extended mode */
        return CHANNEL_FAIL;
    }
    class = ( msg->Id & CAN_MASK_CLASS ) >> CAN_SHIFT_CLASS;
    if( class != CAN_CHN ) {
        /* Is not a channel-message */
        return CHANNEL_FAIL;
    }

    subclass    = ( msg->Id & CHANNEL_MASK_SUBCLASS ) >> CHANNEL_SHIFT_SUBCLASS;
    channel     = ( msg->Id & CHANNEL_MASK_CHANNEL  ) >> CHANNEL_SHIFT_CHANNEL;
    node_id     = ( msg->Id & CHANNEL_MASK_NODEID   ) >> CHANNEL_SHIFT_NODEID;

    switch( subclass ) {
        case CHANNEL_SUBCLASS_DATA:
            channel_mask = 0x0FFF & ~((msg->DataLength>>1)-1);
            for( i=0; i<CHANNEL_COUNT; i++ ) {
                if( ( (channel_defs[i].channel) & channel_mask ) == channel ) {
                    channel_defs[i].value = msg->Data.words[ channel_defs[i].channel - channel ];
                    Channel_Callback_Value( i, channel_defs[i].value );
                }
            }
            return CHANNEL_OK;



/* Node-to-macro
        case CHANNEL_SUBCLASS_IDENTIFY:
            break;
*/

        case CHANNEL_SUBCLASS_CHN_BIND:
            if( NODE_ID == node_id && msg->Data.bytes[0] < CHANNEL_COUNT ) {
                channel_defs[ msg->Data.bytes[0] ].channel = channel;
                return CHANNEL_OK;
            }
            return CHANNEL_FAIL;



#if CHANNEL_ENABLE_NAME
        case CHANNEL_SUBCLASS_CHN_NAME:
            for( i=0; i<CHANNEL_COUNT; i++ ) {
                if( channel_defs[i].channel == channel ) {
                    for( j=0; j<msg->DataLength; j++) {
                        channel_defs[i].name[j]=msg->Data.bytes[j];
                    }
                    channel_defs[i].name[j]='\0';
                }
            }
            return CHANNEL_OK;
#endif

#if CHANNEL_ENABLE_TYPE
        case CHANNEL_SUBCLASS_CHN_TYPE:
            for( i=0; i<CHANNEL_COUNT; i++ ) {
                if( channel_defs[i].channel == channel ) {
                    channel_defs[i].type_k = msg->Data.words[0];
                    channel_defs[i].type_m = msg->Data.words[1];
#if CHANNEL_ENABLE_TYPE_NAME
                    for( j=0; j<msg->DataLength-4; j++) {
                        channel_defs[i].type[j]=msg->Data.bytes[j+4];
                    }
                    channel_defs[i].type[j]='\0';
#endif
                }
            }
            break;
#endif
    }

    return CHANNEL_FAIL;
}

void Channel_Bind( uint8_t id, uint8_t channel ) {
    channel_defs[id].channel = channel;
}

uint16_t Channel_GetValue( uint8_t id ) {
    return channel_defs[id].value;
}

uint8_t Channel_SetValue( uint8_t id, uint16_t value ) {
    uint16_t channel;
    Can_Message_t txMsg;

    channel = channel_defs[id].channel;
    if( channel == 0 ) return CHANNEL_FAIL;

    txMsg.Id    = (CAN_CHN                  << CAN_SHIFT_CLASS)
                | (CHANNEL_SUBCLASS_DATA    << CHANNEL_SHIFT_SUBCLASS)
                | (channel                  << CHANNEL_SHIFT_CHANNEL)
                | (NODE_ID                  << CHANNEL_SHIFT_NODEID);

    txMsg.ExtendedFlag = 1;
    txMsg.RemoteFlag = 0;
    txMsg.DataLength = 2;
    txMsg.Data.words[0] = value;

    BIOS_CanSend( &txMsg );

    channel_defs[id].value = value;

#if CHANNEL_SET_SELF
    Channel_Callback_Value( id, value );
#endif

    return CHANNEL_OK;
}

Channel_Info_t *Channel_GetStorage( uint8_t id ) {
    return &channel_defs[id];
}
