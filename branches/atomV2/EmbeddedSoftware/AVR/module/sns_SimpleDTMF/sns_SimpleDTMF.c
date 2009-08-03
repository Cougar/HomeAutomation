
#include "sns_SimpleDTMF.h"

uint8_t pollrxbuffer[MT_MAX_TONES];
uint8_t pollrxlen=0;
uint8_t rxbuffer[MT_MAX_TONES];
uint8_t state = SNS_SIMPLEDTMF_STATE_IDLE;
uint8_t rxlen = 0;

void sns_SimpleDTMF_Init(void)
{
	DTMFin_Init();
}

void sns_SimpleDTMF_Process(void)
{
	StdCan_Msg_t txMsg;
	DTMF_Ret_t retval;
	
	switch (state)
	{
	case SNS_SIMPLEDTMF_STATE_IDLE:
		if (DTMFin_Pop(pollrxbuffer, &pollrxlen) == DTMF_Ret_Data_avail) {
			state = SNS_SIMPLEDTMF_STATE_COPYDATA;
		} else {
			//go into sleep or something, incoming data from mt8870 is interrupted so we will wake up
		}
	break;
	
	case SNS_SIMPLEDTMF_STATE_COPYDATA:
		state = SNS_SIMPLEDTMF_STATE_WAIT;
		for (uint8_t i=0; i<pollrxlen;i++) {
			rxbuffer[rxlen++] = pollrxbuffer[i];
			if (rxlen >= MT_MAX_TONES) {
				//overflow
				rxlen = 0;
				state = SNS_SIMPLEDTMF_STATE_IDLE;
				break;
			}
		}
		
		Timer_SetTimeout(MT_TIMER, MT_DIAL_TIMEOUT, TimerTypeOneShot, 0);
	break;
	
	case SNS_SIMPLEDTMF_STATE_WAIT:
		if (Timer_Expired(MT_TIMER)) {
			state = SNS_SIMPLEDTMF_STATE_SEND;
		}
		
		retval = DTMFin_Pop(pollrxbuffer, &pollrxlen);
		if (retval == DTMF_Ret_Data_avail) {
			state = SNS_SIMPLEDTMF_STATE_COPYDATA;
		} else if (retval == DTMF_Ret_Overflow) {
			rxlen=0;
			state = SNS_SIMPLEDTMF_STATE_IDLE;
		}
	break;
	
	case SNS_SIMPLEDTMF_STATE_SEND:
		//if ((rxbuffer[0] == 0xa || rxbuffer[0] == 0xb) && rxbuffer[rxlen-1] == 0xc) {
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_SIMPLEDTMF;
		txMsg.Header.ModuleId = sns_SimpleDTMF_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_PHONENUMBER;
		
		for (uint8_t i = 0; i < 16; i=i+2) {
			txMsg.Data[i>>1] = rxbuffer[i]<<4;
			
			if (i+1 < rxlen)
				txMsg.Data[i>>1] |= rxbuffer[i+1];
		}
		txMsg.Length = (rxlen>>1)+(rxlen&1);
		
		StdCan_Put(&txMsg);
		//}
		rxlen = 0;
		state = SNS_SIMPLEDTMF_STATE_IDLE;
	break;
	}
}

void sns_SimpleDTMF_HandleMessage(StdCan_Msg_t *rxMsg)
{
	/*if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS && ///TODO: Change this to the actual class type
		StdCan_Ret_direction(rxMsg->Header) == DIR_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_SIMPLEDTMF && ///TODO: Change this to the actual module type
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
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_SIMPLEDTMF;
	txMsg.Header.ModuleId = sns_SimpleDTMF_ID;
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
