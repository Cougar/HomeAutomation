#ifndef IR_H_
#define IR_H_


/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <stdio.h>

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define IRPORT	PORTD
#define IRPIN	PIND
#define IRDDR	DDRD
#define IRBIT	PD2

#define IR_OK 				1
#define IR_FAILED 			-1	//?
#define IR_NO_PROTOCOL		2
#define IR_TIME_OVFL		3
#define IR_NO_DATA 			4

#define CYCLES_PER_US       (F_CPU/1000000)

#define IR_PROTO_RC5		0
#define IR_RC5_ST_BIT		864*CYCLES_PER_US		//us
#define IR_RC5_BIT			864*2*CYCLES_PER_US
#define IR_RC5_TIMEOUT		105		//ms

#define IR_PROTO_RC6		1
#define IR_RC6_ST_BIT		1*CYCLES_PER_US		//us
#define IR_RC6_TIMEOUT		1		//ms

#define IR_PROTO_RCMM		2
#define IR_RCMM_ST_BIT		256*CYCLES_PER_US		//us //Wont work together with SHARP
#define IR_RCMM_TIMEOUT		158		//ms

#define IR_PROTO_SIRC		3
#define IR_SIRC_ST_BIT		2400*CYCLES_PER_US	//us
#define IR_SIRC_TIMEOUT		40		//ms

#define IR_PROTO_SHARP		4
#define IR_SHARP_ST_BIT		275*CYCLES_PER_US		//us //Wont work together with RCMM
#define IR_SHARP_TIMEOUT	55		//ms

#define IR_MIN_REPEATE_PULSE_WIDTH	200
#define IR_MAX_REPEATE_PULSE_WIDTH	40000


/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
void initTimer(void);
int isTimerOvfl(void);
uint16_t getTimerVal(void);
void setTimerVal(uint16_t value);
int receiveRC5(uint8_t *address, uint8_t *command);
int checkIr(uint8_t *proto, uint8_t *address, uint8_t *command);
int checkIrIdle(void);
uint16_t getLastProtoTimeout(void);


#endif /*MCU_H_*/
