
#include "act_softPWM.h"
uint16_t currentTimer = 0;
uint16_t maxTimer = 10;
uint8_t	resolution = 1;

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
#ifdef PIN_0
	uint16_t PIN_0_timer=0;
#endif
#ifdef PIN_1
	uint16_t PIN_1_timer=0;
#endif
#ifdef PIN_2
	uint16_t PIN_2_timer=0;
#endif
#ifdef PIN_3
	uint16_t PIN_3_timer=0;
#endif
#ifdef PIN_4
	uint16_t PIN_4_timer=0;
#endif
#ifdef PIN_5
	uint16_t PIN_5_timer=0;
#endif
#ifdef PIN_6
	uint16_t PIN_6_timer=0;
#endif
#ifdef PIN_7
	uint16_t PIN_7_timer=0;
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
#ifdef PIN_0
	gpio_set_out(PIN_0);
	gpio_clr_pin(PIN_0);
#endif
#ifdef PIN_1
	gpio_set_out(PIN_1);
	gpio_clr_pin(PIN_1);
#endif
#ifdef PIN_2
	gpio_set_out(PIN_2);
	gpio_clr_pin(PIN_2);
#endif
#ifdef PIN_3
	gpio_set_out(PIN_3);
	gpio_clr_pin(PIN_3);
#endif
#ifdef PIN_4
	gpio_set_out(PIN_4);
	gpio_clr_pin(PIN_4);
#endif
#ifdef PIN_5
	gpio_set_out(PIN_5);
	gpio_clr_pin(PIN_5);
#endif
#ifdef PIN_6
	gpio_set_out(PIN_6);
	gpio_clr_pin(PIN_6);
#endif
#ifdef PIN_7
	gpio_set_out(PIN_7);
	gpio_clr_pin(PIN_7);
#endif
	
	Timer_SetTimeout(act_softPWM_TIMER, resolution, TimerTypeFreeRunning, NULL);
	// to use PCINt lib, call this function: (the callback function look as a timer callback function)
	// Pcint_SetCallbackPin(act_softPWM_PCINT, EXP_C , &act_softPWM_pcint_callback);

}

void act_softPWM_Process(void)
{
	if (Timer_Expired(act_softPWM_TIMER)) {
		currentTimer++;
		if (currentTimer == 10) {
			currentTimer=0;
			#ifdef PIN_0 
				gpio_set_pin(PIN_0);
			#endif
			#ifdef PIN_1 
				gpio_set_pin(PIN_1);
			#endif
			#ifdef PIN_2 
				gpio_set_pin(PIN_2);
			#endif
			#ifdef PIN_3 
				gpio_set_pin(PIN_3);
			#endif
			#ifdef PIN_4 
				gpio_set_pin(PIN_4);
			#endif
			#ifdef PIN_5 
				gpio_set_pin(PIN_5);
			#endif
			#ifdef PIN_6 
				gpio_set_pin(PIN_6);
			#endif
			#ifdef PIN_7 
				gpio_set_pin(PIN_7);
			#endif
		}
		#ifdef PIN_0
		if (currentTimer >= PIN_0_timer) {
			gpio_clr_pin(PIN_0);
		}
		#endif
		#ifdef PIN_1
		if (currentTimer >= PIN_1_timer) {
			gpio_clr_pin(PIN_1);
		}
		#endif
		#ifdef PIN_2
		if (currentTimer >= PIN_2_timer) {
			gpio_clr_pin(PIN_2);
		}
		#endif
		#ifdef PIN_3
		if (currentTimer >= PIN_3_timer) {
			gpio_clr_pin(PIN_3);
		}
		#endif
		#ifdef PIN_4
		if (currentTimer >= PIN_4_timer) {
			gpio_clr_pin(PIN_4);
		}
		#endif
		#ifdef PIN_5
		if (currentTimer >= PIN_5_timer) {
			gpio_clr_pin(PIN_5);
		}
		#endif
		#ifdef PIN_6
		if (currentTimer >= PIN_6_timer) {
			gpio_clr_pin(PIN_6);
		}
		#endif
		#ifdef PIN_7
		if (currentTimer >= PIN_7_timer) {
			gpio_clr_pin(PIN_7);
		}
		#endif
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
		case CAN_MODULE_CMD_PHYSICAL_PWM:
			if ((((uint16_t)rxMsg->Data[1])<8) + rxMsg->Data[2] > maxTimer)
			{
				rxMsg->Data[2] = maxTimer & 0x00ff;
				rxMsg->Data[1] = (maxTimer>>8) & 0x00ff;
			}
			switch (rxMsg->Data[0])
			{
			#ifdef PIN_0
			case 0:
				PIN_0_timer = (((uint16_t)rxMsg->Data[1])<<8) + rxMsg->Data[2];
				break;
			#endif
			#ifdef PIN_1
			case 1:
				PIN_1_timer = (((uint16_t)rxMsg->Data[1])<<8) + rxMsg->Data[2];
				break;
			#endif
			#ifdef PIN_2
			case 2:
				PIN_2_timer = (((uint16_t)rxMsg->Data[1])<<8) + rxMsg->Data[2];
				break;
			#endif
			#ifdef PIN_3
			case 3:
				PIN_3_timer = (((uint16_t)rxMsg->Data[1])<<8) + rxMsg->Data[2];
				break;
			#endif
			#ifdef PIN_4
			case 4:
				PIN_4_timer = (((uint16_t)rxMsg->Data[1])<<8) + rxMsg->Data[2];
				break;
			#endif
			#ifdef PIN_5
			case 5:
				PIN_5_timer = (((uint16_t)rxMsg->Data[1])<<8) + rxMsg->Data[2];
				break;
			#endif
			#ifdef PIN_6
			case 6:
				PIN_6_timer = (((uint16_t)rxMsg->Data[1])<<8) + rxMsg->Data[2];
				break;
			#endif
			#ifdef PIN_7
			case 7:
				PIN_7_timer = (((uint16_t)rxMsg->Data[1])<<8) + rxMsg->Data[2];
				break;
			#endif
			}
			break;
		case CAN_MODULE_CMD_SOFTPWM_CONFIG:
			maxTimer = (((uint16_t)rxMsg->Data[0])<<8) + rxMsg->Data[1];
			resolution = rxMsg->Data[2];
			Timer_SetTimeout(act_softPWM_TIMER, resolution, TimerTypeFreeRunning, NULL);
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
