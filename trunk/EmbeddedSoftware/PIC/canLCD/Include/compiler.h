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
#define NUM_CYK_1us		10
#define NUM_CYK_1ms		10000

#define LED0_TRIS		(TRISCbits.TRISC1)
#define LED0_IO			(PORTCbits.RC1)

// LCD
#define LCD_DATA_4_TRIS (PORTCbits.RC0)
#define LCD_DATA_4_IO 	(TRISCbits.TRISC0)
#define LCD_DATA_5_TRIS (PORTCbits.RC1)
#define LCD_DATA_5_IO 	(TRISCbits.TRISC1)
#define LCD_DATA_6_TRIS (PORTCbits.RC2)
#define LCD_DATA_6_IO 	(TRISCbits.TRISC2)
#define LCD_DATA_7_TRIS (PORTCbits.RC3)
#define LCD_DATA_7_IO 	(TRISCbits.TRISC3)

#define LCD_RS_TRIS (PORTAbits.RA4)
#define LCD_RS_IO 	(TRISAbits.TRISA4)
#define LCD_EN_TRIS (PORTAbits.RA5)
#define LCD_EN_IO 	(TRISAbits.TRISA5)

#define LCD_LINE1 0
#define LCD_LINE2 40
#define LCD_LINE_LENGTH 24

#endif //compiler.h
