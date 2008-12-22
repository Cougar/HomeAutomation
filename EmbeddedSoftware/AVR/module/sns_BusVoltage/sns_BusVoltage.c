
#include "sns_BusVoltage.h"

#define R51	12000
#define R50	47000

#if (5 * (R51 + R50))/(R51) > 64
	#error "ADC_FACTOR must be less then 64, change R51 and R52"
#elseif (5 * (R51 + R50))/(R51) < 32
	#define ADC_FACTOR	(5 * (R51 + R50))/(R51)
	#define ADC_SCALE	10
#else
	#define ADC_FACTOR	(5 * (R51 + R50))/(R51 / 2)
	#define ADC_SCALE	11
#endif

uint8_t sns_BusVoltage_ReportInterval = (uint8_t)sns_BusVoltage_SEND_PERIOD;

void sns_BusVoltage_Init(void)
{
	///TODO: Initialize hardware etc here 
	ADC_Init();
	Timer_SetTimeout(sns_BusVoltage_TIMER, sns_BusVoltage_ReportInterval*1000 , TimerTypeFreeRunning, 0);
}

void sns_BusVoltage_Process(void)
{
	///TODO: Stuff that needs doing is done here
	if (Timer_Expired(sns_BusVoltage_TIMER)) {
		uint16_t busVoltage = ADC_Get(BUSVOLTAGEAD);
		busVoltage = (busVoltage & 0x03ff) * ADC_FACTOR;
		
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_BUSVOLTAGE;
		txMsg.Header.ModuleId = sns_BusVoltage_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_VOLTAGE;
		txMsg.Length = 3;
		txMsg.Data[0] = 0;
		txMsg.Data[1] = (busVoltage>>(ADC_SCALE-6+8))&0xff;
		txMsg.Data[2] = (busVoltage>>(ADC_SCALE-6))&0xff;

		StdCan_Put(&txMsg);
	}
}

void sns_BusVoltage_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_BUSVOLTAGE &&
		rxMsg->Header.ModuleId == sns_BusVoltage_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL:
		if (rxMsg->Length > 0)
		{
			sns_BusVoltage_ReportInterval = rxMsg->Data[0];
			Timer_SetTimeout(sns_BusVoltage_TIMER, sns_BusVoltage_ReportInterval*1000 , TimerTypeFreeRunning, 0);
		}

		StdCan_Msg_t txMsg;

		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_BUSVOLTAGE;
		txMsg.Header.ModuleId = sns_BusVoltage_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL;
		txMsg.Length = 1;

		txMsg.Data[0] = sns_BusVoltage_ReportInterval;

		StdCan_Put(&txMsg);
		break;
		}
	}
}

void sns_BusVoltage_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_BUSVOLTAGE; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = sns_BusVoltage_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_LIST;
	txMsg.Length = 6;

	txMsg.Data[0] = NODE_HW_ID_BYTE0;
	txMsg.Data[1] = NODE_HW_ID_BYTE1;
	txMsg.Data[2] = NODE_HW_ID_BYTE2;
	txMsg.Data[3] = NODE_HW_ID_BYTE3;
	
	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;
	
	StdCan_Put(&txMsg);
}
