#ifndef COM_H_
#define COM_H_


/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <can.h>


/*-----------------------------------------------------------------------------
 * Type Definitions
 *---------------------------------------------------------------------------*/
/**
 * Communication Message Type.
 *
 * IDENT:<FUNCT><FUNCC><NID><SID> DATA:<data7>...<data0>
 *
 */
typedef struct {

    /*
     * <FUNCT>, 4 bits = Function Type.
     */
	uint8_t Funct	:4;

    /*
     * <FUNCC>, 10 bits = Function Code.
     */
	uint16_t Funcc	:10;

    /*
     * <NID>, 6 bits = Network ID.
     */
	uint8_t Nid		:6;

    /*
     * <SID>, 9 bits = Sender ID.
     */
	uint16_t Sid	:9;
	
	/*
     * DATA section. Contains up to 8 data bytes.
     */
    union {
        /* read as 8bit unsigned data */
        uint8_t bytes[8];
        
        /* read as 16bit unsigned data */
        uint16_t words[4];
        
        /* read as 32bit unsigned data */
        uint32_t dwords[2];
    } Data;

} Com_Message_t;


/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
void Com_Init(void);
void Com_Service(void);
void ProtocolParseCanMessage(Can_Message_t *cmsg);


#endif /*COM_H_*/
