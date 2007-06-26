#ifndef IRCOMMON_H_
#define IRCOMMON_H_

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <stdio.h>

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define IR_OK 				1
#define IR_FAILED 			-1	//?
#define IR_NO_PROTOCOL		2
#define IR_TIME_OVFL		3
#define IR_NO_DATA 			4
#define IR_NOT_CORRECT_DATA 5

#define CYCLES_PER_US       (F_CPU/1000000)

#define IR_PROTO_RC5		0
#define IR_RC5_ST_BIT		864*CYCLES_PER_US		//us
#define IR_RC5_BIT			864*2*CYCLES_PER_US
#define IR_RC5_REPETITION	105						//ms	(time between pulsetrains)
#define IR_RC5_F_MOD		36						//kHz	(modulation frequency)

#define IR_PROTO_RC6		1
#define IR_RC6_ST_BIT		1*CYCLES_PER_US			//us
#define IR_RC6_REPETITION	1						//ms	(time between pulsetrains)
#define IR_RC6_F_MOD		36						//kHz	(modulation frequency)

#define IR_PROTO_RCMM		2								//Wont work together with SHARP
#define IR_RCMM_ST_BIT		256*CYCLES_PER_US		//us
#define IR_RCMM_REPETITION 158						//ms	(time between pulsetrains)
#define IR_RCMM_F_MOD		36						//kHz	(modulation frequency)

#define IR_PROTO_SIRC		3
#define IR_SIRC_ST_BIT		2400*CYCLES_PER_US		//us
#define IR_SIRC_REPETITION	40						//ms	(time between pulsetrains)
#define IR_SIRC_F_MOD		40						//kHz	(modulation frequency)
#define IR_SIRC_ZERO_LOW	450*CYCLES_PER_US		//us
#define IR_SIRC_ZERO_HIGH	750*CYCLES_PER_US		//us
#define IR_SIRC_ONE_LOW		1050*CYCLES_PER_US		//us
#define IR_SIRC_ONE_HIGH	1350*CYCLES_PER_US		//us

#define IR_PROTO_SHARP		4								//Wont work together with RCMM
#define IR_SHARP_ST_BIT		275*CYCLES_PER_US		//us
#define IR_SHARP_REPETITION	55						//ms	(time between pulsetrains)
#define IR_SHARP_F_MOD		38						//kHz	(modulation frequency)



#endif /*IRCOMMON_H_*/
