
#include "chn_lcd_control.h"

#ifdef CHN_LOADED
#include "../chn_ChnMaster/chn_ChnMaster.h"
#endif

#define CHANNEL_FLAG_WRITABLE  0x01
#define CHANNEL_FLAG_ENABLED   0x02
#define CHANNEL_FLAG_UNKNOWN   0x04
#define CHANNEL_FLAG_REDRAW    0x80

struct chn_lcd_control_channel_t {
    uint16_t id;
    char     name[6];
    char     type[3];
    uint16_t value;
    uint16_t low;
    uint16_t range;
    uint16_t low_disp;
    uint16_t range_disp;
    int8_t   dot;
    uint8_t  flags;
};


void rotary_pcint_callback(uint8_t id, uint8_t status);
void chn_lcd_control_drawChannel( uint8_t channel );
void chn_lcd_control_chn_update( uint16_t channel_id, uint16_t value );
void chn_lcd_control_timeout( uint8_t timer );


#if chn_lcd_control_DRIVER_LOADED == 0
uint8_t lcd_framebuffer[256]; /* FIXME: a lib or something here... */
#else
extern uint8_t lcd_framebuffer[256]; /* FIXME: a lib or something here... */
#endif



uint8_t controller_editmode = 1;
volatile int8_t rotaryEncoder_Position = 0;
uint8_t rotaryEncoder_Button_Position = 0;
uint8_t rotaryEncoder_Button_Position_old = 0;

uint8_t channels_count         = 8;
uint8_t channel_sel            = 0;
uint8_t channel_border_visible = 0;

struct chn_lcd_control_channel_t channels[8] = {
    {0, "Ceil", "%",  0,     0, 65535, 0, 100, -1, CHANNEL_FLAG_UNKNOWN | CHANNEL_FLAG_WRITABLE | CHANNEL_FLAG_ENABLED },
    {1, "Win",  "%",  0,     0, 65535, 0, 100, -1, CHANNEL_FLAG_UNKNOWN | CHANNEL_FLAG_WRITABLE | CHANNEL_FLAG_ENABLED },
    {3, "Test", "%",  0,     0, 65535, 0, 100, -1, CHANNEL_FLAG_UNKNOWN | CHANNEL_FLAG_WRITABLE | CHANNEL_FLAG_ENABLED },
    {4, "",     "",   0,     0, 65535, 0, 100, -1, 0},
    {5, "",     "",   0,     0, 65535, 0, 100, -1, 0},
    {6, "",     "",   0,     0, 65535, 0, 100, -1, 0},
    {7, "",     "",   0,     0, 65535, 0, 100, -1, 0},
    {2, "Out",  "C ", 0, 25600,  7680, 0, 300,  3, CHANNEL_FLAG_UNKNOWN |                         CHANNEL_FLAG_ENABLED }
};

void chn_lcd_control_timeout( uint8_t timer ) {
    controller_editmode    = 0;
    channel_border_visible = 0;
    channels[ channel_sel ].flags |= CHANNEL_FLAG_REDRAW;
}

void chn_lcd_control_enable( void ) {
    channel_border_visible = 1;
    channels[ channel_sel ].flags |= CHANNEL_FLAG_REDRAW;
    Timer_SetTimeout(chn_lcd_control_rot_TIMER, 3000, TimerTypeOneShot, &chn_lcd_control_timeout);
}


void rotary_pcint_callback(uint8_t id, uint8_t status) {
    uint8_t rot_data = 0;
    static uint8_t rot_lastdir = 0, rot_laststate = 0;

    //Take care of rotary encoder movement
    if(gpio_get_state(ROTARY_CH1)){
        rot_data |= 0x01;
    }
    if(gpio_get_state(ROTARY_CH2)){
        rot_data |= 0x02;
    }

    if( rot_data==0 || rot_data==3 ){ // Are both signals high or low?
        if( rot_data==0 && rot_laststate!=rot_data ){ // Are both signals low? In that case we are finished with one turn and should print out the direction it went.
            if( rot_lastdir&0x01 ){
            #if ROTARY_CHx_INVERT_DIRECTION==1
                rotaryEncoder_Position--;    // Moving clockwise
            #else
                rotaryEncoder_Position++;    // Moving counter clockwise
            #endif
            }else{
            #if ROTARY_CHx_INVERT_DIRECTION==1
                rotaryEncoder_Position++;    // Moving counter clockwise
            #else
                rotaryEncoder_Position--;    // Moving clockwise
            #endif
            }
        }
        rot_laststate = rot_data;
    } else { // No, only one of the signals are high. We can use this to find out what direction we are moving.
        rot_lastdir = rot_data;
    }
} 


