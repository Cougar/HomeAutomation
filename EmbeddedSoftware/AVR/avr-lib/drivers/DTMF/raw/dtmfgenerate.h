#ifndef DTMFGENERATE_H_
#define DTMFGENERATE_H_
/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define  PRESCALER  1                // timer1 prescaler
#define  N_SAMPLES  256              // Number of samples in lookup table
#define  FCK        F_CPU/PRESCALER   // Timer1 working frequency
#define  FP			256
#define  FPSHIFT	8

/*-----------------------------------------------------------------------------
 * Public Function Prototypes
 *---------------------------------------------------------------------------*/
uint8_t DTMFOUT_Init (void);

#endif /*DTMFGENERATE_H_*/
