
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

#if IR_TX_ENABLE==1
struct {
	uint8_t				state;
	uint8_t				sendComplete;
	uint16_t			*txbuf;
	uint8_t				txlen;
	uint8_t				timerNum;
	uint8_t				repeatCount;
	uint8_t				stopSend;
	Ir_Protocol_Data_t	proto;
} irTxChannel[sns_irTransceive_SUPPORTED_NUM_CHANNELS];
#endif

uint16_t	buf[2][MAX_NR_TIMES];

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
	if (channel < sns_irTransceive_SUPPORTED_NUM_CHANNELS)
	{
		irTxChannel[channel].sendComplete = TRUE;
	}
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

#if IR_TX_ENABLE==1
		irTxChannel[i].state = sns_irTransceive_STATE_IDLE;
		irTxChannel[i].sendComplete = FALSE;
		irTxChannel[i].repeatCount = 0;
		irTxChannel[i].txlen = 0;
		irTxChannel[i].proto.data=0; 
		irTxChannel[i].proto.repeats=0;
		irTxChannel[i].proto.framecnt=0; 
		irTxChannel[i].proto.protocol=0;
#endif
	}
	
#if IR_RX_ENABLE==1
	irRxChannel[0].timerNum=sns_irTransceive_RX0_REPEATE_TIMER;
	irRxChannel[1].timerNum=sns_irTransceive_RX1_REPEATE_TIMER;
	irRxChannel[2].timerNum=sns_irTransceive_RX2_REPEATE_TIMER;
#endif

#if IR_TX_ENABLE==1
	irTxChannel[0].timerNum=sns_irTransceive_TX0_REPEATE_TIMER;
	irTxChannel[1].timerNum=sns_irTransceive_TX1_REPEATE_TIMER;
	irTxChannel[2].timerNum=sns_irTransceive_TX2_REPEATE_TIMER;
#endif
	
	IrTransceiver_Init();
	gpio_set_out(sns_irTransceive_VCC_EN0_PIN);
	gpio_set_out(sns_irTransceive_VCC_EN1_PIN);
	gpio_set_out(sns_irTransceive_VCC_EN2_PIN);
	gpio_set_pin(sns_irTransceive_VCC_EN0_PIN);
	gpio_set_pin(sns_irTransceive_VCC_EN1_PIN);
	gpio_set_pin(sns_irTransceive_VCC_EN2_PIN);

#if IR_RX_ENABLE==1
	irRxChannel[0].rxbuf = buf[0];
	IrTransceiver_InitRxChannel(0, irRxChannel[0].rxbuf, sns_irTransceive_RX_done_callback, sns_irTransceive_RX0_PCINT, sns_irTransceive_RX0_PIN);
#endif

#if IR_TX_ENABLE==1
	gpio_set_out(sns_irTransceive_MOD_PIN);
	gpio_set_out(sns_irTransceive_TX0_PIN);
	gpio_set_out(sns_irTransceive_TX1_PIN);
	gpio_set_out(sns_irTransceive_TX2_PIN);
#if IR_TX_ACTIVE_LOW==1
	gpio_set_pin(sns_irTransceive_TX0_PIN);
	gpio_set_pin(sns_irTransceive_TX1_PIN);
	gpio_set_pin(sns_irTransceive_TX2_PIN);
#endif

	irTxChannel[0].txbuf = buf[1];
	IrTransceiver_InitTxChannel(0, sns_irTransceive_TX_done_callback, sns_irTransceive_TX0_PIN);
#endif

}

