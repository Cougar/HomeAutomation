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
 * @brief Return values.
 * 
 * CAN Return Type. Most CAN functions return one of these.
 */
typedef enum {
	CAN_FAIL = -1,				/**< The function failed. */
	CAN_OK = 1,					/**< The function succeeded. */
	
	CAN_MSG_AVAILABLE = 2,		/**< Messsages are available. */
	CAN_NO_MSG_AVAILABLE = -2,	/**< Messages are NOT available. */
	
} Can_Return_t;

/**
 * @brief Message structure.
 * 
 * CAN Message Type. Stores a complete CAN frame.
 */
typedef struct {
    uint32_t Id;				/**< ID section. Contains either 29bit ID or 11bit ID (depending on ExtendedFlag). */
    uint8_t DataLength;			/**< DLC section. Contains the data length expressed in number of bytes. Range [0,8]. */
    uint8_t ExtendedFlag;		/**< Extended frame flag. Set to 1 if the ID is 29bit, or 0 if it is 11bit. */
    uint8_t RemoteFlag;			/**< Remote frame flag. Set to 1 if the frame is a remote frame. 0 otherwise. */
    
    /**
     * DATA section. Contains the 8 data bytes.
     */
    union {
        uint8_t bytes[8];		/**< read as 8bit unsigned data */
        uint16_t words[4];		/**< read as 16bit unsigned data */
        uint32_t dwords[2];		/**< read as 32bit unsigned data */
    } Data;
} Can_Message_t;

#endif /*CAN_H_*/
