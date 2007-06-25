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

#define IR_MIN_REPEATE_PULSE_WIDTH	200
#define IR_MAX_REPEATE_PULSE_WIDTH	40000


/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
void IrReceive_Init(void);
uint8_t IrReceive_CheckIR(uint8_t *proto, uint8_t *address, uint8_t *command, uint16_t *timeout);
uint8_t IrReceive_CheckIdle(void);

/*-----------------------------------------------------------------------------
 * Private Function Prototypes
 *---------------------------------------------------------------------------*/
uint8_t receiveRC5(uint8_t *address, uint8_t *command);
uint8_t receiveSIRC(uint8_t *address, uint8_t *command);
void initTimer(void);
uint8_t isTimerOvfl(void);
uint16_t getTimerVal(void);
void setTimerVal(uint16_t value);


#endif /*IRRECEIVE_H_*/
