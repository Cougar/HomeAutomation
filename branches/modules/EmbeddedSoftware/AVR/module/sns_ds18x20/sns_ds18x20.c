
#include "sns_ds18x20.h"

void ConvertTemperature(void);
void ConvertTemperature_callback(uint8_t timer);
void ReadTemperature(void);
void ReadTemperature_callback(uint8_t timer);

uint8_t NumberOfSensors = 0;
uint16_t SensorIds[] = {0, 0, 0, 0};
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

	StdCan_Set_class(txMsg, CAN_CLASS_MODULE_SNS);
	StdCan_Set_direction(txMsg, DIR_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_TYPE_MODULE_sns_ds18x20;
	txMsg.Header.ModuleId = sns_ds18x20_ID;
	txMsg.Header.Command = CAN_CMD_MODULE_SNS_TEMPERATURE_CELSIUS;
	txMsg.Length = 3;

	txMsg.Data[0] = SensorIds[CurrentSensor];

	if (DS18X20_read_meas(&gSensorIDs[CurrentSensor][0], &subzero, &cel, &cel_frac_bits) == DS18X20_OK)
	{
		if (subzero)
		{
			txMsg.Data[1] = -cel-1;
			txMsg.Data[2] = ~(cel_frac_bits<<4);
		}
		else
		{
			txMsg.Data[1] = cel;
			txMsg.Data[2] = (cel_frac_bits<<4);
		}
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

		Timer_SetTimeout(sns_ds18x20_TIMER, sns_ds18x20_SEND_PERIOD , TimerTypeOneShot, &ConvertTemperature_callback);
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
	NumberOfSensors = search_sensors();
	
	Timer_SetTimeout(sns_ds18x20_TIMER, sns_ds18x20_SEND_PERIOD , TimerTypeFreeRunning, &ConvertTemperature_callback);
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
}

void sns_ds18x20_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg, CAN_CLASS_MODULE_SNS);
	StdCan_Set_direction(txMsg, DIR_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_TYPE_MODULE_sns_ds18x20;
	txMsg.Header.ModuleId = sns_ds18x20_ID;
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
