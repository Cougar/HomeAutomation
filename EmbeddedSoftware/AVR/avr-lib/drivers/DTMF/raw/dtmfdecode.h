#ifndef DTMFDECODE_H_
#define DTMFDECODE_H_

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <avr/pgmspace.h>
#include <math.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define SAMPLING_RATE		8000
#define MAX_BINS			8			//nr of frequencies to calculate
#define GOERTZEL_N			92			//nr of samples to collect 

#define COEFFSPREC			1024		//precision for fixed-point values in coefficients
#define COEFFSPRECSHIFT		10
#define SAMPLEPREC			512			//precision for fixed-point values in calculations with samples
#define SAMPLEPRECSHIFT		9

#define UPPERSILENCELIMIT	132			//limits for when a dc-signal is treated like silence
#define LOWERSILENCELIMIT	122

#define BUFFER_SIZE 		GOERTZEL_N

#define TONE_BUFFER_SIZE	8

#define	RSPAUSE				0
#define	RSRECEIVING			1

#define CLK_PER_SAMPLE		(F_CPU/SAMPLING_RATE)
#define CLK_RELOAD			(uint16_t)(0xffff-CLK_PER_SAMPLE+1)
//#define CLK_RELOAD			(uint16_t)(0xffff-35000+1)

/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
uint8_t DTMFIN_Init(void);
uint8_t DTMFIN_GetData(uint8_t* returntone);

#endif /*DTMFDECODE_H_*/
