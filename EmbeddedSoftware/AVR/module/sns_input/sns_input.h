#ifndef SNS_INPUT
#define SNS_INPUT

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

void sns_input_Init(void);
void sns_input_Process(void);
void sns_input_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_input_List(uint8_t ModuleSequenceNumber);

#ifndef	sns_input_CH0PCA95xxIO
#define sns_input_CH0PCA95xxIO 0
#endif
#ifndef	sns_input_CH1PCA95xxIO
#define sns_input_CH1PCA95xxIO 0
#endif
#ifndef	sns_input_CH2PCA95xxIO
#define sns_input_CH2PCA95xxIO 0
#endif
#ifndef	sns_input_CH3PCA95xxIO
#define sns_input_CH3PCA95xxIO 0
#endif
#ifndef	sns_input_CH4PCA95xxIO
#define sns_input_CH4PCA95xxIO 0
#endif
#ifndef	sns_input_CH5PCA95xxIO
#define sns_input_CH5PCA95xxIO 0
#endif
#ifndef	sns_input_CH6PCA95xxIO
#define sns_input_CH6PCA95xxIO 0
#endif
#ifndef	sns_input_CH7PCA95xxIO
#define sns_input_CH7PCA95xxIO 0
#endif

#if sns_input_CH0PCA95xxIO==1 |sns_input_CH1PCA95xxIO==1 | sns_input_CH2PCA95xxIO==1 | sns_input_CH3PCA95xxIO==1 | sns_input_CH4PCA95xxIO==1 | sns_input_CH5PCA95xxIO==1 | sns_input_CH6PCA95xxIO==1 | sns_input_CH7PCA95xxIO==1
#define sns_input_ENABLE_PCA95xx 1
#else
#define sns_input_ENABLE_PCA95xx 0
#endif

#endif // SNS_INPUT
