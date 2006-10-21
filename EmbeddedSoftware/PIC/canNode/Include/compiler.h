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

#endif //compiler.h
