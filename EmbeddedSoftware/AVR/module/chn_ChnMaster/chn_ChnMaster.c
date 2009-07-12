
#include "chn_ChnMaster.h"

struct chn_ChnMaster_listener_t {
    uint16_t channel_id;
    void (*update_func)(uint16_t, uint16_t); /* channel, value */
};

struct chn_ChnMaster_listener_t chn_ChnMaster_listeners[ chn_ChnMaster_MAX_LISTENERS ];
uint8_t chn_ChnMaster_listenCount = 0;



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


void chn_ChnMaster_Init(void)
{
#ifdef chn_ChnMaster_USEEEPROM
    if (EEDATA_OK)
    {
      ///TODO: Use stored data to set initial values for the module
      blablaX = eeprom_read_byte(EEDATA.x);
      blablaY = eeprom_read_word(EEDATA.y);
    } else
    {    //The CRC of the EEPROM is not correct, store default values and update CRC
      eeprom_write_byte_crc(EEDATA.x, 0xAB, WITHOUT_CRC);
      eeprom_write_word_crc(EEDATA.y, 0x1234, WITHOUT_CRC);
      EEDATA_UPDATE_CRC;
    }
#endif  
    ///TODO: Initialize hardware etc here

    // to use PCINt lib, call this function: (the callback function look as a timer callback function)
    // Pcint_SetCallbackPin(chn_ChnMaster_PCINT, EXP_C , &chn_ChnMaster_pcint_callback);

}

void chn_ChnMaster_Process(void)
{
    ///TODO: Stuff that needs doing is done here
}

void chn_ChnMaster_HandleMessage(StdCan_Msg_t *rxMsg)
{
    uint8_t i,j;
    uint16_t channel_id;
    uint16_t value;
    if ( StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_CHN)
    {
        if( (rxMsg->Id & (1L<<24)) == 0 ) { /* Value */
            channel_id = (rxMsg->Id >> 8) & 0xFFFF;

            for( j=0; j<(rxMsg->Length&~1); j+=2 ) {
                value      = (rxMsg->Data[j]<<8) | rxMsg->Data[j+1];
                for( i=0; i<chn_ChnMaster_listenCount; i++ ) {
                    if( chn_ChnMaster_listeners[i].channel_id == channel_id ) {
                        (*chn_ChnMaster_listeners[i].update_func)( channel_id, value );
                    }
                }
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

    txMsg.Data[0] = NODE_HW_ID_BYTE0;
    txMsg.Data[1] = NODE_HW_ID_BYTE1;
    txMsg.Data[2] = NODE_HW_ID_BYTE2;
    txMsg.Data[3] = NODE_HW_ID_BYTE3;
    
    txMsg.Data[4] = NUMBER_OF_MODULES;
    txMsg.Data[5] = ModuleSequenceNumber;
    
    while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}
