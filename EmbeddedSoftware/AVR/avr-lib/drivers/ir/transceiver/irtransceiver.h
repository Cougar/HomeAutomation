#ifndef IRTRANSCEIVER_H_
#define IRTRANSCEIVER_H_

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <drivers/ir/ircommon.h>

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define IRPORT	PORTB
#define IRPIN	PINB
#define IRDDR	DDRB
#define IRBIT	PB0 // Input Capture Pin

#define MAX_NR_TIMES		101 								//max ir pulses+pauses, used for dimensioning the buffer 

/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
void IrTransceiver_Init(void);
void IrTransceiver_Start(void);
uint8_t IrTransceiver_Poll(uint16_t **buffer, uint8_t *length);
void IrReceive_Init(void);
uint8_t IrReceive_CheckIR(uint8_t *proto, uint8_t *address, uint8_t *command, uint16_t *timeout);
uint8_t IrReceive_CheckIdle(void);
uint16_t getRawData(uint8_t index);
uint8_t getRawDataCnt(void);

/*-----------------------------------------------------------------------------
 * Private Function Prototypes
 *---------------------------------------------------------------------------*/
uint8_t receiveRC5(uint8_t *address, uint8_t *command);
uint8_t receiveSIRC(uint8_t *address, uint8_t *command);


#endif /*IRTRANSCEIVER_H_*/
