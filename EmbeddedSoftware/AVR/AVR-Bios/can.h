#ifndef CAN_H_
#define CAN_H_

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <inttypes.h>


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

//---------------------------------------------------------------------------
// CAN ID definitions

#define CAN_MASK_CLASS		0x1E000000
#define CAN_SHIFT_CLASS		25
//------------------------------------
#define CAN_NMT				0x00UL
#define CAN_MASK_NMT_TYPE	0x01F00000
#define CAN_SHIFT_NMT_TYPE	20
#define CAN_MASK_NMT_NID	0x000F0000
#define CAN_SHIFT_NMT_NID	16
#define CAN_MASK_NMT_SID	0x0000FF00
#define CAN_SHIFT_NMT_SID	8
#define CAN_MASK_NMT_RID	0x000000FF
#define CAN_SHIFT_NMT_RID	0
#define CAN_NMT_RESET		0x00UL
#define CAN_NMT_BIOS_START	0x01UL	//D0: VER7:0, D1:VER15:8, D2: 0 if no app, >0 if app
#define CAN_NMT_PGM_START	0x02UL	//D0: ADR7:0, D1:ADR15:8, D2:ADR23:16, D3:ADR31:24
#define CAN_NMT_PGM_DATA	0x03UL	//D0: OFS7:0, D1: OFS15:8, D(n+2): DATAn7:0 [n=0..x] [x=0..5]
#define CAN_NMT_PGM_END		0x04UL	//D0: CRC7:0, D1: CRC15:8
#define CAN_NMT_PGM_ACK		0x05UL	//D0: OFS7:0, D1: OFS15:8
#define CAN_NMT_PGM_NACK	0x06UL	//D0: OFS7:0, D1: OFS15:8
#define CAN_NMT_START_APP	0x07UL
#define CAN_NMT_APP_START	0x08UL	//D0: ID7:0, D1:ID15:8, D2: VER7:0, D3:VER15:8
#define CAN_NMT_HEARTBEAT	0x09UL
#define CAN_NMT_PGM_COPY	0x0FUL	//D0: SRC7:0, D1:SRC15:8, D0: DST7:0, D1:DST15:8, D0: LEN7:0, D1:LEN15:8
//------------------------------------
#define CAN_SNS				0x02UL
#define CAN_MASK_SNS_FUNCC	0x01FF8000
#define CAN_SHIFT_SNS_FUNCC	15
#define CAN_MASK_SNS_NID	0x00007E00
#define CAN_SHIFT_SNS_NID	9
#define CAN_MASK_SNS_SID	0x000001FF
#define CAN_SHIFT_SNS_SID	0
//------------------------------------
#define CAN_PKT				0x0c000000
//------------------------------------
#define CAN_CON				0x10000000
//------------------------------------
#define CAN_TST				0x1E000000
//------------------------------------

#endif
