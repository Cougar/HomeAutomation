#include "sns_ds18x20.h"

#ifdef CHN_LOADED
#include "../chn_ChnMaster/chn_ChnMaster.h"
#endif

void ConvertTemperature(void);
void ConvertTemperature_callback(uint8_t timer);
void ReadTemperature(void);
void ReadTemperature_callback(uint8_t timer);

uint8_t sns_ds18x20_ReportInterval = (uint8_t)sns_ds18x20_SEND_PERIOD;
uint8_t NumberOfSensors = 0;
uint16_t SensorIds[DS18x20_MAXSENSORS];
uint8_t FlagReadTemperature = 0, FlagConvertTemperature = 0, FlagSearchSensors = 0;

void ReadTemperature_callback(uint8_t timer)
{
	FlagReadTemperature = 1;
}


void ReadTemperature(void)
{
	static uint8_t SearchSensorsTimeout = sns_ds18x20_SEARCH_TIMEOUT;
	static uint8_t CurrentSensor = 0;
	uint8_t subzero, cel, cel_frac_bits = 0;
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_DS18X20;
	txMsg.Header.ModuleId = sns_ds18x20_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_TEMPERATURE_CELSIUS;
	txMsg.Length = 3;

	txMsg.Data[0] = SensorIds[CurrentSensor];

	if (DS18X20_read_meas(&gSensorIDs[CurrentSensor][0], &subzero, &cel, &cel_frac_bits) == DS18X20_OK)
	{
		int16_t temp = cel;

		temp = temp << 6;

		temp += (cel_frac_bits << 2);

		if (subzero)
		{
			temp = -temp;
		}

		txMsg.Data[1] = (uint8_t)(temp >> 8);
		txMsg.Data[2] = (uint8_t)(temp & 0xff);

#ifdef CHN_LOADED
        chn_ChnMaster_UpdateChannel( sns_ds18x20_CHN_CHANNEL, (temp<<2)+25600 );
#endif
	}
	
	StdCan_Put(&txMsg);

	CurrentSensor++;
	if (CurrentSensor < NumberOfSensors)
		Timer_SetTimeout(sns_ds18x20_TIMER, sns_ds18x20_SEND_DELAY, TimerTypeOneShot, &ReadTemperature_callback);
	else
	{
		SearchSensorsTimeout--;
		if (SearchSensorsTimeout == 0)
		{
			SearchSensorsTimeout = sns_ds18x20_SEARCH_TIMEOUT;
			FlagSearchSensors = 1;
		}
		
		CurrentSensor = 0;

		Timer_SetTimeout(sns_ds18x20_TIMER, sns_ds18x20_ReportInterval*1000, TimerTypeOneShot, &ConvertTemperature_callback);
	}
}

void ConvertTemperature_callback(uint8_t timer)
{
	FlagConvertTemperature = 1;
}

void ConvertTemperature(void)
{
	if (DS18X20_start_meas(DS18X20_POWER_PARASITE, NULL) == DS18X20_OK)
	{
		Timer_SetTimeout(sns_ds18x20_TIMER, DS18B20_TCONV_12BIT, TimerTypeOneShot, &ReadTemperature_callback);
	}
}

void sns_ds18x20_Init(void)
{
	/* Make sure there is no more then 4 sensors. */
	FlagSearchSensors = 1;
	NumberOfSensors = 0;
	uint8_t i;
	for (i=0; i < DS18x20_MAXSENSORS;i++) {
	  SensorIds[i] = 0;
	}
	Timer_SetTimeout(sns_ds18x20_TIMER, sns_ds18x20_ReportInterval*1000, TimerTypeFreeRunning, &ConvertTemperature_callback);
}

void sns_ds18x20_Process(void)
{
	if (FlagConvertTemperature)
	{
		ConvertTemperature();
		FlagConvertTemperature = 0;
	}

	if (FlagSearchSensors)
	{
		NumberOfSensors = search_sensors();
		uint8_t i, j;

		for(i = 0; i < NumberOfSensors; i++)
		{
			SensorIds[i] = 0;
			for (j = 0; j < 7; j++)
			{
				SensorIds[i] ^= gSensorIDs[i][j];	
			}
		}
		FlagSearchSensors = 0;
	}

	if (FlagReadTemperature)
	{
		ReadTemperature();
		FlagReadTemperature = 0;
	}
}

void sns_ds18x20_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_DS18X20 &&
		rxMsg->Header.ModuleId == sns_ds18x20_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL:
		if (rxMsg->Length > 0)
			sns_ds18x20_ReportInterval = rxMsg->Data[0];

		StdCan_Msg_t txMsg;

		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_DS18X20;
		txMsg.Header.ModuleId = sns_ds18x20_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL;
		txMsg.Length = 1;

		txMsg.Data[0] = sns_ds18x20_ReportInterval;

		StdCan_Put(&txMsg);
		break;
		}
	}
}

void sns_ds18x20_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_DS18X20;
	txMsg.Header.ModuleId = sns_ds18x20_ID;
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
