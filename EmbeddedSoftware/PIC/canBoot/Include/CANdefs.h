/*********************************************************************
 *
 *                  CAN specification
 *
 *********************************************************************
 * FileName:        CANdefs.h
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Johan Böhlin     21-10-06 	Original        (Rev 1.0)
 ********************************************************************/
// $Id$

#ifndef CANDEFS_H
#define CANDEFS_H

#include <compiler.h>

#define CAN_BAUD 0.500000 //in Mbits/s
//#define CAN_LOOPBACK_MODE


// TQ(us) = (2 * (BRP + 1))/FOSC(MHz)
// Nominal Bit Rate(kbit/s) = 1 / TBIT = 1 / (TQ(us) * 8)
// Where 8 is nominal bit time.
// From this, BRP can be calculated.
//
// Nominal Bit Rate(kbit/s) = 1 / TBIT = 1 / ((2 * (BRP + 1))/FOSC(MHz) * 8)
                    
#define CAN_BRP (int)(CLOCK_FOSC/(CAN_BAUD * 16) - 1)


#endif //CANdefs.h
