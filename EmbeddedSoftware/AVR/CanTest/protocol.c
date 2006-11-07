/**
 * Parser for the home automation protocol.
 *
 * @date	2006-10-29
 * @author	Jimmy Myhrman
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <protocol.h>
#include <can.h>

/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/

void ProtocolParseCanMessage(CanMessage_t *cmsg) {
	/* parse as protocol message */
	ProtocolMessage_t pmsg;
	pmsg.Header.Raw = cmsg->Id;
}
