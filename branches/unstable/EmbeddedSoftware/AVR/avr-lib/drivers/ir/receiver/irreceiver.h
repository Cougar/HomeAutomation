#ifndef IRRECEIVE_H_
#define IRRECEIVE_H_


/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <drivers/ir/ircommon.h>

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define IRPORT	PORTD
#define IRPIN	PIND
#define IRDDR	DDRD
#define IRBIT	PD2

#define MAX_NR_TIMES		100 								//max ir pulses+pauses, used for dimensioning the buffer 

/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
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


#endif /*IRRECEIVE_H_*/
