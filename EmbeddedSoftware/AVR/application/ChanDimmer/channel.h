#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <inttypes.h>
#include <bios.h>

#define CHANNEL_FAIL    0
#define CHANNEL_OK      1

#define CHANNEL_MASK_SUBCLASS   0x01F00000
#define CHANNEL_SHIFT_SUBCLASS  20
#define CHANNEL_MASK_CHANNEL    0x000FFF00
#define CHANNEL_SHIFT_CHANNEL   8
#define CHANNEL_MASK_NODEID     0x000000FF
#define CHANNEL_SHIFT_NODEID    0

#define CHANNEL_SUBCLASS_DATA       0x1UL
#define CHANNEL_SUBCLASS_IDENTIFY   0x2UL
#define CHANNEL_SUBCLASS_CHN_BIND   0x3UL
#define CHANNEL_SUBCLASS_CHN_NAME   0x4UL
#define CHANNEL_SUBCLASS_CHN_TYPE   0x5UL

#define CAN_CHANNEL     0x0aUL

typedef struct {
    uint16_t    channel;
    uint16_t    value;
    char        name[9];

    uint16_t    type_k;
    uint16_t    type_m;

    char        type[5];

} Channel_Info_t;

uint8_t Channel_Init( void );

uint8_t Channel_HandleMsg( Can_Message_t *msg );
void Channel_Bind( uint8_t id, uint8_t channel );
uint16_t Channel_GetValue( uint8_t id );

void Channel_Callback_Value( uint8_t id, uint16_t value );

#endif
