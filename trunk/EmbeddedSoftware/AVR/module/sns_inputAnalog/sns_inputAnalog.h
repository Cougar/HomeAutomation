#ifndef SNS_INPUTANALOG
#define SNS_INPUTANALOG

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
#include <drivers/io/PCA95xx.h>

//to use PCINT lib. uncomment the line below
//#include <drivers/mcu/pcint.h>

void sns_inputAnalog_Init(void);
void sns_inputAnalog_Process(void);
void sns_inputAnalog_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_inputAnalog_List(uint8_t ModuleSequenceNumber);

struct sns_inputAnalog_Config{
	uint16_t			LowTh;			//Config, low level threshold voltage
	uint16_t			HighTh;			//Config, high level threshold voltage
	uint16_t			Periodicity;	//Config, periodicity
	uint8_t				Type;			//Config, if sensor is of type periodic or digital input
	uint8_t				PullupEnable;	//Config, if the pullup should be enabled
	uint8_t				RefEnable;		//Config, if the reference to GND should be enabled
} sns_inputAnalog_Config[sns_inputAnalog_NUM_SUPPORTED];

#ifndef sns_inputAnalog_USEEEPROM
#define sns_inputAnalog_USEEEPROM 0
#endif

#if sns_inputAnalog_USEEEPROM==1
	struct sns_inputAnalog_Data{
		struct sns_inputAnalog_Config Config0;
#if sns_inputAnalog_NUM_SUPPORTED>=2
		struct sns_inputAnalog_Config Config1;
#endif
#if sns_inputAnalog_NUM_SUPPORTED>=3
		struct sns_inputAnalog_Config Config2;
#endif
#if sns_inputAnalog_NUM_SUPPORTED>=4
		struct sns_inputAnalog_Config Config3;
#endif
	};	
#endif

#define TRUE 1
#define FALSE 0

#define MAX(a,b) ((a)>(b) ? (a) : (b))
#define MIN(a,b) ((a)<(b) ? (a) : (b))

#if sns_inputAnalog0PullupPCA95xx==1 | sns_inputAnalog1PullupPCA95xx==1 | sns_inputAnalog2PullupPCA95xx==1 | sns_inputAnalog3PullupPCA95xx==1 | sns_inputAnalog0RefPCA95xx==1 | sns_inputAnalog1RefPCA95xx==1 | sns_inputAnalog2RefPCA95xx==1 | sns_inputAnalog3RefPCA95xx==1
#define sns_inputAnalog_ENABLE_PCA95xx 1
#else
#define sns_inputAnalog_ENABLE_PCA95xx 0
#endif


#endif // SNS_INPUTANALOG
