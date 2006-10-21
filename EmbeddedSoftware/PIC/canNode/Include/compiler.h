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

#define UART_BAUD_RATE       (19200)     // bps

#define SPBRG_VAL   ( ((INSTR_FREQ/UART_BAUD_RATE)/16) - 1)

#if (SPBRG_VAL > 255)
    #error "Calculated SPBRG value is out of range for currnet CLOCK_FREQ."
#endif

#define LED0_TRIS		(TRISCbits.TRISC1)
#define LED0_IO			(PORTCbits.RC1)

#define UART_START_BYTE 253
#define UART_END_BYTE 250

#define UART_READ_TIMEOUT 30000

#endif //compiler.h
