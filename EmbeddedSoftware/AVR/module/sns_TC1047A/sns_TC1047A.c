
#include "sns_TC1047A.h"

uint8_t sns_TC1047A_ReportInterval = (uint8_t)sns_TC1047A_SEND_PERIOD;

void sns_TC1047A_Init(void)
{
	///TODO: Initialize hardware etc here 
	ADC_Init();
	Timer_SetTimeout(sns_TC1047A_TIMER, sns_TC1047A_ReportInterval*1000 , TimerTypeFreeRunning, 0);
}

void sns_TC1047A_Process(void)
{
	///TODO: Stuff that needs doing is done here
	if (Timer_Expired(sns_TC1047A_TIMER)) {
		uint16_t temperature = ADC_Get(TC1047AAD);
		/* Use 5 volt as reference */
		temperature = (((temperature * 125)/(4))-3200);
		//temperature = temperature * 5;
		//temperature = ((uint16_t)(((uint32_t)temperature * 100) / 1024) - 50);
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_TC1047A;
		txMsg.Header.ModuleId = sns_TC1047A_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_TEMPERATURE_CELSIUS;
		txMsg.Length = 3;
		txMsg.Data[0] = 0;
		txMsg.Data[1] = (temperature>>8)&0xff;
		txMsg.Data[2] = (temperature)&0xff;

		StdCan_Put(&txMsg);
	}
}

void sns_TC1047A_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_TC1047A &&
		rxMsg->Header.ModuleId == sns_TC1047A_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL:
		if (rxMsg->Length > 0)
		{
			sns_TC1047A_ReportInterval = rxMsg->Data[0];
			Timer_SetTimeout(sns_TC1047A_TIMER, sns_TC1047A_ReportInterval*1000 , TimerTypeFreeRunning, 0);
		}

		StdCan_Msg_t txMsg;

		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_TC1047A;
		txMsg.Header.ModuleId = sns_TC1047A_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL;
		txMsg.Length = 1;

		txMsg.Data[0] = sns_TC1047A_ReportInterval;

		StdCan_Put(&txMsg);
		break;
		}
	}
}

void sns_TC1047A_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_TC1047A; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = sns_TC1047A_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_LIST;
	txMsg.Length = 6;

	uint32_t HwId=BIOS_GetHwId();
	txMsg.Data[0] = HwId&0xff;
	txMsg.Data[1] = (HwId>>8)&0xff;
	txMsg.Data[2] = (HwId>>16)&0xff;
	txMsg.Data[3] = (HwId>>24)&0xff;
	
	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;
	
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}
