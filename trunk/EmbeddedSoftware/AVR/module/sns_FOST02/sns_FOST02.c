
#include "sns_FOST02.h"



void GetTemperature(void);
void GetTemperature_callback(uint8_t timer);
void GetHumidity(void);
void GetHumidity_callback(uint8_t timer);

uint8_t FlagGetTemperature = 0, FlagGetHumidity = 0, byte1 = 0, byte2 = 0;

void GetTemperature_callback(uint8_t timer)
{
	FlagGetTemperature = 1;
	sendCommand(0x03);
}

void GetHumidity_callback(uint8_t timer)
{
	FlagGetHumidity = 1;
	sendCommand(0x05);
}

void GetTemperature(void)
{
	uint16_t TmpV;
	if (getFOST02PinStatus() == 0) {
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_FOST02;
		txMsg.Header.ModuleId = sns_FOST02_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_TEMPERATURE_CELSIUS;
		txMsg.Length = 3;
		txMsg.Data[0] = FOST02_ID;
	
		getFOST02DataByte(&byte1, &byte2);
		TmpV = (uint16_t) (byte1<<8) + byte2;
		TmpV = TmpV << 4;
		TmpV = TmpV/25;
		TmpV = TmpV << 2;
		TmpV = TmpV-(40*64);
		txMsg.Data[2]= (uint8_t) (TmpV & 0xFF);
		txMsg.Data[1]= (uint8_t) ((TmpV >> 8) & 0xFF);
		StdCan_Put(&txMsg);
		
		FlagGetTemperature = 0;
	}
	
}

void GetHumidity(void)
{
	uint16_t TmpV;
	if (getFOST02PinStatus() == 0) {
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_FOST02;
		txMsg.Header.ModuleId = sns_FOST02_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_HUMIDITY_PERCENT;
		txMsg.Length = 3;
		txMsg.Data[0] = FOST02_ID;
	
		getFOST02DataByte(&byte1, &byte2);
		TmpV = HumidityTable[byte2];
		txMsg.Data[2]= (uint8_t) ((TmpV>>2) & 0x00FF);
		txMsg.Data[1]= (uint8_t) ((TmpV >> 10) & 0x00FF);

		StdCan_Put(&txMsg);

		FlagGetHumidity = 0;
	}
}

void InitTemp_callback(uint8_t timer)
{
	Timer_SetTimeout(sns_FOST02_TEMP_TIMER, sns_FOST02_TEMP_SEND_PERIOD , TimerTypeFreeRunning, &GetTemperature_callback);
}

void sns_FOST02_Init(void)
{
	FlagGetTemperature = 0;
	FlagGetHumidity = 0;
	FOST02Init();

	Timer_SetTimeout(sns_FOST02_HYDRO_TIMER, sns_FOST02_HYDRO_SEND_PERIOD , TimerTypeFreeRunning, &GetHumidity_callback);
	// The reson for using sns_FOST02_HYDRO_SEND_PERIOD/2 as time out for this is to spread out the communication with the sensor if
	// both temp and humi has the same send period time.
	Timer_SetTimeout(sns_FOST02_TEMP_TIMER, sns_FOST02_HYDRO_SEND_PERIOD/2 , TimerTypeFreeRunning, &InitTemp_callback);
}

void sns_FOST02_Process(void)
{

	if (FlagGetHumidity)
	{
		GetHumidity();
	}

	if (FlagGetTemperature)
	{
		GetTemperature();
	}
}

void sns_FOST02_HandleMessage(StdCan_Msg_t *rxMsg)
{
}

void sns_FOST02_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_FOST02;
	txMsg.Header.ModuleId = sns_FOST02_ID;
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
