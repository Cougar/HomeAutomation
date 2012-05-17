#ifndef SNS_TCN75A
#define SNS_TCN75A

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
#include <drivers/mcu/TWI_Master.h>

//to use PCINT lib. uncomment the line below
//#include <drivers/mcu/pcint.h>

void sns_TCN75A_Init(void);
void sns_TCN75A_Process(void);
void sns_TCN75A_HandleMessage(StdCan_Msg_t *rxMsg);
void sns_TCN75A_List(uint8_t ModuleSequenceNumber);


#define TCN75A_REG_TEMPERATURE	0
#define TCN75A_REG_CONFIG		1
#define TCN75A_REG_HYSTERESIS	2
#define TCN75A_REG_LIMITSET		3

#define TCN75A_CNF_RESOLUTION_BIT	5
#define TCN75A_CNF_RESOLUTION_9		0 /* gives 9bit resolution, 0.5deg C*/
#define TCN75A_CNF_RESOLUTION_10	1 /* gives 10bit resolution, 0.25deg C*/
#define TCN75A_CNF_RESOLUTION_11	2 /* gives 11bit resolution, 0.125deg C*/
#define TCN75A_CNF_RESOLUTION_12	3 /* gives 12bit resolution, 0.0625deg C*/


#ifndef sns_TCN75A_USEEEPROM
#define sns_TCN75A_USEEEPROM 0
#endif

#if sns_TCN75A_USEEEPROM==1
	struct sns_TCN75A_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#endif // SNS_TCN75A
