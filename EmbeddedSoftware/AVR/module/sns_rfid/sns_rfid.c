
#include "sns_rfid.h"

#include "rfid.h"

uint8_t sns_rfid_got_card = 0;

void sns_rfid_Init(void)
{
	rfid_init();

	gpio_set_out( sns_rfid_STATUS_LED );
}

void sns_rfid_Process(void)
{
	uint8_t version;
	uint32_t id;
	if( 0 == rfid_fetch( &version, &id ) ) {
		if( !sns_rfid_got_card ) {
			sns_rfid_got_card = 1;
			sns_rfid_card.id      = id;
			sns_rfid_card.version = version;
			// Card arrived
			sns_rfid_HandleCardEvent(1);
		}
		Timer_SetTimeout(sns_rfid_CARD_LEFT_TIMER, sns_rfid_CARD_LEFT_TIME, TimerTypeOneShot, 0);
	}
	if ( Timer_Expired(sns_rfid_CARD_LEFT_TIMER) ) {
		sns_rfid_got_card = 0;
		// Card left
		sns_rfid_HandleCardEvent(0);
	}
}

void sns_rfid_HandleCardEvent( uint8_t event ) {
	StdCan_Msg_t txMsg;
	
	gpio_set_statement( event, sns_rfid_STATUS_LED );

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_RFID; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = sns_rfid_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_LIST;
	txMsg.Length = 6;

	txMsg.Data[0] = event;
	txMsg.Data[1] =  sns_rfid_card.version;
	txMsg.Data[2] =  sns_rfid_card.id&0xff;
	txMsg.Data[3] = (sns_rfid_card.id>>8)&0xff;
	txMsg.Data[4] = (sns_rfid_card.id>>16)&0xff;
	txMsg.Data[5] = (sns_rfid_card.id>>24)&0xff;

	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}

void sns_rfid_HandleMessage(StdCan_Msg_t *rxMsg)
{
/*	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS && ///TODO: Change this to the actual class type
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_RFID && ///TODO: Change this to the actual module type
		rxMsg->Header.ModuleId == sns_rfid_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}*/
}

void sns_rfid_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_RFID; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = sns_rfid_ID;
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
