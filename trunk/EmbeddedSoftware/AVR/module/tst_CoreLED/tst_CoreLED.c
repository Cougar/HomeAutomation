
#include "tst_CoreLED.h"

void tst_CoreLED_Init(void)
{
	/// Initialize hardware etc here

	Timer_SetTimeout(tst_CoreLED_Blink_TIMER, tst_CoreLED_Blink_Interval , TimerTypeFreeRunning, 0);
	
	gpio_set_out(tst_CoreLED_PIN);
	
}

void tst_CoreLED_Process(void)
{
	/// Stuff that needs doing is done here
	
	if (Timer_Expired(tst_CoreLED_Blink_TIMER))
	{
		gpio_toggle_pin(tst_CoreLED_PIN);
	}
}

void tst_CoreLED_HandleMessage(StdCan_Msg_t *rxMsg)
{
	/*if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_xyz && ///TODO: Change this to the actual class type
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_xyz && ///TODO: Change this to the actual module type
		rxMsg->Header.ModuleId == tst_CoreLED_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}*/
}

void tst_CoreLED_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_TST);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_TST_DEBUG;
	txMsg.Header.ModuleId = tst_CoreLED_ID;
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
