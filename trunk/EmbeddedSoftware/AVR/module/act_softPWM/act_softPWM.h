#ifndef ACT_SOFTPWM
#define ACT_SOFTPWM

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

void act_softPWM_Init(void);
void act_softPWM_Process(void);
void act_softPWM_HandleMessage(StdCan_Msg_t *rxMsg);
void act_softPWM_List(uint8_t ModuleSequenceNumber);

#define NUMBEROFCHANNELS 0
#ifdef PIN_0
	#define PIN_0_ID NUMBEROFCHANNELS
	#undef NUMBEROFCHANNELS
	#define NUMBEROFCHANNELS PIN_0_ID + 1
#warning pinne0
#endif
#ifdef PIN_1
	#define PIN_1_ID NUMBEROFCHANNELS
	#undef NUMBEROFCHANNELS
	#define NUMBEROFCHANNELS PIN_1_ID + 1
	#warning pinne1
#endif
#ifdef PIN_2
	#define PIN_2_ID NUMBEROFCHANNELS
	#undef NUMBEROFCHANNELS
	#define NUMBEROFCHANNELS PIN_2_ID + 1
#warning pinne2 NUMBEROFCHANNELS
#ifdef PIN_2_ID
#warning Hej
#endif
#endif
#ifdef PIN_3
	#define PIN_3_ID NUMBEROFCHANNELS
	#undef NUMBEROFCHANNELS
	#define NUMBEROFCHANNELS PIN_3_ID + 1
#endif
#ifdef PIN_4
	#define PIN_4_ID NUMBEROFCHANNELS
	#undef NUMBEROFCHANNELS
	#define NUMBEROFCHANNELS PIN_4_ID + 1
#endif
#ifdef PIN_5
	#define PIN_5_ID NUMBEROFCHANNELS
	#undef NUMBEROFCHANNELS
	#define NUMBEROFCHANNELS PIN_5_ID + 1
#endif
#ifdef PIN_6
	#define PIN_6_ID NUMBEROFCHANNELS
	#undef NUMBEROFCHANNELS
	#define NUMBEROFCHANNELS PIN_6_ID + 1
#endif
#ifdef PIN_7
	#define PIN_7_ID NUMBEROFCHANNELS
	#undef NUMBEROFCHANNELS
	#define NUMBEROFCHANNELS PIN_7_ID + 1
#endif


#ifdef act_softPWM_USEEEPROM
	struct act_softPWM_Data{
		///TODO: Define EEPROM variables needed by the module
		uint16_t PwmPeriod;
		uint16_t defaultPwmValue;
		uint8_t defaultStates;
		uint8_t ReportInterval;
	};	
#endif

#endif // ACT_SOFTPWM
