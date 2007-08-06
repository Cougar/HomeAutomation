#ifndef CAN_H_
#define CAN_H_

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <drivers/can/canid.h>

/*-----------------------------------------------------------------------------
 * Type Definitions
 *---------------------------------------------------------------------------*/

/**
 * CAN Return Type. Most CAN functions return one of these. 
 */
typedef enum {
	CAN_FAIL = -1,					/* The function failed */
	CAN_OK = 1,						/* The function succeeded */
	
	CAN_MSG_AVAILABLE = 2,			/* Messsages are available */
	CAN_NO_MSG_AVAILABLE = -2,		/* Messages are NOT available */
	
	//CAN_CTRLERROR = -4
} Can_Return_t;

#define CAN_FLAG_EXT 0x01
#define CAN_FLAG_RTR 0x80

/**
 * CAN Message Type. Stores a complete CAN frame.
 */
typedef struct {
    /*
     * ID section. Contains either 29bit ID or 11bit ID (depending on ExtendedFlag).
     */
    uint32_t Id;
    
    /*
     * DLC section. Contains the data length expressed in number of bytes. Range [0,8].
     */
    uint8_t DataLength;
    
    /**
     * Extended frame flag. Set to 1 if the ID is 29bit, or 0 if it is 11bit.
     */
    uint8_t Flags;
    
    /*
     * DATA section. Contains the 8 data bytes.
     */
    union {
        /* read as 8bit unsigned data */
        uint8_t bytes[8];
        
        /* read as 16bit unsigned data */
        uint16_t words[4];
        
        /* read as 32bit unsigned data */
        uint32_t dwords[2];
    } Data;
} Can_Message_t;

#endif /*CAN_H_*/
