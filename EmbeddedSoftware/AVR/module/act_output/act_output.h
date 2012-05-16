#ifndef ACT_OUTPUT
#define ACT_OUTPUT

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
#include <drivers/io/PCA95xx.h>

//to use PCINT lib. uncomment the line below
//#include <drivers/mcu/pcint.h>

void act_output_Init(void);
void act_output_Process(void);
void act_output_HandleMessage(StdCan_Msg_t *rxMsg);
void act_output_List(uint8_t ModuleSequenceNumber);


#ifndef act_output_USEEEPROM
#define act_output_USEEEPROM 0
#endif

#if act_output_USEEEPROM==1
	struct act_output_Data{
		///TODO: Define EEPROM variables needed by the module

#ifdef	act_output_CH0
		uint8_t ch0;
#endif
#ifdef	act_output_CH1
		uint8_t ch1;
#endif
#ifdef	act_output_CH2
		uint8_t ch2;
#endif
#ifdef	act_output_CH3
		uint8_t ch3;
#endif
#ifdef	act_output_CH4
		uint8_t ch4;
#endif
#ifdef	act_output_CH5
		uint8_t ch5;
#endif
#ifdef	act_output_CH6
		uint8_t ch6;
#endif
#ifdef	act_output_CH7
		uint8_t ch7;
#endif
#ifdef	act_output_CH8
		uint8_t ch8;
#endif
#ifdef	act_output_CH9
		uint8_t ch9;
#endif
	};	
#endif

#ifndef	act_output_NUM_SUPPORTED
#define act_output_NUM_SUPPORTED 10
#endif

#ifndef	act_output_CH0PCA95xxIO
#define act_output_CH0PCA95xxIO 0
#endif
#ifndef	act_output_CH1PCA95xxIO
#define act_output_CH1PCA95xxIO 0
#endif
#ifndef	act_output_CH2PCA95xxIO
#define act_output_CH2PCA95xxIO 0
#endif
#ifndef	act_output_CH3PCA95xxIO
#define act_output_CH3PCA95xxIO 0
#endif
#ifndef	act_output_CH4PCA95xxIO
#define act_output_CH4PCA95xxIO 0
#endif
#ifndef	act_output_CH5PCA95xxIO
#define act_output_CH5PCA95xxIO 0
#endif
#ifndef	act_output_CH6PCA95xxIO
#define act_output_CH6PCA95xxIO 0
#endif
#ifndef	act_output_CH7PCA95xxIO
#define act_output_CH7PCA95xxIO 0
#endif
#ifndef	act_output_CH8PCA95xxIO
#define act_output_CH8PCA95xxIO 0
#endif
#ifndef	act_output_CH9PCA95xxIO
#define act_output_CH9PCA95xxIO 0
#endif


#endif // ACT_OUTPUT
