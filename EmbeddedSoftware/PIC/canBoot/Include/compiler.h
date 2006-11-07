/*********************************************************************
 *
 *                  Compiler specific
 *
 *********************************************************************
 * FileName:        compiler.h
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Johan Böhlin     21-10-06 	Original        (Rev 1.0)
 ********************************************************************/

#ifndef COMPILER_H
#define COMPILER_H

#include <p18cxxx.h>
#define CLOCK_FREQ		(40000000)      // Hz
#define INSTR_FREQ			(CLOCK_FREQ/4)
#define CLOCK_FOSC		(40)      // MHz

#define LED0_TRIS		(TRISCbits.TRISC1)
#define LED0_IO			(PORTCbits.RC1)


#endif //compiler.h
