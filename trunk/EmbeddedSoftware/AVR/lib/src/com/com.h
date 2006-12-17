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

} Com_Message_t;


/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
void Com_Init(void);
void Com_Service(void);
void ProtocolParseCanMessage(Can_Message_t *cmsg);


#endif /*COM_H_*/