void sns_irTransceive_Process(void)
{
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

#if IR_TX_ENABLE==1
		switch (irTxChannel[channel].state)
		{
		case sns_irTransceive_STATE_IDLE:
			/* transmission is started when a command is received on can */
			break;

		case sns_irTransceive_STATE_START_TRANSMIT:
//		printf("1\n");
			if (expandProtocol(irTxChannel[channel].txbuf, &irTxChannel[channel].txlen, &irTxChannel[channel].proto) == IR_OK)
			{
				IrTransceiver_Transmit(channel, irTxChannel[channel].txbuf, irTxChannel[channel].txlen);
				irTxChannel[channel].state = sns_irTransceive_STATE_TRANSMITTING;
//		printf("2\n");
			}
			else
			{
				irTxChannel[channel].state = sns_irTransceive_STATE_IDLE;
			}
			break;
		
		case sns_irTransceive_STATE_TRANSMITTING:
			if (irTxChannel[channel].sendComplete == TRUE)
			{
				cli();
				irTxChannel[channel].sendComplete = FALSE;
				sei();
				irTxChannel[channel].state = sns_irTransceive_STATE_START_PAUSE;
//		printf("3\n");
			}
			break;

		case sns_irTransceive_STATE_START_PAUSE:
			if (irTxChannel[channel].repeatCount < irTxChannel[channel].proto.repeats)
			{
				irTxChannel[channel].repeatCount++;
			}
			
			Timer_SetTimeout(irTxChannel[channel].timerNum, irTxChannel[channel].proto.timeout, TimerTypeOneShot, 0);

			if (irTxChannel[channel].proto.framecnt != 255)
			{
				irTxChannel[channel].proto.framecnt++;
			}
			
			irTxChannel[channel].state = sns_irTransceive_STATE_PAUSING;
			break;

		case sns_irTransceive_STATE_PAUSING:
			if (Timer_Expired(irTxChannel[channel].timerNum))
			{
				irTxChannel[channel].state = sns_irTransceive_STATE_START_TRANSMIT;
//		printf("4\n");
			}
			
			/* transmission is stopped when such command is recevied on can */
			if (irTxChannel[channel].stopSend == TRUE && irTxChannel[channel].repeatCount >= irTxChannel[channel].proto.repeats)
			{
				//TODO maybe send message on can for status? to confirm stopped sending, ready for new command
				irTxChannel[channel].state = sns_irTransceive_STATE_START_IDLE;
//		printf("5\n");
			}
			break;

		case sns_irTransceive_STATE_START_IDLE:
			irTxChannel[channel].stopSend = FALSE;
			irTxChannel[channel].repeatCount = 0;
			irTxChannel[channel].proto.framecnt = 0;
			
			irTxChannel[channel].state = sns_irTransceive_STATE_IDLE;
//		printf("6\n");
			break;
		}
#endif

	}
}

void sns_irTransceive_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS && 
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_IRTRANSCEIVE && 
		rxMsg->Header.ModuleId == sns_irTransceive_ID)
	{
		uint8_t channel;
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_PHYSICAL_IR:
			channel = rxMsg->Data[0]>>4;
			if ((0xf&rxMsg->Data[0]) == CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_PRESSED && channel < sns_irTransceive_SUPPORTED_NUM_CHANNELS)
			{
				if (irTxChannel[channel].state == sns_irTransceive_STATE_IDLE)
				{
					irTxChannel[channel].proto.protocol = rxMsg->Data[1];

					irTxChannel[channel].proto.data = rxMsg->Data[2];
					irTxChannel[channel].proto.data = irTxChannel[channel].proto.data<<8;
					irTxChannel[channel].proto.data |= rxMsg->Data[3];
					irTxChannel[channel].proto.data = irTxChannel[channel].proto.data<<8;
					irTxChannel[channel].proto.data |= rxMsg->Data[4];
					irTxChannel[channel].proto.data = irTxChannel[channel].proto.data<<8;
					irTxChannel[channel].proto.data |= rxMsg->Data[5];

					irTxChannel[channel].state = sns_irTransceive_STATE_START_TRANSMIT;
				}
			}
			else if ((0xf&rxMsg->Data[0]) == CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_RELEASED && channel < sns_irTransceive_SUPPORTED_NUM_CHANNELS)
			{
				if (irTxChannel[channel].state != sns_irTransceive_STATE_IDLE)
				{
					irTxChannel[channel].stopSend = TRUE;
				}
			}
			break;
		}
	}
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
