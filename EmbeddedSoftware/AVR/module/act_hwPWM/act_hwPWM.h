#ifndef ACT_HWPWM
#define ACT_HWPWM

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
//#include <drivers/mcu/pcint.h>

void act_hwPWM_Init(void);
void act_hwPWM_Process(void);
void act_hwPWM_HandleMessage(StdCan_Msg_t *rxMsg);
void act_hwPWM_List(uint8_t ModuleSequenceNumber);

#if act_hwPWM_CH1_COM>0 || act_hwPWM_CH2_COM>0
#if act_hwPWM_CH1_CS != act_hwPWM_CH2_CS
#error Cannot have different Clock Prescaler Select values, act_hwPWM_CH1_CS != act_hwPWM_CH2_CS
#endif
#if act_hwPWM_CH1_WGM != act_hwPWM_CH2_WGM
#error Cannot have different Waveform Generation Mode values, act_hwPWM_CH1_WGM != act_hwPWM_CH2_WGM
#endif
#endif

#if act_hwPWM_CH3_COM>0 || act_hwPWM_CH4_COM>0
#if act_hwPWM_CH3_CS != act_hwPWM_CH4_CS
#error Cannot have different Clock Prescaler Select values, act_hwPWM_CH3_CS != act_hwPWM_CH4_CS
#endif
#if act_hwPWM_CH3_WGM != act_hwPWM_CH4_WGM
#error Cannot have different Waveform Generation Mode values, act_hwPWM_CH3_WGM != act_hwPWM_CH4_WGM
#endif
#endif

#if act_hwPWM_CH1_WGM==0
#define act_hwPWM_CH1_TOP 0xffffUL
#define act_hwPWM_CH2_TOP 0xffffUL
#endif
#if act_hwPWM_CH1_WGM==1 || act_hwPWM_CH1_WGM==5
#define act_hwPWM_CH1_TOP 0x00ffUL
#define act_hwPWM_CH2_TOP 0x00ffUL
#endif
#if act_hwPWM_CH1_WGM==2 || act_hwPWM_CH1_WGM==6
#define act_hwPWM_CH1_TOP 0x01ffUL
#define act_hwPWM_CH2_TOP 0x01ffUL
#endif
#if act_hwPWM_CH1_WGM==3 || act_hwPWM_CH1_WGM==7
#define act_hwPWM_CH1_TOP 0x03ffUL
#define act_hwPWM_CH2_TOP 0x03ffUL
#endif
#define act_hwPWM_CH1_FACT (256UL*act_hwPWM_CH1_TOP)/10000UL
#define act_hwPWM_CH2_FACT (256UL*act_hwPWM_CH2_TOP)/10000UL

#if act_hwPWM_CH1_WGM==4 || act_hwPWM_CH1_WGM>7
#error Not supported Waveform Generation Mode on CH1 and CH2
#endif


#if act_hwPWM_CH3_WGM==0 || act_hwPWM_CH3_WGM==1 || act_hwPWM_CH3_WGM==3
#define act_hwPWM_CH3_TOP 0xff
#define act_hwPWM_CH4_TOP 0xff
#endif
#define act_hwPWM_CH3_FACT (256UL*act_hwPWM_CH3_TOP)/10000UL
#define act_hwPWM_CH4_FACT (256UL*act_hwPWM_CH4_TOP)/10000UL

#if act_hwPWM_CH3_WGM==2 || act_hwPWM_CH3_WGM>3
#error Not supported Waveform Generation Mode on CH3 and CH4
#endif

#define OCR_1	OCR1B
#define OCR_2	OCR1A
#define OCR_3	OCR0A
#define OCR_4	OCR0B

#define ACT_HWPWM_MAX_DIM				10000UL
#define ACT_HWPWM_MIN_DIM				0

#define ACT_HWPWM_DEMO_STATE_NOT_RUNNING	0
#define ACT_HWPWM_DEMO_STATE_DECREASE		1
#define ACT_HWPWM_DEMO_STATE_INCREASE		2
#define ACT_HWPWM_DEMO_STATE_GOBACK		3

#ifdef act_hwPWM_USEEEPROM
	struct act_hwPWM_Data{
		///TODO: Define EEPROM variables needed by the module
		uint16_t ch1;
		uint16_t ch2;
		uint16_t ch3;
		uint16_t ch4;
	};	
#endif

#endif // ACT_HWPWM
