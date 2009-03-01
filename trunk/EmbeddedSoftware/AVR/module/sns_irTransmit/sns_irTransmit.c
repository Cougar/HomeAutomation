
#include "sns_irTransmit.h"

uint8_t sns_irTransmit_state = sns_irTransmit_STATE_IDLE;
uint8_t sns_irTransmit_repeatCount = 0;
uint8_t sns_irTransmit_stop = 0;
Ir_Protocol_Data_t sns_irTransmit_proto;
uint16_t sns_irTransmit_txbuffer[MAX_NR_TIMES];
uint8_t sns_irTransmit_length = 0;

void sns_irTransmit_Init(void)
{
	IrTransceiver_Init();
	sns_irTransmit_proto.timeout = 0;
	sns_irTransmit_proto.data = 0;
	sns_irTransmit_proto.repeats = 0;
	sns_irTransmit_proto.protocol = 0;
	sns_irTransmit_proto.framecnt = 0;
}

void sns_irTransmit_Process(void)
{
	if (sns_irTransmit_state == sns_irTransmit_STATE_IDLE)
	{
	}
	else if (sns_irTransmit_state == sns_irTransmit_STATE_START_TRANSMIT)
	{
		if (expandProtocol(sns_irTransmit_txbuffer, &sns_irTransmit_length, &sns_irTransmit_proto) == IR_OK)
		{
			if (IrTransceiver_Transmit(sns_irTransmit_txbuffer, sns_irTransmit_length, sns_irTransmit_proto.modfreq) == IR_OK)
			{
				sns_irTransmit_state = sns_irTransmit_STATE_TRANSMITTING;
			}
			else
			{
				sns_irTransmit_state = sns_irTransmit_STATE_IDLE;
			}
		}
		else
		{
			sns_irTransmit_state = sns_irTransmit_STATE_IDLE;
		}
	}
	else if (sns_irTransmit_state == sns_irTransmit_STATE_TRANSMITTING){
		//polla om den är klar, om klar gå till sns_irTransmit_STATE_START_PAUSE
		if (IrTransceiver_Transmit_Poll() != IR_NOT_FINISHED)
		{
			sns_irTransmit_state = sns_irTransmit_STATE_START_PAUSE;
		}
	}
	else if (sns_irTransmit_state == sns_irTransmit_STATE_START_PAUSE)
	{
		if (sns_irTransmit_repeatCount < sns_irTransmit_proto.repeats)
		{
			sns_irTransmit_repeatCount++;
		}
		
		Timer_SetTimeout(sns_irTransmit_REPEAT_TIMER, sns_irTransmit_proto.timeout, TimerTypeOneShot, 0);
		
		if (sns_irTransmit_proto.framecnt != 255)
		{
			sns_irTransmit_proto.framecnt++;
		}
		
		sns_irTransmit_state = sns_irTransmit_STATE_PAUSING;
	}
	else if (sns_irTransmit_state == sns_irTransmit_STATE_PAUSING)
	{
		//när timeout har gått (timebase) så gå till sns_irTransmit_STATE_START_TRANSMIT
		if (Timer_Expired(sns_irTransmit_REPEAT_TIMER))
		{
			sns_irTransmit_state = sns_irTransmit_STATE_START_TRANSMIT;
		}
		
		if (sns_irTransmit_stop == 1 && sns_irTransmit_repeatCount >= sns_irTransmit_proto.repeats)
		{
			sns_irTransmit_state = sns_irTransmit_STATE_STOP;
		}
	}
	else if (sns_irTransmit_state == sns_irTransmit_STATE_STOP)
	{
		sns_irTransmit_stop = 0;
		sns_irTransmit_proto.timeout = 0;
		sns_irTransmit_proto.framecnt = 0;
		sns_irTransmit_repeatCount = 0;
		sns_irTransmit_state = sns_irTransmit_STATE_IDLE;
	}
}

void sns_irTransmit_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS && 
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_IRTRANSMIT && 
		rxMsg->Header.ModuleId == sns_irTransmit_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_PHYSICAL_IR:
		
		if (sns_irTransmit_state == sns_irTransmit_STATE_IDLE && rxMsg->Data[0] == CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_PRESSED)
		{
			sns_irTransmit_proto.protocol = rxMsg->Data[1];

			sns_irTransmit_proto.data = rxMsg->Data[2];
			sns_irTransmit_proto.data = sns_irTransmit_proto.data<<8;
			sns_irTransmit_proto.data |= rxMsg->Data[3];
			sns_irTransmit_proto.data = sns_irTransmit_proto.data<<8;
			sns_irTransmit_proto.data |= rxMsg->Data[4];
			sns_irTransmit_proto.data = sns_irTransmit_proto.data<<8;
			sns_irTransmit_proto.data |= rxMsg->Data[5];

			sns_irTransmit_state = sns_irTransmit_STATE_START_TRANSMIT;
		}
		else if (sns_irTransmit_state != sns_irTransmit_STATE_IDLE && rxMsg->Data[0] == CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_RELEASED)
		{
			sns_irTransmit_stop = 1;
		}
		break;
		}
	}
}

void sns_irTransmit_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_IRTRANSMIT;
	txMsg.Header.ModuleId = sns_irTransmit_ID;
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
