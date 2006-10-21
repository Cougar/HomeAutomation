#ifndef CAN_H_
#define CAN_H_

#include <inttypes.h>

#define CANDEBUG   1

#define CANUSELOOP 0

#define CANSENDTIMEOUT (200) /* milliseconds */

// initial value of gCANAutoProcess
#define CANAUTOPROCESS (1)
#define CANAUTOON  (1)
#define CANAUTOOFF (0)

#define CAN_STDID (0)
#define CAN_EXTID (1)

#define CANDEFAULTIDENT    (0x55CC)
#define CANDEFAULTIDENTEXT (CAN_EXTID)

#define CAN_20KBPS   (1)
#define CAN_125KBPS  (CAN_20KBPS+1)

#define CAN_OK         (0)
#define CAN_FAILINIT   (1)
#define CAN_FAILTX     (2)
#define CAN_MSGAVAIL   (3)
#define CAN_NOMSG      (4)
#define CAN_CTRLERROR  (5)
#define CAN_FAIL       (0xff)

#define CAN_MAX_CHAR_IN_MESSAGE (8)

typedef struct {
	// identifier CAN_xxxID
	uint8_t  extended_identifier; 
	// either extended (the 29 LSB) or standard (the 11 LSB)
	uint32_t identifier; 
	// data length:
	uint8_t  dlc;
	uint8_t  dta[CAN_MAX_CHAR_IN_MESSAGE];
	
	// used for receive only:
	// Received Remote Transfer Bit 
	//  (0=no... 1=remote transfer request received)
	uint8_t  rtr;  
	// Acceptence Filter that enabled the reception
	uint8_t  filhit;
} CanMessage;

extern uint8_t gCANAutoProcess;

uint8_t can_init(uint8_t speedset);

void can_initMessageStruct(CanMessage* msg);
uint8_t can_sendMessage(const CanMessage* msg);
uint8_t can_checkReceive(void);
uint8_t can_readMessage(CanMessage *msg);
uint8_t can_checkError(void);

#if (CANDEBUG)
void can_debug1(void);
void can_dumpMessage(CanMessage *msg);
uint8_t can_testTransmit(const uint8_t ext, const uint8_t data2);
void can_dumpStatus(void);
#endif

#endif
