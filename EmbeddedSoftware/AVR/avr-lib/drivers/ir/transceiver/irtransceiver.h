#ifndef IRTRANSCEIVER_H_
#define IRTRANSCEIVER_H_

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <stdio.h>

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
void IrTransceiver_Receive_Start(uint16_t *buffer);
uint8_t IrTransceiver_Receive_Poll(void);
uint8_t IrTransceiver_Receive_Pause_Poll(void);
uint8_t IrTransceiver_Transmit(uint16_t *buffer, uint8_t length, uint8_t modfreq);
uint8_t IrTransceiver_Transmit_Poll(void);


/*-----------------------------------------------------------------------------
 * Private Function Prototypes
 *---------------------------------------------------------------------------*/


#endif /*IRTRANSCEIVER_H_*/
