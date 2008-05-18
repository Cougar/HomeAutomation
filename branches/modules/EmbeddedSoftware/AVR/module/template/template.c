
#include "<template>.h"

void <template>_Init(void)
{
	///TODO: Initialize hardware etc here
}

void <template>_Process(void)
{
	///TODO: Stuff that needs doing is done here
}

void <template>_HandleMessage(StdCan_Msg_t *rxMsg)
{
	StdCan_Msg_t txMsg;
	uint8_t n = 0;

	if (	StdCan_Ret_class(rxMsg->Header) == CAN_CLASS_MODULE_<template> && ///TODO: Change this to the actual class type
		StdCan_Ret_direction(rxMsg->Header) == DIR_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_TYPE_MODULE_<template> && ///TODO: Change this to the actual module type
		rxMsg->Header.ModuleId == <template>_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}
}

void <template>_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_CLASS_MODULE_<template>); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIR_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_TYPE_MODULE_<template>; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = <template>_ID;
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
