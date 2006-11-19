#ifndef CAN_H_
#define CAN_H_

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <can_cfg.h>


/*-----------------------------------------------------------------------------
 * Type Definitions
 *---------------------------------------------------------------------------*/
/**
 * CAN Bitrate type.
 */
typedef enum {
    CAN_BITRATE_125K = 0,
    CAN_BITRATE_250K = 1,
    CAN_BITRATE_500K = 2,
    CAN_BITRATE_1M = 3
} Can_Bitrate_t;

/**
 * CAN Return Type. Most CAN functions return one of these. 
 */
typedef enum {
	CAN_OK = 0,
	CAN_FAILINIT = 1,
	CAN_FAILTX = 2,
	CAN_MSGAVAIL = 3,
	CAN_NOMSG = 4,
	CAN_CTRLERROR = 5,
	CAN_FAIL = 0xFF
} Can_Return_t;

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
    uint8_t ExtendedFlag;
    
    /**
     * Remote frame flag. Set to 1 if the frame is a remote frame. 0 otherwise.
     */
    uint8_t RemoteFlag;
    
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


/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/

Can_Return_t Can_Init(void);
Can_Return_t Can_Send(Can_Message_t* msg);
Can_Return_t Can_ReceiveAvailable(void);
Can_Return_t Can_Receive(Can_Message_t *msg);
Can_Return_t Can_CheckError(void);

#if (CANDEBUG)
void can_debug1(void);
void can_dumpMessage(CanMessage *msg);
uint8_t can_testTransmit(const uint8_t ext, const uint8_t data2);
void can_dumpStatus(void);
#endif

#endif
