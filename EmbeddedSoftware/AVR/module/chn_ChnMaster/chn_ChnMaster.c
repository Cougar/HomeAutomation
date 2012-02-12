
#include "chn_ChnMaster.h"

struct chn_ChnMaster_listener_t {
    uint16_t channel_id;
    void (*update_func)(uint16_t, uint16_t); /* channel, value */
};

struct chn_ChnMaster_listener_t chn_ChnMaster_listeners[ chn_ChnMaster_MAX_LISTENERS ];
uint8_t chn_ChnMaster_listenCount = 0;

struct chn_ChnMaster_buffer_t {
    uint16_t channel_id;
    uint16_t value;
} chn_ChnMaster_buffer[ chn_ChnMaster_BUFFER_SIZE ];

volatile uint8_t chn_ChnMaster_buf_in_pos = 0; // next to write
volatile uint8_t chn_ChnMaster_buf_out_pos = 0; // next to read

#ifdef chn_ChnMaster_USEEEPROM
#include "chn_ChnMaster_eeprom.h"
struct eeprom_chn_ChnMaster EEMEM eeprom_chn_ChnMaster = 
{
    {
        ///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file. 
        0xAB,    // x
        0x1234    // y
    },
    0    // crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 
#endif

void chn_ChnMaster_RegisterListener( uint16_t channel_id, void (*update_func)(uint16_t, uint16_t) ) {
    chn_ChnMaster_listeners[ chn_ChnMaster_listenCount ].update_func = update_func;
    chn_ChnMaster_listeners[ chn_ChnMaster_listenCount ].channel_id = channel_id;
    chn_ChnMaster_listenCount++;
}

void chn_ChnMaster_UpdateChannel( uint16_t channel_id, uint16_t value ) {
    StdCan_Msg_t txMsg;
    txMsg.Id = 0;
    StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_CHN);

    txMsg.Id |= channel_id << 8;
    txMsg.Id |= chn_ChnMaster_ID;

    txMsg.Length = 2;
    txMsg.Data[0] = (value     >>8)&0xFF;
    txMsg.Data[1] = (value        )&0xFF;

    while( StdCan_Put(&txMsg) != StdCan_Ret_OK );
}


void chn_ChnMaster_Init(void) {
}

void chn_ChnMaster_Process(void)
{
    uint16_t value;
    uint16_t channel_id;
    uint8_t i;
    while( chn_ChnMaster_buf_out_pos != chn_ChnMaster_buf_in_pos ) {
        value      = chn_ChnMaster_buffer[chn_ChnMaster_buf_out_pos].value;
        channel_id = chn_ChnMaster_buffer[chn_ChnMaster_buf_out_pos].channel_id;

        for( i=0; i<chn_ChnMaster_listenCount; i++ ) {
            if( chn_ChnMaster_listeners[i].channel_id == channel_id ) {
                (*chn_ChnMaster_listeners[i].update_func)( channel_id, value );
            }
        }

        chn_ChnMaster_buf_out_pos = (chn_ChnMaster_buf_out_pos+1)%chn_ChnMaster_BUFFER_SIZE;
    }
    ///TODO: Stuff that needs doing is done here
}

void chn_ChnMaster_HandleMessage(StdCan_Msg_t *rxMsg)
{
    uint8_t j;
    uint16_t channel_id;
    uint16_t value;
    if ( StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_CHN)
    {
        channel_id = (rxMsg->Id >> 8) & 0xFFFF;
        if( (rxMsg->Id & (1L<<24)) == 0 ) { /* Value */
            for( j=0; j<(rxMsg->Length&~1); j+=2 ) {
                value      = (rxMsg->Data[j]<<8) | rxMsg->Data[j+1];
/*
                for( i=0; i<chn_ChnMaster_listenCount; i++ ) {
                    if( chn_ChnMaster_listeners[i].channel_id == channel_id ) {
                        (*chn_ChnMaster_listeners[i].update_func)( channel_id, value );
                    }
                }
                */
                chn_ChnMaster_buffer[chn_ChnMaster_buf_in_pos].channel_id = channel_id;
                chn_ChnMaster_buffer[chn_ChnMaster_buf_in_pos].value = value;
                chn_ChnMaster_buf_in_pos = (chn_ChnMaster_buf_in_pos+1)%chn_ChnMaster_BUFFER_SIZE;
                channel_id++;
            }
        } else { /* Metadata */
        }
    }
}

void chn_ChnMaster_List(uint8_t ModuleSequenceNumber)
{
    StdCan_Msg_t txMsg;
    
    StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_CHN);
    StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
    txMsg.Header.ModuleType = CAN_MODULE_TYPE_CHN_CHNMASTER;
    txMsg.Header.ModuleId = chn_ChnMaster_ID;
    txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_LIST;
    txMsg.Length = 6;

	uint32_t HwId=BIOS_GetHwId();
	txMsg.Data[0] = HwId&0xff;
	txMsg.Data[1] = (HwId>>8)&0xff;
	txMsg.Data[2] = (HwId>>16)&0xff;
	txMsg.Data[3] = (HwId>>24)&0xff;
    
    txMsg.Data[4] = NUMBER_OF_MODULES;
    txMsg.Data[5] = ModuleSequenceNumber;
    
    while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}