void chn_lcd_control_chn_update( uint16_t channel_id, uint16_t value ) {
    uint8_t i;
    for( i = 0; i < channels_count; i++ ) {
        if( channels[i].id == channel_id ) {
            channels[i].value = value;
            channels[i].flags &= ~CHANNEL_FLAG_UNKNOWN;
            channels[i].flags |= CHANNEL_FLAG_REDRAW;
        }
    }
}

void chn_lcd_control_drawChannel( uint8_t channel ) {
    uint8_t *fb = lcd_framebuffer + channel*4;
    uint8_t i;
    int32_t value;
    int8_t normvalue = 23;
    int8_t tilevalue;
    struct chn_lcd_control_channel_t *chn = &channels[ channel ];

    if( !( chn->flags & CHANNEL_FLAG_ENABLED ) )
        return;

    /* Draw frame */
    if( channel_border_visible && channel == channel_sel ) {
        fb[0] = 157; fb[1] = 158; fb[2] = 158; fb[3] = 159;
        if( controller_editmode ) {
            for( i=1; i<8; i++ ) {
                fb[i*32] = 189; fb[i*32+3] = 191;
            }
        } else {
            for( i=1; i<8; i++ ) {
                fb[i*32] = 0; fb[i*32+3] = 0;
            }
        }
        fb[224] = 221; fb[225] = 222; fb[226] = 222; fb[227] = 223;
    } else {
        fb[0] = 0; fb[1] = 0; fb[2] = 0; fb[3] = 0;
        for( i=1; i<8; i++ ) {
            fb[i*32] = 0; fb[i*32+3] = 0;
        }
        fb[224] = 0; fb[225] = 0; fb[226] = 0; fb[227] = 0;
    }

    /* Draw name */
    for( i=0; chn->name[i]; i++ ) {
        fb[(i+1)*32] = chn->name[i];
    }

    if( !( chn->flags & CHANNEL_FLAG_UNKNOWN ) ) {
        /* Draw value */
        value  = chn->value;
        value -= chn->low;
        value *= chn->range_disp;
        value /= chn->range;
        value += chn->low_disp;

        i = 0;
        while( chn->type[i] ) {
            fb[ 195 - 32*i ] = chn->type[i];
            i++;
        }
        do {
            if( chn->dot == i ) {
                fb[ 195 - 32*i ] = '.';
            } else {
                fb[ 195 - 32*i ] = '0' + (value%10);
                value /= 10;
            }
            i++;
        } while( value > 0 );
        if( i<=6 ) {
            fb[ 195 - 32*i ]  = 0;
        }


        /* Scale value */
        value = chn->value - chn->low;
        if( value < 0 ) value = 0;
        if( value > chn->range ) value = chn->range;

        value = value * 44 / chn->range;

        normvalue = value + 2; /* Base of meter isn't at bottom of tile */

    }

    /* Draw meter */


    if( chn->flags & CHANNEL_FLAG_UNKNOWN ) {
        tilevalue = 9;
    } else {
        if( normvalue < 0 ) normvalue = 0;
        tilevalue = normvalue;
        if( tilevalue > 8 ) tilevalue = 8;
        normvalue -= 8;
    }
    tilevalue *= 2;

    fb[32*6 + 1] = 192+tilevalue; fb[32*6 + 2] = 193+tilevalue;
    for( i=5; i>=2; i-- ) {
        if( !( chn->flags & CHANNEL_FLAG_UNKNOWN ) ) {
            if( normvalue < 0 ) normvalue = 0;
            tilevalue = normvalue;
            if( tilevalue > 8 ) tilevalue = 8;
            normvalue -= 8;
            tilevalue *= 2;
        }

        fb[32*i + 1] = 160+tilevalue; fb[32*i + 2] = 161+tilevalue;
    }
    if( !( chn->flags & CHANNEL_FLAG_UNKNOWN ) ) {
        if( normvalue < 0 ) normvalue = 0;
        tilevalue = normvalue;
        if( tilevalue > 8 ) tilevalue = 8;
        normvalue -= 8;
        tilevalue *= 2;
    }

    fb[32*1 + 1] = 128+tilevalue; fb[32*1 + 2] = 129+tilevalue;

}

