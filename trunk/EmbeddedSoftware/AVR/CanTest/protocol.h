#ifndef PROTOCOL_H_
#define PROTOCOL_H_


/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <can.h>


/*-----------------------------------------------------------------------------
 * Type Definitions
 *---------------------------------------------------------------------------*/
/**
 * Protocol Message Type.
 * A protocol message has the following structure:
 *		-------- 29bit IDENT --------- -- 8 bytes data --
 *		<FUNC><RID><SID><RIDEN><RESVD>  <data7>...<data0>
 */
typedef struct {
    /*
     * Header section.
     */
    union {
        uint32_t Raw;
        
        struct {
        	unsigned RESVD:2;
	        unsigned RIDEN:1;
	        unsigned SID:7;
	        unsigned RID:7;
	        unsigned FUNC:12;
	        unsigned __unused:3;	/* pad to 32bit */
        } Fields;
    } Header;
    
    /*
     * Data section.
     */
    union {
    	/* read as 8bit unsigned data */
        uint8_t bytes[8];
        
        /* read as 16bit unsigned data */
        uint16_t words[4];
        
        /* read as 32bit unsigned data */
        uint32_t dwords[2];
    } Data; 
} ProtocolMessage_t;


/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
void ProtocolParseCanMessage(CanMessage_t *cmsg);


#endif /*PROTOCOL_H_*/
