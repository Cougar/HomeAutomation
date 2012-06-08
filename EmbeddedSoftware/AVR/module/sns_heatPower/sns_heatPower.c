
#include "sns_heatPower.h"

#if sns_heatPower_USEEEPROM==1
#include "sns_heatPower_eeprom.h"
struct eeprom_sns_heatPower EEMEM eeprom_sns_heatPower =
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

void sns_heatPower_Init(void)
{
#if sns_heatPower_USEEEPROM==1
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
	// Pcint_SetCallbackPin(sns_heatPower_PCINT, EXP_C , &sns_heatPower_pcint_callback);

}

void sns_heatPower_Process(void)
{
	///TODO: Stuff that needs doing is done here
}

void sns_heatPower_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_HEATPOWER &&
		rxMsg->Header.ModuleId == sns_heatPower_ID)
	{
		switch (rxMsg->Header.Command)
		{
		//case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}
}

void sns_heatPower_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_HEATPOWER;
	txMsg.Header.ModuleId = sns_heatPower_ID;
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
