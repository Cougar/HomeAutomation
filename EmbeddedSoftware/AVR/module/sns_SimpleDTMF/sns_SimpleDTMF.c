
#include "sns_SimpleDTMF.h"

uint8_t rxbuffer[MT_MAX_TONES];
uint8_t state = SNS_SIMPLEDTMF_STATE_IDLE;
uint8_t rxlen = 0;

void sns_SimpleDTMF_Init(void)
{
	DTMFin_Init();
}

void sns_SimpleDTMF_Process(void)
{
	uint8_t retval;
	StdCan_Msg_t txMsg;
	
	switch (state)
	{
	case SNS_SIMPLEDTMF_STATE_IDLE:
	DTMFin_Start(rxbuffer);
	state = SNS_SIMPLEDTMF_STATE_START_WAIT;
	break;
	
	case SNS_SIMPLEDTMF_STATE_START_WAIT:
	state = SNS_SIMPLEDTMF_STATE_WAIT;
	break;
	
	case SNS_SIMPLEDTMF_STATE_WAIT:
	retval = DTMFin_Poll(&rxlen);
	
	if (retval == MT8870_Ret_Finished)
		state = SNS_SIMPLEDTMF_STATE_STOP;
	else if (retval == MT8870_Ret_Overflow)
		state = SNS_SIMPLEDTMF_STATE_IDLE;
		
	break;
	
	case SNS_SIMPLEDTMF_STATE_STOP:
	//if ((rxbuffer[0] == 0xa || rxbuffer[0] == 0xb) && rxbuffer[rxlen-1] == 0xc) {
	
		StdCan_Set_class(txMsg.Header, CAN_CLASS_MODULE_SNS);
		StdCan_Set_direction(txMsg.Header, DIR_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_TYPE_MODULE_sns_SimpleDTMF;
		txMsg.Header.ModuleId = sns_SimpleDTMF_ID;
		txMsg.Header.Command = CAN_CMD_MODULE_PHYS_PHONENUMBER;
		
		for (uint8_t i = 0; i < 16; i=i+2) {
			txMsg.Data[i>>1] = rxbuffer[i]<<4;
			
			if (i+1 < rxlen)
				txMsg.Data[i>>1] |= rxbuffer[i+1];
		}
		txMsg.Length = (rxlen>>1)+(rxlen&1);
		
		StdCan_Put(&txMsg);

	//}
	state = SNS_SIMPLEDTMF_STATE_IDLE;
	break;
	}
}

void sns_SimpleDTMF_HandleMessage(StdCan_Msg_t *rxMsg)
{
	/*if (	StdCan_Ret_class(rxMsg->Header) == CAN_CLASS_MODULE_SNS && ///TODO: Change this to the actual class type
		StdCan_Ret_direction(rxMsg->Header) == DIR_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_TYPE_MODULE_sns_SimpleDTMF && ///TODO: Change this to the actual module type
		rxMsg->Header.ModuleId == sns_SimpleDTMF_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}*/
}

void sns_SimpleDTMF_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_CLASS_MODULE_SNS);
	StdCan_Set_direction(txMsg.Header, DIR_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_TYPE_MODULE_sns_SimpleDTMF;
	txMsg.Header.ModuleId = sns_SimpleDTMF_ID;
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
