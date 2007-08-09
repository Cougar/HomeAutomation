/**********************************************************
 * Channel based subsystem for HomeAutomation
 *
 * Author: Max Sikstrom
 *
 **********************************************************/

#ifndef CHANNEL_H_
#define CHANNEL_H_

/**********************************************************
 * Includes
 **********************************************************/
#include <inttypes.h>
#include <bios.h>

/**********************************************************
 * Constants
 **********************************************************/
#define CAN_CHANNEL     0x0aUL

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

/**********************************************************
 * Internal storage structure
 **********************************************************/
typedef struct {
    uint16_t    channel;
    uint16_t    value;

#if CHANNEL_ENABLE_NAME
    char        name[9];
#endif

#if CHANNEL_ENABLE_TYPE
    uint16_t    type_k;
    uint16_t    type_m;

#   if CHANNEL_ENABLE_TYPE_NAME
    char        type[5];
#   endif
#endif

} Channel_Info_t;

/**********************************************************
 * Functions
 **********************************************************/

/* Initialize channel subsystem
 *
 * Must be done after can initializes.
 */
uint8_t Channel_Init( void );

/* Handle a channel CAN-message.
 *
 * Returns CHANNEL_OK if ok, CHANNEL_FAIL otherwise
 */
uint8_t Channel_HandleMsg( Can_Message_t *msg );

/* Bind a channel to function id
 *
 * This can, and should, be done from a macronode through the bus
 */
void Channel_Bind( uint8_t id, uint8_t channel );

/* Get a value unscaled from a channel referenced by a function id.
 */
uint16_t Channel_GetValue( uint8_t id );

/* Set a value unscaled to a channeld referenced by a function id.
 */
uint8_t Channel_SetValue( uint8_t id, uint16_t value );

/* Get internal storage for function id
 */
Channel_Info_t *Channel_GetStorage( uint8_t id );

/**********************************************************
 * Callbacks
 **********************************************************/

/* A callback routine for handling a valuechange in a channel
 */
void Channel_Callback_Value( uint8_t id, uint16_t value );

#endif
