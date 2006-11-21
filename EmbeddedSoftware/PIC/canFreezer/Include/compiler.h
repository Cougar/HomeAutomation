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

#define DOOR_OPEN 1
#define DOOR_CLOSED 0

#define DOOR_FREEZER_TRIS 		(TRISAbits.TRISA4)
#define DOOR_FREEZER_IO 		(PORTAbits.RA4)
#define DOOR_REFRIGERATOR_TRIS 	(TRISAbits.TRISA5)
#define DOOR_REFRIGERATOR_IO 	(PORTAbits.RA5)


#define TEMPERATURE_FREEZER 0b0000
#define TEMPERATURE_REFRIGERATOR 0b0001

#endif //compiler.h
