#ifndef CHN_CHNMASTER
#define CHN_CHNMASTER

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

void chn_ChnMaster_RegisterListener( uint16_t channel_id, void (*UpdateFunc)(uint16_t, uint16_t) /* channel, value */ );
void chn_ChnMaster_UpdateChannel( uint16_t channel_id, uint16_t value );

void chn_ChnMaster_Init(void);
void chn_ChnMaster_Process(void);
void chn_ChnMaster_HandleMessage(StdCan_Msg_t *rxMsg);
void chn_ChnMaster_List(uint8_t ModuleSequenceNumber);


#ifndef chn_ChnMaster_USEEEPROM
#define chn_ChnMaster_USEEEPROM 0
#endif

#if chn_ChnMaster_USEEEPROM==1
	struct chn_ChnMaster_Data{
		///TODO: Define EEPROM variables needed by the module
		uint8_t x;
		uint16_t y;
	};	
#endif

#endif // CHN_CHNMASTER
