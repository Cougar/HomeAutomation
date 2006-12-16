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
 */
typedef struct {
    //TODO
} Com_Message_t;


/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
void Com_Init(void);
void Com_Service(void);
void ProtocolParseCanMessage(Can_Message_t *cmsg);


#endif /*COM_H_*/
