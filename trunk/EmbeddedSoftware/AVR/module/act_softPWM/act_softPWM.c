
#include "act_softPWM.h"

#ifdef act_softPWM_USEEEPROM
#include "act_softPWM_eeprom.h"
struct eeprom_act_softPWM EEMEM eeprom_act_softPWM = 
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file. 
		0xAB,	// x
		0x1234	// y
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 
#endif

void act_softPWM_Init(void)
{
#ifdef act_softPWM_USEEEPROM
	if (EEDATA_OK)
	{
	  ///TODO: Use stored data to set initial values for the module
	  blablaX = eeprom_read_byte(EEDATA.x);
	  blablaY = eeprom_read_word(EEDATA.y);
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_byte_crc(EEDATA.x, 0xAB, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA.y, 0x1234, WITHOUT_CRC);
	  EEDATA_UPDATE_CRC;
	}
#endif  
	///TODO: Initialize hardware etc here
	gpio_set_out(PIN_RED);
	gpio_set_out(PIN_GREEN);
	gpio_set_out(PIN_BLUE);
	gpio_clr_pin(PIN_RED);
	gpio_clr_pin(PIN_GREEN);
	gpio_clr_pin(PIN_BLUE);
	
	Timer_SetTimeout(act_softPWM_TIMER, 1, TimerTypeFreeRunning, NULL);
	// to use PCINt lib, call this function: (the callback function look as a timer callback function)
	// Pcint_SetCallbackPin(act_softPWM_PCINT, EXP_C , &act_softPWM_pcint_callback);

}
uint8_t redTimer = 0;
uint8_t greenTimer = 0;
uint8_t blueTimer = 0;
uint8_t currentTimer = 0;

void act_softPWM_Process(void)
{
	if (Timer_Expired(act_softPWM_TIMER)) {
		currentTimer++;
		if (currentTimer == 10) {
			currentTimer=0;
			gpio_set_pin(PIN_RED);
			gpio_set_pin(PIN_GREEN);
			gpio_set_pin(PIN_BLUE);
		}
		
		if (currentTimer == redTimer) {
			gpio_clr_pin(PIN_RED);
		}
		if (currentTimer == greenTimer) {
			gpio_clr_pin(PIN_GREEN);
		}
		if (currentTimer == blueTimer) {
			gpio_clr_pin(PIN_BLUE);
		}
	}
}

void act_softPWM_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT && 
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_SOFTPWM && 
		rxMsg->Header.ModuleId == act_softPWM_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_SOFTPWM_RGB_VALUE:
			redTimer = rxMsg->Data[0];
			greenTimer = rxMsg->Data[1];
			blueTimer = rxMsg->Data[2];
		break;
		}
	}
}

void act_softPWM_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_SOFTPWM; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = act_softPWM_ID;
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
