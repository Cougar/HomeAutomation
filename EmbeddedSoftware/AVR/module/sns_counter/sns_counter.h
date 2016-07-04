#ifndef SNS_COUNTER
#define SNS_COUNTER

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
#include <drivers/adc/adc.h>
#include <drivers/mcu/pcint.h>

void sns_counter_Init(void);
void sns_counter_Process(void);
void sns_counter_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_counter_List(uint8_t ModuleSequenceNumber);


#ifndef sns_counter_USEEEPROM
#define sns_counter_USEEEPROM 0
#endif

#if sns_counter_USEEEPROM==1
	struct sns_counter_Data{
		uint8_t reportInterval;
		uint32_t Count0;
	};
#endif

#ifndef sns_counter_CH0_pullup
#define	sns_counter_CH0_pullup 0
#endif
#ifndef sns_counter_CH1_pullup
#define	sns_counter_CH1_pullup 1
#endif
#ifndef sns_counter_CH2_pullup
#define	sns_counter_CH2_pullup 2
#endif
#ifndef sns_counter_CH3_pullup
#define	sns_counter_CH3_pullup 3
#endif
#ifndef sns_counter_CH4_pullup
#define	sns_counter_CH4_pullup 4
#endif
#ifndef sns_counter_CH5_pullup
#define	sns_counter_CH5_pullup 5
#endif
#ifndef sns_counter_CH6_pullup
#define	sns_counter_CH6_pullup 6
#endif
#ifndef sns_counter_CH7_pullup
#define	sns_counter_CH7_pullup 7
#endif

#ifndef sns_counter_CH0_DEBOUNCE_TIME_MS
#define	sns_counter_CH0_DEBOUNCE_TIME_MS 0
#endif
#ifndef sns_counter_CH1_DEBOUNCE_TIME_MS
#define	sns_counter_CH1_DEBOUNCE_TIME_MS 0
#endif
#ifndef sns_counter_CH2_DEBOUNCE_TIME_MS
#define	sns_counter_CH2_DEBOUNCE_TIME_MS 0
#endif
#ifndef sns_counter_CH3_DEBOUNCE_TIME_MS
#define	sns_counter_CH3_DEBOUNCE_TIME_MS 0
#endif
#ifndef sns_counter_CH4_DEBOUNCE_TIME_MS
#define	sns_counter_CH4_DEBOUNCE_TIME_MS 0
#endif
#ifndef sns_counter_CH5_DEBOUNCE_TIME_MS
#define	sns_counter_CH5_DEBOUNCE_TIME_MS 0
#endif
#ifndef sns_counter_CH6_DEBOUNCE_TIME_MS
#define	sns_counter_CH6_DEBOUNCE_TIME_MS 0
#endif
#ifndef sns_counter_CH7_DEBOUNCE_TIME_MS
#define	sns_counter_CH7_DEBOUNCE_TIME_MS 0
#endif

#endif // SNS_COUNTER
