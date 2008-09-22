
#include "sns_BusVoltage.h"

void sns_BusVoltage_Init(void)
{
	///TODO: Initialize hardware etc here 
	ADC_Init();
	Timer_SetTimeout(sns_BusVoltage_TIMER, sns_BusVoltage_SEND_PERIOD , TimerTypeFreeRunning, 0);
}

void sns_BusVoltage_Process(void)
{
	///TODO: Stuff that needs doing is done here
	if (Timer_Expired(sns_BusVoltage_TIMER)) {
		StdCan_Msg_t txMsg;
		uint16_t busVoltage = ADC_Get(BUSVOLTAGEAD);
		busVoltage = ((busVoltage>>2)*195)>>7; //((4.95*(12+47)/12) * 8)
		txMsg.Header.ModuleType = CAN_TYPE_MODULE_sns_BusVoltage;
		txMsg.Header.ModuleId = sns_BusVoltage_ID;
		txMsg.Header.Command = CAN_CMD_MODULE_PHYS_VOLTAGE;
		txMsg.Length = 3;
		txMsg.Data[0] = 0;
		txMsg.Data[2] = (busVoltage>>2)&0xff;
		txMsg.Data[1] = (busVoltage>>10)&0xff;

		StdCan_Put(&txMsg);
	}
}

void sns_BusVoltage_HandleMessage(StdCan_Msg_t *rxMsg)
{
#if 0
	StdCan_Msg_t txMsg;
	uint8_t n = 0;


	if (	StdCan_Ret_class(rxMsg->Header) == CAN_CLASS_MODULE_SNS && ///TODO: Change this to the actual class type
		StdCan_Ret_direction(rxMsg->Header) == DIR_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_TYPE_MODULE_TEMPLATE && ///TODO: Change this to the actual module type
		rxMsg->Header.ModuleId == sns_BusVoltage_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}
#endif
}

void sns_BusVoltage_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_CLASS_MODULE_SNS); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIR_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_TYPE_MODULE_sns_BusVoltage; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = sns_BusVoltage_ID;
	txMsg.Header.Command = CAN_CMD_MODULE_NMT_LIST;
	txMsg.Length = 6;

	txMsg.Data[0] = NODE_HW_ID_BYTE0;
	txMsg.Data[1] = NODE_HW_ID_BYTE1;
	txMsg.Data[2] = NODE_HW_ID_BYTE2;
	txMsg.Data[3] = NODE_HW_ID_BYTE3;
	
	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;
	
	StdCan_Put(&txMsg);
}
