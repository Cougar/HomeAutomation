/*********************************************************************
 *
 *                  Compiler specifics like clockspeed and portIOs.
 *
 *********************************************************************
 * FileName:        $HeadURL: http://svn.arune.se/svn/HomeAutomation/trunk/EmbeddedSoftware/PIC/canBase/Include/compiler.h $
 * Last changed:	$LastChangedDate: 2006-11-14 12:49:56 +0100 (ti, 14 nov 2006) $
 * By:				$LastChangedBy: johboh $
 * Revision:		$Revision: 90 $
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
