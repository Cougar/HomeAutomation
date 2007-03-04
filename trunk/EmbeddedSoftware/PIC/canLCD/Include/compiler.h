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

#define APP_VERSION 0x0001


#include <p18cxxx.h>
#define CLOCK_FREQ		(40000000)      // Hz
#define INSTR_FREQ			(CLOCK_FREQ/4)
#define CLOCK_FOSC		(40)      // MHz
#define NUM_CYK_1us		10
#define NUM_CYK_1ms		10000

#define LED0_IO			(PORTCbits.RC1)
#define LED0_TRIS		(TRISCbits.TRISC1)

// GLCD
#define GLCD_CS_IO		(PORTCbits.RC0)
#define GLCD_CS_TRIS	(TRISCbits.TRISC0)
#define GLCD_RES_IO		(PORTCbits.RC7)
#define GLCD_RES_TRIS 	(TRISCbits.TRISC7)
#define GLCD_A0_IO		(PORTCbits.RC1)
#define GLCD_A0_TRIS	(TRISCbits.TRISC1)
#define GLCD_SCL_IO		(PORTCbits.RC3)
#define GLCD_SCL_TRIS 	(TRISCbits.TRISC3)
#define GLCD_SI_IO		(PORTCbits.RC5)
#define GLCD_SI_TRIS	(TRISCbits.TRISC5)

#define GLCD_BAL_IO		(PORTCbits.RC2)
#define GLCD_BAL_TRIS	(TRISCbits.TRISC2)

// Rotary, Rotary switch, switch
#define ROTARY_A_IO		(PORTBbits.RB4)
#define ROTARY_A_TRIS 	(TRISBbits.TRISB4)   // RB0
#define ROTARY_B_IO		(PORTBbits.RB5)
#define ROTARY_B_TRIS 	(TRISBbits.TRISB5)

#define ROTARY_SW_IO	(PORTAbits.RA4)
#define ROTARY_SW_TRIS	(TRISAbits.TRISA4)   // RB1
#define SW0_IO			(PORTAbits.RA5)
#define SW0_TRIS		(TRISAbits.TRISA5)

// door
#define SW1_IO			(PORTAbits.RA2)		// Poll
#define SW1_TRIS		(TRISAbits.TRISA2)

// VREF, temp
#define VREF_IO			(PORTAbits.RA3)
#define VREF_TRIS		(TRISAbits.TRISA3)
#define TEMP_IO			(PORTAbits.RA0)
#define TEMP_TRIS		(TRISAbits.TRISA0)

// EE
#define EE_CS_IO		(PORTCbits.RC6)
#define EE_CS_TRIS		(TRISCbits.TRISC6)
#define EE_SI_IO		(PORTCbits.RC4)
#define EE_SI_TRIS		(TRISCbits.TRISC4)


// LCD
/*
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
*/

#define TEMPERATURE_OUTSIDE 0x0




#endif //compiler.h
