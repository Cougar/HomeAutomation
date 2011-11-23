
#include "sns_VoltageCurrent.h"

#ifdef sns_VoltageCurrent_USEEEPROM
#include "sns_VoltageCurrent_eeprom.h"

struct eeprom_sns_VoltageCurrent EEMEM eeprom_sns_VoltageCurrent = 
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file. 
		0xAB,	// x
		0x1234	// y
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 
#endif

void sns_VoltageCurrent_Init(void)
{
#ifdef sns_VoltageCurrent_USEEEPROM
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

	// to use PCINt lib, call this function: (the callback function look as a timer callback function)
	// Pcint_SetCallbackPin(sns_VoltageCurrent_PCINT, EXP_C , &sns_VoltageCurrent_pcint_callback);

	ADC_Init();
#ifdef sns_VoltageCurrent_SEND_PERIOD
	Timer_SetTimeout(sns_VoltageCurrent_TIMER, sns_VoltageCurrent_SEND_PERIOD*1000 , TimerTypeFreeRunning, 0);
#else
#ifdef sns_VoltageCurrent_SEND_PERIOD_MS
	Timer_SetTimeout(sns_VoltageCurrent_TIMER, sns_VoltageCurrent_SEND_PERIOD_MS , TimerTypeFreeRunning, 0);
#endif
#endif
	

}

uint8_t VoltageCurrentChannelToSend = 0;
void sns_VoltageCurrent_Process(void)
{
	if (Timer_Expired(sns_VoltageCurrent_TIMER)) {
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_VOLTAGECURRENT;
		txMsg.Header.ModuleId = sns_VoltageCurrent_ID;
		txMsg.Length = 3;
		uint16_t ADvalue;
		
#ifdef sns_VoltageCurrent0AD
		if (VoltageCurrentChannelToSend==0)
		{
			ADvalue = ADC_Get(sns_VoltageCurrent0AD);
			ADvalue = ADvalue * sns_VoltageCurrent0Factor;
#if sns_VoltageCurrent0VorA==0
			txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_VOLTAGE;
#else
			txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_CURRENT;
#endif
			txMsg.Data[0] = 0;
			txMsg.Data[1] = (ADvalue>>(sns_VoltageCurrent0Scale-6+8))&0xff;
			txMsg.Data[2] = (ADvalue>>(sns_VoltageCurrent0Scale-6))&0xff;

			while (StdCan_Put(&txMsg) != StdCan_Ret_OK) {}
		}
#endif

#ifdef sns_VoltageCurrent1AD
		if (VoltageCurrentChannelToSend==1)
		{
			ADvalue = ADC_Get(sns_VoltageCurrent1AD);
			ADvalue = ADvalue * sns_VoltageCurrent1Factor;
#if sns_VoltageCurrent1VorA==0
			txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_VOLTAGE;
#else
			txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_CURRENT;
#endif
			txMsg.Data[0] = 1;
			txMsg.Data[1] = (ADvalue>>(sns_VoltageCurrent1Scale-6+8))&0xff;
			txMsg.Data[2] = (ADvalue>>(sns_VoltageCurrent1Scale-6))&0xff;

			while (StdCan_Put(&txMsg) != StdCan_Ret_OK) {}
		}
#endif

#ifdef sns_VoltageCurrent2AD
		if (VoltageCurrentChannelToSend==2)
		{
			ADvalue = ADC_Get(sns_VoltageCurrent2AD);
			ADvalue = ADvalue * sns_VoltageCurrent2Factor;
#if sns_VoltageCurrent2VorA==0
			txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_VOLTAGE;
#else
			txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_CURRENT;
#endif
			txMsg.Data[0] = 2;
			txMsg.Data[1] = (ADvalue>>(sns_VoltageCurrent2Scale-6+8))&0xff;
			txMsg.Data[2] = (ADvalue>>(sns_VoltageCurrent2Scale-6))&0xff;

			while (StdCan_Put(&txMsg) != StdCan_Ret_OK) {}
		}
#endif

#ifdef sns_VoltageCurrent3AD
		if (VoltageCurrentChannelToSend==3)
		{
			ADvalue = ADC_Get(sns_VoltageCurrent3AD);
			ADvalue = ADvalue * sns_VoltageCurrent3Factor;
#if sns_VoltageCurrent3VorA==0
			txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_VOLTAGE;
#else
			txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_CURRENT;
#endif
			txMsg.Data[0] = 3;
			txMsg.Data[1] = (ADvalue>>(sns_VoltageCurrent3Scale-6+8))&0xff;
			txMsg.Data[2] = (ADvalue>>(sns_VoltageCurrent3Scale-6))&0xff;

			while (StdCan_Put(&txMsg) != StdCan_Ret_OK) {}
		}
#endif

#ifdef sns_VoltageCurrent4AD
		if (VoltageCurrentChannelToSend==4)
		{
			ADvalue = ADC_Get(sns_VoltageCurrent4AD);
			ADvalue = ADvalue * sns_VoltageCurrent4Factor;
#if sns_VoltageCurrent4VorA==0
			txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_VOLTAGE;
#else
			txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_CURRENT;
#endif
			txMsg.Data[0] = 4;
			txMsg.Data[1] = (ADvalue>>(sns_VoltageCurrent4Scale-6+8))&0xff;
			txMsg.Data[2] = (ADvalue>>(sns_VoltageCurrent4Scale-6))&0xff;

			while (StdCan_Put(&txMsg) != StdCan_Ret_OK) {}
		}
#endif

#ifdef sns_VoltageCurrent5AD
		if (VoltageCurrentChannelToSend==5)
		{
			ADvalue = ADC_Get(sns_VoltageCurrent5AD);
			ADvalue = ADvalue * sns_VoltageCurrent5Factor;
#if sns_VoltageCurrent5VorA==0
			txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_VOLTAGE;
#else
			txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_CURRENT;
#endif
			txMsg.Data[0] = 5;
			txMsg.Data[1] = (ADvalue>>(sns_VoltageCurrent5Scale-6+8))&0xff;
			txMsg.Data[2] = (ADvalue>>(sns_VoltageCurrent5Scale-6))&0xff;

			while (StdCan_Put(&txMsg) != StdCan_Ret_OK) {}
		}
#endif

		if (VoltageCurrentChannelToSend++ >=6)
		{
			VoltageCurrentChannelToSend=0;
		}
	}
}

void sns_VoltageCurrent_HandleMessage(StdCan_Msg_t *rxMsg)
{
/*	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS && 
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_VOLTAGECURRENT && 
		rxMsg->Header.ModuleId == sns_VoltageCurrent_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}*/
}

void sns_VoltageCurrent_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_VOLTAGECURRENT; 
	txMsg.Header.ModuleId = sns_VoltageCurrent_ID;
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
