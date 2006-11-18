/*********************************************************************
 *
 *                  IR Reciver header file
 *
 *********************************************************************
 * FileName:        $HeadURL$
 * Last changed:	$LastChangedDate$
 * By:				$LastChangedBy$
 * Revision:		$Revision$
 ********************************************************************/

#ifndef IREC_H
#define IREC_H

#include <p18cxxx.h>
#include <stackTasks.h>
#include <compiler.h>
#include <typedefs.h>

typedef enum {IR_NONE=0,IR_RC5=1,IR_RC5X=2,IR_RC6=3} IR_TYPE;

#define t1  534 //   444uS at prescale 1:8 at 10MHz
#define t2 1069 //   889uS at prescale 1:8 at 10MHz
#define t3 1638 //  1333uS at prescale 1:8 at 10MHz
#define t4 2185 //  1778uS at prescale 1:8 at 10MHz
#define t6 3204 //  2664uS at prescale 1:8 at 10MHz

// T3 overflow = 65536 * 1/10Mhz*8 = 0,0524288 = 52ms

void irecInit(void);
void irecISR(void);
void irecParse(IR_TYPE type, BYTE toggle, BYTE addr, BYTE data);



#endif //IREC_H

