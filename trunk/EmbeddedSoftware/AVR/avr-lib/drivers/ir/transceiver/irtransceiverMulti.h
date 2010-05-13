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

/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
void IrTransceiver_Init(void);
void IrTransceiver_Receive_Start(uint16_t *buffer);
uint8_t IrTransceiver_Receive_Poll(uint8_t *len);
uint8_t IrTransceiver_Transmit(uint16_t *buffer, uint8_t length, uint8_t modfreq);
uint8_t IrTransceiver_Transmit_Poll(void);

/**
 * @brief Type of the callback function pointer
 * parameters: channel, buffer, length
 */
typedef void (*irCallback_t)(uint8_t, uint16_t, uint8_t);


/*-----------------------------------------------------------------------------
 * Private Function Prototypes
 *---------------------------------------------------------------------------*/

void IrTransceiver_Store(uint8_t channel);

#endif /*IRTRANSCEIVER_H_*/