
#include "sns_irReceive.h"

uint8_t state = sns_irReceive_STATE_IDLE;
Ir_Protocol_Data_t proto;
uint8_t len=0;
uint16_t rxbuffer[MAX_NR_TIMES];
StdCan_Msg_t irTxMsg;


#if (sns_irReceive_SEND_DEBUG==1)
void send_debug(uint16_t *buffer, uint8_t len) {

	/* the protocol is unknown so the raw ir-data is sent, makes it easier to develop a new protocol */

	StdCan_Set_class(irTxMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(irTxMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	irTxMsg.Length = 8;
	irTxMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_IRRECEIVE;
	irTxMsg.Header.ModuleId = sns_irReceive_ID;
	irTxMsg.Header.Command = CAN_MODULE_CMD_IRRECEIVE_IRRAW;
	for (uint8_t i = 0; i < len>>2; i++) {
		uint8_t index = i<<2;

		irTxMsg.Data[0] = (buffer[index]>>8)&0xff;
		irTxMsg.Data[1] = (buffer[index]>>0)&0xff;
		irTxMsg.Data[2] = (buffer[index+1]>>8)&0xff;
		irTxMsg.Data[3] = (buffer[index+1]>>0)&0xff;
		irTxMsg.Data[4] = (buffer[index+2]>>8)&0xff;
		irTxMsg.Data[5] = (buffer[index+2]>>0)&0xff;
		irTxMsg.Data[6] = (buffer[index+3]>>8)&0xff;
		irTxMsg.Data[7] = (buffer[index+3]>>0)&0xff;
				
		/* buffers will be filled when sending more than 2-3 messages, so retry until sent */
		while (StdCan_Put(&irTxMsg) != StdCan_Ret_OK) {}
	}
	
	uint8_t lastpacketcnt = len&0x03;
	if (lastpacketcnt > 0) {
		irTxMsg.Length = lastpacketcnt<<1;
		for (uint8_t i = 0; i < lastpacketcnt; i++) {
			irTxMsg.Data[i<<1] = (buffer[(len&0xfc)|i]>>8)&0xff;
			irTxMsg.Data[(i<<1)+1] = (buffer[(len&0xfc)|i]>>0)&0xff;
		}
		/* buffers will be filled when sending more than 2-3 messages, so retry until sent */
		while (StdCan_Put(&irTxMsg) != StdCan_Ret_OK) {}
	}

}
#endif

void sns_irReceive_Init(void)
{
	IrTransceiver_Init();
	proto.timeout=0; proto.data=0; proto.repeats=0; proto.protocol=0;

}

void sns_irReceive_Process(void)
{
	uint8_t res;
	switch (state)
	{
	case sns_irReceive_STATE_IDLE:
		IrTransceiver_Receive_Start(rxbuffer);
		state = sns_irReceive_STATE_START_RECEIVE;
		break;

	case sns_irReceive_STATE_START_RECEIVE:
		state = sns_irReceive_STATE_RECEIVING;
		break;

	case sns_irReceive_STATE_RECEIVING:
		res = IrTransceiver_Receive_Poll(&len);
		if ((res == IR_OK) && (len > 0)) {
			//låt protocols parsa och skicka sen på can
			uint8_t res2 = parseProtocol(rxbuffer, len, &proto);
			if (res2 == IR_OK && proto.protocol != IR_PROTO_UNKNOWN) {
				StdCan_Set_class(irTxMsg.Header, CAN_MODULE_CLASS_SNS);
				StdCan_Set_direction(irTxMsg.Header, DIRECTIONFLAG_FROM_OWNER);
				irTxMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_IRRECEIVE;
				irTxMsg.Header.ModuleId = sns_irReceive_ID;
				irTxMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_IR;
				irTxMsg.Data[0] = IR_BUTTON_PRESSED;
				irTxMsg.Data[1] = proto.protocol;
				irTxMsg.Data[2] = (proto.data>>24)&0xff;
				irTxMsg.Data[3] = (proto.data>>16)&0xff;
				irTxMsg.Data[4] = (proto.data>>8)&0xff;
				irTxMsg.Data[5] = proto.data&0xff;
				irTxMsg.Length = 6;
				StdCan_Put(&irTxMsg);
			} else if (proto.protocol == IR_PROTO_UNKNOWN) {
#if (sns_irReceive_SEND_DEBUG==1)
				send_debug(rxbuffer, len);
				proto.timeout=300;
#endif
			}
			
			state = sns_irReceive_STATE_START_PAUSE;
		}
		break;

	case sns_irReceive_STATE_START_PAUSE:
		IrTransceiver_Receive_Start(rxbuffer);
		Timer_SetTimeout(sns_irReceive_REPEATE_TIMER, proto.timeout, TimerTypeOneShot, 0);
		state = sns_irReceive_STATE_PAUSING;
		break;

	case sns_irReceive_STATE_PAUSING:
		//resetta timebase-var om ir finns
		res = IrTransceiver_Receive_Poll(&len);
		if (res == IR_NOT_FINISHED || res == IR_OK) {
			Timer_SetTimeout(sns_irReceive_REPEATE_TIMER, proto.timeout, TimerTypeOneShot, 0);
		}
		if (res == IR_OK) {
			/* start a new reception */
			IrTransceiver_Receive_Start(rxbuffer);
		}
		
		if (Timer_Expired(sns_irReceive_REPEATE_TIMER)) {
			state = sns_irReceive_STATE_START_IDLE;
		}
		break;

	case sns_irReceive_STATE_START_IDLE:
		if (proto.protocol != IR_PROTO_UNKNOWN) {
			//skicka på can
			irTxMsg.Data[0] = IR_BUTTON_RELEASED;
			StdCan_Put(&irTxMsg);
		}
		state = sns_irReceive_STATE_IDLE;
		break;

	}

}

void sns_irReceive_HandleMessage(StdCan_Msg_t *rxMsg)
{
	/*if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS && 
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_IRRECEIVE && 
		rxMsg->Header.ModuleId == sns_irReceive_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}*/
}

void sns_irReceive_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS); 
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_IRRECEIVE; 

	txMsg.Header.ModuleId = sns_irReceive_ID;
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
