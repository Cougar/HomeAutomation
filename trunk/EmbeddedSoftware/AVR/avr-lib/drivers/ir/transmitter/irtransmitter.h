#ifndef IRTRANSMITTER_H_
#define IRTRANSMITTER_H_


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
#define IRBIT	PD6

#define STATE_IDLE	0
#define STATE_TRANS	1

/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
void IrTransmit_Init(void);
uint8_t IrTransmit_Start(uint8_t proto, uint8_t address, uint8_t command);
uint8_t IrTransmit_Stop(void);
void IrTransmit_Poll(void);

/*-----------------------------------------------------------------------------
 * Private Function Prototypes
 *---------------------------------------------------------------------------*/
void setIrPort(void);
void clearIrPort(void);

#endif /*IRTRANSMITTER_H_*/
