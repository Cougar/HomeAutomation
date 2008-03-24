/*********************************************************************
 *
 *                  Compiler specifics like clockspeed and portIOs.
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/

#ifndef COMPILER_H
#define COMPILER_H

#include <p18cxxx.h>
#define CLOCK_FREQ		(40000000)      // Hz
#define INSTR_FREQ			(CLOCK_FREQ/4)
#define CLOCK_FOSC		(40)      // MHz

#define LED0_TRIS		(TRISCbits.TRISC1)
#define LED0_IO			(PORTCbits.RC1)

#define BLINDS0_TRIS	(TRISCbits.TRISC4)
#define BLINDS0_IO		(PORTCbits.RC4)

#define BLINDS0P_TRIS	(TRISCbits.TRISC5)
#define BLINDS0P_IO		(PORTCbits.RC5)

#define TEMPERATURE_INSIDE 0x1
#define TEMPERATURE_OUTSIDE 0x2

#define APP_VERSION 0x0002

#endif //compiler.h
