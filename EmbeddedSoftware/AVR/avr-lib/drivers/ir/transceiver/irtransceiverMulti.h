/** 
 * @defgroup infrared IR Driver Library
 * @code #include <drivers/ir/transceiver/transceiver.h> @endcode
 * 
 * @brief  
 *
 *
 * @author	Andreas Fritiofson, Anders Runeson
 * @date	2007
 */

/**@{*/

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

#ifndef IRTRANS_NUM_IR_REC
#define IRTRANS_NUM_IR_REC 1
#endif

#ifndef IR_TRANS_CH0_MASK
#define IR_TRANS_CH0_MASK PB0
#endif

#define TRUE 1
#define FALSE 0

/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
uint8_t IrTransceiver_Transmit(uint16_t *buffer, uint8_t length, uint8_t modfreq);
uint8_t IrTransceiver_Transmit_Poll(void);

void IrTransceiver_Init(void);
uint8_t IrTransceiver_GetStoreEnable(uint8_t channel);
void IrTransceiver_Disable(uint8_t channel);
void IrTransceiver_Enable(uint8_t channel);
void IrTransceiver_Reset(uint8_t channel);

//void IrTransceiver_Init_TX_Channel(uint8_t channel, uint16_t *buffer, irCallback_t callback, uint8_t pcint_id, volatile uint8_t* port, volatile uint8_t* pin, volatile uint8_t* ddr,uint8_t nr, uint8_t pcint);

/**
 * @brief Type of the callback function pointer
 * parameters: channel, buffer, length
 */
typedef void (*irCallback_t)(uint8_t, uint16_t*, uint8_t);


/*-----------------------------------------------------------------------------
 * Private Function Prototypes
 *---------------------------------------------------------------------------*/

void IrTransceiver_Store(uint8_t channel);

#endif /*IRTRANSCEIVER_H_*/
