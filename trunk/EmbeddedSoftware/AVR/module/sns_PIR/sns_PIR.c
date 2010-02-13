
#include "sns_PIR.h"

#ifdef sns_PIR_USEEEPROM
#include "sns_PIR_eeprom.h"
struct eeprom_sns_PIR EEMEM eeprom_sns_PIR = 
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file. 
		0xAB,	// x
		0x1234	// y
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 
#endif

void sns_PIR_Init(void)
{
#ifdef sns_PIR_USEEEPROM
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

	// to use PCINt lib, call this function: (the callback function look as a timer callback function)
	// Pcint_SetCallbackPin(sns_PIR_PCINT, EXP_C , &sns_PIR_pcint_callback);
	
	ADC_Init();
	Timer_SetTimeout(sns_PIR_BRIGHT_TIMER, sns_PIR_SEND_PERIOD*1000, TimerTypeFreeRunning, 0);
}

void sns_PIR_Process(void)
{
	if (Timer_Expired(sns_PIR_BRIGHT_TIMER)) {
		uint16_t brightness = ADC_Get(sns_PIR_BRIGHTNESS_AD);
		//busVoltage = (busVoltage & 0x03ff) * ADC_FACTOR;
		
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_PIR;
		txMsg.Header.ModuleId = sns_PIR_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PIR_MOTION;
		txMsg.Length = 3;
		txMsg.Data[0] = 0;
		txMsg.Data[1] = (brightness>>2)&0xff;
		txMsg.Data[2] = 0;
		//txMsg.Data[1] = (busVoltage>>(ADC_SCALE-6+8))&0xff;
		//txMsg.Data[2] = (busVoltage>>(ADC_SCALE-6))&0xff;

		StdCan_Put(&txMsg);
	}
}

void sns_PIR_HandleMessage(StdCan_Msg_t *rxMsg)
{
	/*if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_xyz && ///TODO: Change this to the actual class type
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_xyz && ///TODO: Change this to the actual module type
		rxMsg->Header.ModuleId == sns_PIR_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}*/
}

void sns_PIR_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_PIR;
	txMsg.Header.ModuleId = sns_PIR_ID;
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
