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
void IrTransceiver_Start(uint16_t *buffer);
uint8_t IrTransceiver_Poll(void);
uint8_t IrTransceiver_Transmit(uint16_t *buffer, uint8_t length);
//uint8_t IrReceive_CheckIdle(void);

#endif /*IRTRANSCEIVER_H_*/
