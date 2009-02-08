#ifndef ACT_DIMMER230
#define ACT_DIMMER230

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
/* system files */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
/* lib files */
#include <config.h>
#include <bios.h>
#include <drivers/can/stdcan.h>
#include <drivers/timer/timer.h>
#include <drivers/mcu/gpio.h>

#define ACT_DIMMMER230_STATE_IDLE			0
#define ACT_DIMMMER230_STATE_TIMER_ON		1
#define ACT_DIMMMER230_STATE_xyz			2

#define ACT_DIMMMER230_DEMO_STATE_NOT_RUNNING	0
#define ACT_DIMMMER230_DEMO_STATE_DECREASE		1
#define ACT_DIMMMER230_DEMO_STATE_INCREASE		2
#define ACT_DIMMMER230_DEMO_STATE_GOBACK		3

#define ACT_DIMMMER230_PERIOD_TIME			10000

#define ACT_DIMMMER230_MAX_DIM				255
#define ACT_DIMMMER230_MIN_DIM				0

#define ACT_DIMMMER230_50HZ_PERIOD_TIME		10000
#define ACT_DIMMMER230_60HZ_PERIOD_TIME		8000

#if act_dimmer230_ZC_PCINT<8
#define act_dimmer230_ZC_PCINT_vect	PCINT0_vect		//interrupt vector
#define act_dimmer230_ZC_PCIE		PCIE0			//interrupt enable
#define act_dimmer230_ZC_PCIF		PCIF0			//interrupt flag
#define act_dimmer230_ZC_PCMSK		PCMSK0			//mask register
#define act_dimmer230_ZC_PCINT_BIT	act_dimmer230_ZC_PCINT		//interrupt bit
#endif

#if act_dimmer230_ZC_PCINT>=8 && act_dimmer230_ZC_PCINT<16
#define act_dimmer230_ZC_PCINT_vect	PCINT1_vect		//interrupt vector
#define act_dimmer230_ZC_PCIE		PCIE1			//interrupt enable
#define act_dimmer230_ZC_PCIF		PCIF1			//interrupt flag
#define act_dimmer230_ZC_PCMSK		PCMSK1			//mask register
#define act_dimmer230_ZC_PCINT_BIT	act_dimmer230_ZC_PCINT-8	//interrupt bit
#endif

#if act_dimmer230_ZC_PCINT>=16 && act_dimmer230_ZC_PCINT<24
#define act_dimmer230_ZC_PCINT_vect	PCINT2_vect		//interrupt vector
#define act_dimmer230_ZC_PCIE		PCIE2			//interrupt enable
#define act_dimmer230_ZC_PCIF		PCIF2			//interrupt flag
#define act_dimmer230_ZC_PCMSK		PCMSK2			//mask register
#define act_dimmer230_ZC_PCINT_BIT	act_dimmer230_ZC_PCINT-16	//interrupt bit
#endif

void act_dimmer230_Init(void);
void act_dimmer230_Process(void);
void act_dimmer230_HandleMessage(StdCan_Msg_t *rxMsg);
void act_dimmer230_List(uint8_t ModuleSequenceNumber);

#ifdef act_dimmer230_USEEEPROM
	struct act_dimmer230_Data{
		// the dimmer will store its current value in eeprom, to use when starting
		uint8_t eeDimmerValue;
	};	

// Do not change anything below this line.
// ----------------------------------------------------------------

	#include <drivers/misc/eeprom_crc8.h>
	#include <avr/eeprom.h>
	#define EEDATA 			(uint8_t*)&eeprom_act_dimmer230.Data
	#define EEDATA16		(uint16_t*)&eeprom_act_dimmer230.Data
	#define EEDATA_CALC_CRC		eeprom_crc8((uint8_t*)&eeprom_act_dimmer230.Data,sizeof(struct act_dimmer230_Data))
	#define EEDATA_UPDATE_CRC	eeprom_write_byte((uint8_t*)&eeprom_act_dimmer230.crc,eeprom_crc8((uint8_t*)&eeprom_act_dimmer230.Data,sizeof(struct act_dimmer230_Data)))
	#define EEDATA_OK		EEDATA_CALC_CRC == EEDATA_STORED_CRC
	#define EEDATA_STORED_CRC 	eeprom_read_byte((uint8_t*)&eeprom_act_dimmer230.crc)
	
	struct eeprom_act_dimmer230{
		struct act_dimmer230_Data Data;
		uint8_t crc;
	};
	extern struct eeprom_act_dimmer230 EEMEM eeprom_act_dimmer230;
	
	#define WITH_CRC	1
	#define WITHOUT_CRC	0
	static __inline__ void eeprom_write_byte_crc(uint8_t *__p, uint8_t __value, uint8_t crc) {
	  if (__value != eeprom_read_byte(__p))
	  {
	    eeprom_write_byte(__p, __value);
	    if (crc == WITH_CRC)
	    {
	      EEDATA_UPDATE_CRC;
	    }
	  }
	}
	static __inline__ void eeprom_write_word_crc(uint16_t *__p, uint16_t __value, uint8_t crc) {
	  if (__value != eeprom_read_word(__p))
	  {
	    eeprom_write_word(__p, __value);
	    if (crc == WITH_CRC)
	    {
	      EEDATA_UPDATE_CRC;
	    }
	  }
	}
#endif

#endif // ACT_DIMMER230
