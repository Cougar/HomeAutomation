/* vim: set sw=8 ts=8 si et: */
/*********************************************
* Author: Guido Socher
* Copyright: GPL V2
**********************************************/

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include "avr_compat.h"

#ifndef ALIBC_OLD
#include <util/delay.h>
#else
#include <avr/delay.h>
#endif

void delay_ms(unsigned int ms)
/* delay for a minimum of <ms> */
{
        // we use a calibrated macro. This is more
        // accurate and not so much compiler dependent
        // as self made code.
        while(ms){
                _delay_ms(0.96);
                ms--;
        }
}


