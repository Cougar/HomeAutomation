
#include "sns_irTransceive.h"

#if IR_RX_ENABLE==1
struct {
	uint8_t				state;
	uint8_t				newData;
	uint16_t			*rxbuf;
	uint8_t				rxlen;
	uint8_t				timerNum;
	Ir_Protocol_Data_t	proto;
} irRxChannel[sns_irTransceive_SUPPORTED_NUM_CHANNELS];
#endif

uint16_t	buf0[MAX_NR_TIMES];
uint16_t	buf1[MAX_NR_TIMES];
uint16_t	buf2[MAX_NR_TIMES];


StdCan_Msg_t		irTxMsg;

#if (sns_irTransceive_SEND_DEBUG==1)
void send_debug(uint16_t *buffer, uint8_t len) {
	StdCan_Msg_t dbgIrTxMsg;
	/* the protocol is unknown so the raw ir-data is sent, makes it easier to develop a new protocol */

	StdCan_Set_class(dbgIrTxMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(dbgIrTxMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	dbgIrTxMsg.Length = 8;
	dbgIrTxMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_IRTRANSCEIVE;
	dbgIrTxMsg.Header.ModuleId = sns_irTransceive_ID;
	dbgIrTxMsg.Header.Command = CAN_MODULE_CMD_IRTRANSCEIVE_IRRAW;
	for (uint8_t i = 0; i < len>>2; i++) {
		uint8_t index = i<<2;

		dbgIrTxMsg.Data[0] = (buffer[index]>>8)&0xff;
		dbgIrTxMsg.Data[1] = (buffer[index]>>0)&0xff;
		dbgIrTxMsg.Data[2] = (buffer[index+1]>>8)&0xff;
		dbgIrTxMsg.Data[3] = (buffer[index+1]>>0)&0xff;
		dbgIrTxMsg.Data[4] = (buffer[index+2]>>8)&0xff;
		dbgIrTxMsg.Data[5] = (buffer[index+2]>>0)&0xff;
		dbgIrTxMsg.Data[6] = (buffer[index+3]>>8)&0xff;
		dbgIrTxMsg.Data[7] = (buffer[index+3]>>0)&0xff;
				
		/* buffers will be filled when sending more than 2-3 messages, so retry until sent */
		while (StdCan_Put(&dbgIrTxMsg) != StdCan_Ret_OK) {}
		_delay_ms(1);
	}
	
	uint8_t lastpacketcnt = len&0x03;
	if (lastpacketcnt > 0) {
		dbgIrTxMsg.Length = lastpacketcnt<<1;
		for (uint8_t i = 0; i < lastpacketcnt; i++) {
			dbgIrTxMsg.Data[i<<1] = (buffer[(len&0xfc)|i]>>8)&0xff;
			dbgIrTxMsg.Data[(i<<1)+1] = (buffer[(len&0xfc)|i]>>0)&0xff;
		}
		/* buffers will be filled when sending more than 2-3 messages, so retry until sent */
		while (StdCan_Put(&dbgIrTxMsg) != StdCan_Ret_OK) {}
		_delay_ms(1);
	}

}
#endif

#if IR_RX_ENABLE==1
void sns_irTransceive_RX_done_callback(uint8_t channel, uint16_t *buffer, uint8_t len)
{
	if (channel < sns_irTransceive_SUPPORTED_NUM_CHANNELS)
	{
		irRxChannel[channel].newData = TRUE;
		irRxChannel[channel].rxlen = len;
	}
}
#endif

void sns_irTransceive_TX_done_callback(uint8_t channel)
{

}

void sns_irTransceive_Init(void)
{
gpio_set_out(EXP_A);
gpio_set_pin(EXP_A);
	StdCan_Set_class(irTxMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(irTxMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	irTxMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_IRTRANSCEIVE;
	irTxMsg.Header.ModuleId = sns_irTransceive_ID;
	irTxMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_IR;
	irTxMsg.Length = 6;

	for (uint8_t i = 0; i < sns_irTransceive_SUPPORTED_NUM_CHANNELS; i++)
	{
#if IR_RX_ENABLE==1
		irRxChannel[i].state = sns_irTransceive_STATE_RECEIVING;
		irRxChannel[i].newData = FALSE;
		irRxChannel[i].rxlen = 0;
		irRxChannel[i].proto.timeout=0; 
		irRxChannel[i].proto.data=0; 
		irRxChannel[i].proto.repeats=0; 
		irRxChannel[i].proto.protocol=0;
#endif
	}
	
#if IR_RX_ENABLE==1
	irRxChannel[0].timerNum=sns_irTransceive_RX0_REPEATE_TIMER;
	irRxChannel[1].timerNum=sns_irTransceive_RX1_REPEATE_TIMER;
	irRxChannel[2].timerNum=sns_irTransceive_RX2_REPEATE_TIMER;
#endif
	
	IrTransceiver_Init();

#if IR_RX_ENABLE==1
	irRxChannel[0].rxbuf = buf0;
	IrTransceiver_InitRxChannel(0, irRxChannel[0].rxbuf, sns_irTransceive_RX_done_callback, sns_irTransceive_RX0_PCINT, sns_irTransceive_RX0_PIN);
#endif

#if IR_TX_ENABLE==1
	IrTransceiver_InitTxChannel(0, sns_irTransceive_TX_done_callback, sns_irTransceive_TX0_PIN);
#endif

}

void sns_irTransceive_Process(void)
{
	uint8_t res;
	
	for (uint8_t channel=0; channel < sns_irTransceive_SUPPORTED_NUM_CHANNELS; channel++)
	{
//gpio_toggle_pin(EXP_A);

#if IR_RX_ENABLE==1
		switch (irRxChannel[channel].state)
		{
		case sns_irTransceive_STATE_IDLE:
			irRxChannel[channel].state = sns_irTransceive_STATE_START_RECEIVE;
			break;

		case sns_irTransceive_STATE_START_RECEIVE:
			IrTransceiver_ResetRx(channel);
			cli();
			irRxChannel[channel].newData = FALSE;
			sei();
			irRxChannel[channel].state = sns_irTransceive_STATE_RECEIVING;
			break;
		
		case sns_irTransceive_STATE_RECEIVING:
			if (irRxChannel[channel].newData == TRUE) {
				//TODO: move this line to the RX callback
				IrTransceiver_DisableRx(channel);
				cli();
				irRxChannel[channel].newData = FALSE;
				sei();

				/* Let protocol driver parse and then send on CAN */ 
				uint8_t res2 = parseProtocol(irRxChannel[channel].rxbuf, irRxChannel[channel].rxlen, &irRxChannel[channel].proto);
				if (res2 == IR_OK && irRxChannel[channel].proto.protocol != IR_PROTO_UNKNOWN) {
					irTxMsg.Data[0] = 0xf&CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_PRESSED;
					irTxMsg.Data[0] |= channel<<4;
					irTxMsg.Data[1] = irRxChannel[channel].proto.protocol;
					irTxMsg.Data[2] = (irRxChannel[channel].proto.data>>24)&0xff;
					irTxMsg.Data[3] = (irRxChannel[channel].proto.data>>16)&0xff;
					irTxMsg.Data[4] = (irRxChannel[channel].proto.data>>8)&0xff;
					irTxMsg.Data[5] = irRxChannel[channel].proto.data&0xff;

					StdCan_Put(&irTxMsg);
				} 
				else if (irRxChannel[channel].proto.protocol == IR_PROTO_UNKNOWN) 
				{
#if (sns_irTransceive_SEND_DEBUG==1)
					send_debug(irRxChannel[channel].rxbuf, irRxChannel[channel].rxlen);
					irRxChannel[channel].proto.timeout=300;
#endif
				}
				
				/* Enable the receiver again */
				IrTransceiver_EnableRx(channel);
				
				irRxChannel[channel].state = sns_irTransceive_STATE_START_PAUSE;
			}
			break;

		case sns_irTransceive_STATE_START_PAUSE:
			/* set a timer so we can send release button event when no new IR is arriving */
			Timer_SetTimeout(irRxChannel[channel].timerNum, irRxChannel[channel].proto.timeout, TimerTypeOneShot, 0);
			irRxChannel[channel].state = sns_irTransceive_STATE_PAUSING;
			break;

		case sns_irTransceive_STATE_PAUSING:
			/* reset timer if new IR arrived */
			if (irRxChannel[channel].newData == TRUE || IrTransceiver_GetStoreEnableRx(channel) == TRUE) {
				cli();
				irRxChannel[channel].newData = FALSE;
				sei();
				Timer_SetTimeout(irRxChannel[channel].timerNum, irRxChannel[channel].proto.timeout, TimerTypeOneShot, 0);
			}
		
			if (Timer_Expired(irRxChannel[channel].timerNum)) {
				irRxChannel[channel].state = sns_irTransceive_STATE_START_IDLE;
			}
			break;

		case sns_irTransceive_STATE_START_IDLE:
			if (irRxChannel[channel].proto.protocol != IR_PROTO_UNKNOWN) {
				/* Send button release command on CAN */
				irTxMsg.Data[0] = 0xf&CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_RELEASED;
				irTxMsg.Data[0] |= channel<<4;
				irTxMsg.Data[1] = irRxChannel[channel].proto.protocol;
				irTxMsg.Data[2] = (irRxChannel[channel].proto.data>>24)&0xff;
				irTxMsg.Data[3] = (irRxChannel[channel].proto.data>>16)&0xff;
				irTxMsg.Data[4] = (irRxChannel[channel].proto.data>>8)&0xff;
				irTxMsg.Data[5] = irRxChannel[channel].proto.data&0xff;

				StdCan_Put(&irTxMsg);
			}
			irRxChannel[channel].state = sns_irTransceive_STATE_IDLE;
			break;
		}
#endif


	}
}

void sns_irTransceive_HandleMessage(StdCan_Msg_t *rxMsg)
{
	/*if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS && 
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_IRTRANSCEIVE && 
		rxMsg->Header.ModuleId == sns_irTransceive_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}*/
}

void sns_irTransceive_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS); 
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_IRTRANSCEIVE; 

	txMsg.Header.ModuleId = sns_irTransceive_ID;
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
