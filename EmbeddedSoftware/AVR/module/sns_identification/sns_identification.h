#ifndef SNS_IDENTIFICATION
#define SNS_IDENTIFICATION

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
/* lib files */
#include <config.h>
#include <bios.h>
#include <drivers/can/stdcan.h>
#include <drivers/timer/timer.h>
#include <drivers/mcu/gpio.h>

//to use PCINT lib. uncomment the line below
#include <drivers/mcu/pcint.h>
#include <drivers/io/PCA95xx.h>

void sns_identification_Init(void);
void sns_identification_Process(void);
void sns_identification_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_identification_List(uint8_t ModuleSequenceNumber);

#ifndef	sns_identification_PCA95xxIO
#define sns_identification_PCA95xxIO 0
#endif


#endif // SNS_IDENTIFICATION