void chn_lcd_control_Init(void)
{
    uint8_t i;
    ///TODO: Initialize hardware etc here

    // to use PCINt lib, call this function: (the callback function look as a timer callback function)
    // Pcint_SetCallbackPin(chn_lcd_control_PCINT, EXP_C , &chn_lcd_control_pcint_callback);

    for( i = 0; i < channels_count; i++ ) {
        chn_lcd_control_drawChannel( i );
#ifdef CHN_LOADED
        chn_ChnMaster_RegisterListener( channels[i].id, chn_lcd_control_chn_update );
#endif
    }

    /*
     * Initialize rotaryencoders and buttons
     */
    gpio_set_in(ROTARY_CH1);    // Set to input
    gpio_set_pullup(ROTARY_CH1);    // Enable pull-up
    gpio_set_in(ROTARY_CH2);    // Set to input
    gpio_set_pullup(ROTARY_CH2);    // Enable pull-up
    gpio_set_in(ROTARY_BTN);    // Set to input
    gpio_set_pullup(ROTARY_BTN);    // Enable pull-up

    // Enable IO-pin interrupt
    Pcint_SetCallbackPin(chn_lcd_control_rot_PCINT_CH1, ROTARY_CH1, &rotary_pcint_callback);
    Pcint_SetCallbackPin(chn_lcd_control_rot_PCINT_CH2, ROTARY_CH2, &rotary_pcint_callback);
//    Pcint_SetCallbackPin(chn_lcd_control_rot_PCINT_BTN, ROTARY_BTN, &rotary_pcint_callback);

}

void chn_lcd_control_Process(void)
{
    int8_t rotary_pos = rotaryEncoder_Position;
    int32_t value;
    uint8_t old_channel,i;

    rotaryEncoder_Position = 0;

    if( rotary_pos != 0 ) {
        chn_lcd_control_enable();
    }

    if( controller_editmode ) {
        if( channel_sel < channels_count && rotary_pos != 0 ) {
            value = channels[ channel_sel ].value;
            value += 2048L * (int32_t)rotary_pos;
            if( value < 0 ) value = 0;
            if( value > 65535 ) value = 65535;
            channels[ channel_sel ].value = value;

#ifdef CHN_LOADAED
            chn_ChnMaster_UpdateChannel( channels[ channel_sel ].id, value );
#endif

            channels[ channel_sel ].flags &= ~CHANNEL_FLAG_UNKNOWN;
            channels[ channel_sel ].flags |= CHANNEL_FLAG_REDRAW;
        }
    } else {
        old_channel = channel_sel;
        while( rotary_pos > 0 ) {
            do {
                channel_sel = (channel_sel+1)%channels_count;
            } while( !( channels[ channel_sel ].flags & CHANNEL_FLAG_WRITABLE ) );
            rotary_pos--;
        }
        while( rotary_pos < 0 ) {
            do {
                channel_sel = (channel_sel+channels_count-1)%channels_count;
            } while( !( channels[ channel_sel ].flags & CHANNEL_FLAG_WRITABLE ) );
            rotary_pos++;
        }
        if( old_channel != channel_sel ) {
            channels[ old_channel ].flags |= CHANNEL_FLAG_REDRAW;
            channels[ channel_sel ].flags |= CHANNEL_FLAG_REDRAW;
        }
    }


    //Take care of button push
    rotaryEncoder_Button_Position = gpio_get_state(ROTARY_BTN);
    if( rotaryEncoder_Button_Position && !rotaryEncoder_Button_Position_old ) {
        chn_lcd_control_enable();
        controller_editmode = !controller_editmode;
        channels[ channel_sel ].flags |= CHANNEL_FLAG_REDRAW;
    }
    rotaryEncoder_Button_Position_old = rotaryEncoder_Button_Position;

    for( i=0; i<channels_count; i++ ) {
        if( channels[ i ].flags & CHANNEL_FLAG_REDRAW ) {
            channels[ i ].flags &= ~CHANNEL_FLAG_REDRAW;
            chn_lcd_control_drawChannel( i );
        }
    }

}

void chn_lcd_control_HandleMessage(StdCan_Msg_t *rxMsg)
{
}

void chn_lcd_control_List(uint8_t ModuleSequenceNumber)
{
    StdCan_Msg_t txMsg;
    
    StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_CHN);
    StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
    txMsg.Header.ModuleType = CAN_MODULE_TYPE_CHN_CHNCONTROLLER;
    txMsg.Header.ModuleId = chn_lcd_control_ID;
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
