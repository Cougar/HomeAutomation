
#include "sns_DHT11.h"
uint8_t sendNewTemp = 0;
uint8_t temperature = 0;
uint8_t humidity = 0;

#ifdef sns_DHT11_USEEEPROM
#include "sns_DHT11_eeprom.h"
struct eeprom_sns_DHT11 EEMEM eeprom_sns_DHT11 =
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file.
		0xAB,		// x
		0x1234		// y
		0x12345678	// z
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
};
#endif

void ConvertTemperature2_callback(uint8_t timer)
{
	uint8_t word[5];
	if (dht11_readData((uint8_t*) word)) {
		sendNewTemp = 1;
		temperature = word[2];
		humidity = word[0];
	}
}

void ConvertTemperature_callback_DHT11(uint8_t timer)
{
	dht11_start();
	Timer_SetTimeout(sns_DHT11_18MS_TIMER, 18, TimerTypeOneShot, &ConvertTemperature2_callback);
}

void sns_DHT11_Init(void)
{
#ifdef sns_DHT11_USEEEPROM
	if (EEDATA_OK)
	{
	  ///TODO: Use stored data to set initial values for the module
	  blablaX = eeprom_read_byte(EEDATA.x);
	  blablaY = eeprom_read_word(EEDATA16.y);
	  blablaZ = eeprom_read_dword(EEDATA32.y);
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_byte_crc(EEDATA.x, 0xAB, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.y, 0x1234, WITHOUT_CRC);
	  eeprom_write_dword_crc(EEDATA32.y, 0x12345678, WITHOUT_CRC);
	  EEDATA_UPDATE_CRC;
	}
#endif
	///TODO: Initialize hardware etc here

	// to use PCINt lib, call this function: (the callback function look as a timer callback function)
	// Pcint_SetCallbackPin(sns_DHT11_PCINT, EXP_C , &sns_DHT11_pcint_callback);
	Timer_SetTimeout(sns_DHT11_TIMER, 5000, TimerTypeFreeRunning, &ConvertTemperature_callback_DHT11);
}

void sns_DHT11_Process(void)
{
	if (sendNewTemp) {
		uint16_t temp;
		sendNewTemp = 0;

		StdCan_Msg_t txMsg;

		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_DHT11;
		txMsg.Header.ModuleId = sns_DHT11_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_TEMPERATURE_CELSIUS;
		txMsg.Length = 3;

		txMsg.Data[0] = 1;
		temp = temperature*64;
		txMsg.Data[1] = (uint8_t)(temp >> 8);
		txMsg.Data[2] = (uint8_t)(temp & 0xff);
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);

		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_DHT11;
		txMsg.Header.ModuleId = sns_DHT11_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_HUMIDITY_PERCENT;
		txMsg.Length = 3;

		txMsg.Data[0] = 1;
		temp = humidity*64;
		txMsg.Data[1] = (uint8_t)(temp >> 8);
		txMsg.Data[2] = (uint8_t)(temp & 0xff);
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);

	}

	///TODO: Stuff that needs doing is done here
}

void sns_DHT11_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS && ///TODO: Change this to the actual class type
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_DHT11 && ///TODO: Change this to the actual module type
		rxMsg->Header.ModuleId == sns_DHT11_ID)
	{
		StdCan_Msg_t txMsg;
		switch (rxMsg->Header.Command)
		{
			case CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL:
				//if (rxMsg->Length > 0)
				//	sns_ds18x20_ReportInterval = rxMsg->Data[0];


				StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
				StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
				txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_DHT11;
				txMsg.Header.ModuleId = sns_DHT11_ID;
				txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL;
				txMsg.Length = 1;

				//txMsg.Data[0] = sns_ds18x20_ReportInterval;

				StdCan_Put(&txMsg);
				break;
		}
	}
}

void sns_DHT11_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_DHT11; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = sns_DHT11_ID;
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
