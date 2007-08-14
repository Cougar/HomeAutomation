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
#define IR_R_PORT	PORTB
#define IR_R_PIN	PINB
#define IR_R_DDR	DDRB
#define IR_R_BIT	PB0 // Input Capture Pin

#define IR_T_PORT	PORTD
#define IR_T_PIN	PIND
#define IR_T_DDR	DDRD
#define IR_T_BIT	PD6

#define MAX_NR_TIMES		101 								//max ir pulses+pauses, used for dimensioning the buffer 

/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
void IrTransceiver_Init(void);
void IrTransceiver_Start(void);
uint8_t IrTransceiver_Poll(uint16_t **buffer, uint8_t *length);
#if 0
uint8_t IrTransceiver_Transmit(uint16_t **buffer, uint8_t *length);
#endif
uint8_t IrTransceiver_Transmit(uint8_t proto, uint32_t data, uint16_t *timeout, uint8_t *repeates);
uint8_t IrTransceiver_Transmit_Poll(void);
void IrReceive_Init(void);
//uint8_t IrReceive_CheckIR(uint8_t *proto, uint8_t *address, uint8_t *command, uint16_t *timeout);
uint8_t IrReceive_CheckIR(uint8_t *proto, uint32_t *data, uint16_t *timeout);
uint8_t IrReceive_CheckIdle(void);
uint16_t getRawData(uint8_t index);
uint8_t getRawDataCnt(void);

/*-----------------------------------------------------------------------------
 * Private Function Prototypes
 *---------------------------------------------------------------------------*/


#endif /*IRTRANSCEIVER_H_*/
