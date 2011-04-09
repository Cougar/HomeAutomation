
#include "act_sigmadelta_dimmer.h"

#ifdef CHN_LOADED
#include "../chn_ChnMaster/chn_ChnMaster.h"

void act_sigmadelta_dimmer_update( uint16_t channel_id, uint16_t value );
#endif

uint8_t stepper = 0;

uint16_t inval = 0;
int32_t dim_level[act_sigmadelta_dimmer_channel_count];
int32_t dim_sigma[act_sigmadelta_dimmer_channel_count];

void act_sigmadelta_dimmer_chn_update( uint16_t channel_id, uint16_t value ) {
    channel_id -= act_sigmadelta_dimmer_base_channel; // Overflows for lower id
    if( channel_id < act_sigmadelta_dimmer_channel_count ) { // filters all channels due to overflow before.
        dim_level[channel_id] = value;
    }
}

void act_sigmadelta_dimmer_Init(void) {
    Timer_SetTimeout( act_sigmadelta_dimmer_SYS_TIMER, 1 , TimerTypeFreeRunning, 0);

// CDEA GFIH JKLM

    gpio_set_out( EXP_C );
    gpio_set_out( EXP_D );
    gpio_set_out( EXP_E );

    gpio_set_out( EXP_A );
    gpio_set_out( EXP_G );
    gpio_set_out( EXP_F );

    gpio_set_out( EXP_I );
    gpio_set_out( EXP_H );
    gpio_set_out( EXP_N );//J );

    gpio_set_out( EXP_B );//K );
    gpio_set_out( EXP_L );
    gpio_set_out( EXP_M );

#ifdef CHN_LOADED
    uint8_t i;
    for( i=0; i<act_sigmadelta_dimmer_channel_count; i++ ) {
        dim_level[i] = 0;
        dim_sigma[i] = 0;
        chn_ChnMaster_RegisterListener( act_sigmadelta_dimmer_base_channel + i, act_sigmadelta_dimmer_chn_update );
    }
#endif
}

void act_sigmadelta_dimmer_Process(void) {
    int32_t delta;
    uint8_t i;

    if( Timer_Expired( act_sigmadelta_dimmer_SYS_TIMER ) ) {
    }

    for( i = 0; i<act_sigmadelta_dimmer_channel_count; i++) {
        delta = dim_level[i] - ((dim_sigma[i] >= 0) ? 65536L : 0L);
        dim_sigma[i] += delta;
        if( dim_sigma[i] >  655360 ) dim_sigma[i] = 655360;
        if( dim_sigma[i] < -655360 ) dim_sigma[i] = -655360;
    }

    gpio_set_statement( dim_sigma[ 0]>=0, EXP_C );
    gpio_set_statement( dim_sigma[ 1]>=0, EXP_D );
    gpio_set_statement( dim_sigma[ 2]>=0, EXP_E );

    gpio_set_statement( dim_sigma[ 3]>=0, EXP_A );
    gpio_set_statement( dim_sigma[ 4]>=0, EXP_G );
    gpio_set_statement( dim_sigma[ 5]>=0, EXP_F );

    gpio_set_statement( dim_sigma[ 6]>=0, EXP_I );
    gpio_set_statement( dim_sigma[ 7]>=0, EXP_H );
    gpio_set_statement( dim_sigma[ 8]>=0, EXP_N ); //J );

    gpio_set_statement( dim_sigma[ 9]>=0, EXP_B ); //K );
    gpio_set_statement( dim_sigma[10]>=0, EXP_L );
    gpio_set_statement( dim_sigma[11]>=0, EXP_M );
}

void act_sigmadelta_dimmer_HandleMessage(StdCan_Msg_t *rxMsg) {
}

void act_sigmadelta_dimmer_List(uint8_t ModuleSequenceNumber) {
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_TST_DEBUG; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = act_sigmadelta_dimmer_ID;
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
