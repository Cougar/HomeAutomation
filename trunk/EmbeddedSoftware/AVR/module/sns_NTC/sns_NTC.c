#include <math.h>
#include "sns_NTC.h"

uint8_t sns_NTC_ReportInterval = (uint8_t)sns_NTC_SEND_PERIOD;

void sns_NTC_Init(void)
{
	ADC_Init();
	Timer_SetTimeout(sns_NTC_TIMER, sns_NTC_ReportInterval*1000 , TimerTypeFreeRunning, 0);
	Timer_SetTimeout(sns_NTC_SAMPLE_TIMER, sns_NTC_SAMPLE_PERIOD , TimerTypeFreeRunning, 0);
}

void sns_NTC_Process(void)
{
	static uint32_t ad_filter;

	if (Timer_Expired(sns_NTC_SAMPLE_TIMER)) {
		uint32_t x = ADC_Get(sns_NTC_AD_CH);
		ad_filter += x - ad_filter / sns_NTC_FILTER_LEN;
	}

	if (Timer_Expired(sns_NTC_TIMER)) {
		//T=B/{ln{R/R_0}+B/T_0}
		const float B = (float)sns_NTC_B;
		const float B_div_T_0 = (float)sns_NTC_B / ((float)sns_NTC_T_0 + 273.15f);
		const float R_0 = (float)sns_NTC_R_0;
		float R;
		float T;
		R = R_0 * ad_filter / (1024.0f * sns_NTC_FILTER_LEN - ad_filter);
		T = B / (log(R / R_0) + B_div_T_0);
		int16_t T_10_6 = (int16_t)((T - 273.15f) * 64.0f);

		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_NTC;
		txMsg.Header.ModuleId = sns_NTC_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_TEMPERATURE_CELSIUS;
		txMsg.Length = 3;
		txMsg.Data[0] = 0;
		txMsg.Data[1] = (T_10_6>>8)&0xff;
		txMsg.Data[2] = (T_10_6)&0xff;

		StdCan_Put(&txMsg);
	}
}

void sns_NTC_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_NTC &&
		rxMsg->Header.ModuleId == sns_NTC_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL:
		if (rxMsg->Length > 0)
		{
			sns_NTC_ReportInterval = rxMsg->Data[0];
			Timer_SetTimeout(sns_NTC_TIMER, sns_NTC_ReportInterval*1000 , TimerTypeFreeRunning, 0);
		}

		StdCan_Msg_t txMsg;

		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_NTC;
		txMsg.Header.ModuleId = sns_NTC_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL;
		txMsg.Length = 1;

		txMsg.Data[0] = sns_NTC_ReportInterval;

		StdCan_Put(&txMsg);
		break;
		}
	}
}

void sns_NTC_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_NTC; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = sns_NTC_ID;
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
