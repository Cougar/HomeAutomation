
#include "sns_qtouch.h"

#include "src/QTouch/touch_api.h"
#include "touch.h"
#define GET_SENSOR_STATE(SENSOR_NUMBER) qt_measure_data.qt_touch_status.sensor_states[(SENSOR_NUMBER/8)] & (1 << (SENSOR_NUMBER % 8))
#define GET_ROTOR_SLIDER_POSITION(ROTOR_SLIDER_NUMBER) qt_measure_data.qt_touch_status.rotor_slider_values[ROTOR_SLIDER_NUMBER]

extern void touch_measure(void);
extern void touch_init( void );

/* Timer period in msec. */
uint16_t time_ms_inc=0;

/* flag set by timer ISR when it's time to measure touch */
volatile uint8_t time_to_measure_touch = 0u;

/* current time, set by timer ISR */
volatile uint16_t current_time_ms_touch = 0u;

#ifdef sns_qtouch_USEEEPROM
#include "sns_qtouch_eeprom.h"

struct eeprom_sns_qtouch EEMEM eeprom_sns_qtouch =
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file.
		0xAB,		// x
		0x1234		// y
		0x12345678	// z
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
};
#endif
  
void sns_qtouch_timer_callback(uint8_t timer) 
  {
	time_to_measure_touch = 1u;  
  }

void sns_qtouch_Init(void)
{
#ifdef sns_qtouch_USEEEPROM
	if (EEDATA_OK)
	{
	  ///TODO: Use stored data to set initial values for the module
	  blablaX = eeprom_read_byte(EEDATA.x);
	  blablaY = eeprom_read_word(EEDATA16.y);
	  blablaZ = eeprom_read_dword(EEDATA32.y);
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_byte_crc(EEDATA.x, 0xAB, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.y, 0x1234, WITHOUT_CRC);
	  eeprom_write_dword_crc(EEDATA32.y, 0x12345678, WITHOUT_CRC);
	  EEDATA_UPDATE_CRC;
	}
#endif
	///TODO: Initialize hardware etc here

	// to use PCINt lib, call this function: (the callback function look as a timer callback function)
	// Pcint_SetCallbackPin(sns_qtouch_PCINT, EXP_C , &sns_qtouch_pcint_callback);



    /* configure timer to fire regularly */
    Timer_SetTimeout(sns_qtouch_TIMER, QT_MEASUREMENT_PERIOD_MS , TimerTypeFreeRunning, &sns_qtouch_timer_callback);


	/* Initialize Touch sensors */
	touch_init();



}

void sns_qtouch_Process(void)
{
	/*  update the current time */
	current_time_ms_touch = (uint16_t)(Timer_GetTicks() & 0xFFFFu);
       	touch_measure();

}

void sns_qtouch_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS && 
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_QTOUCH &&
		rxMsg->Header.ModuleId == sns_qtouch_ID)
	{
/*
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_...:
			///TODO: Do something dummy
		break;
		}
*/
	}
}

void sns_qtouch_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_QTOUCH; 
	txMsg.Header.ModuleId = sns_qtouch_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_LIST;
	txMsg.Length = 6;

	txMsg.Data[0] = NODE_HW_ID_BYTE0;
	txMsg.Data[1] = NODE_HW_ID_BYTE1;
	txMsg.Data[2] = NODE_HW_ID_BYTE2;
	txMsg.Data[3] = NODE_HW_ID_BYTE3;

	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;

	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}
