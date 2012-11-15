
#include "sns_rfTransceive.h"

#if IR_RX_ENABLE==1
StdCan_Msg_t		rfTxMsg;
uint8_t rfRxChannel_newData;
uint8_t rfRxChannel_len;
uint8_t rfRxChannel_index;
uint8_t rfRxChannel_state;
Ir_Protocol_Data_t	rfRxChannel_proto;
uint16_t	rfRxChannel_buf[MAX_NR_TIMES];

void sns_rfTransceive_RX_done_callback(uint8_t channel, uint16_t *buffer, uint8_t len, uint8_t index)
{
#if IR_RX_CONTINUOUS_MODE==0
	rfRxChannel_newData = TRUE;
	rfRxChannel_len = len;
#else
	if (len > sns_rfTransceive_MIN_NUM_PULSES)
	{
		rfRxChannel_newData = TRUE;
		rfRxChannel_len = len;
		rfRxChannel_index = index;
//gpio_set_pin(EXP_B);
	}
#endif
}
#endif


#if IR_TX_ENABLE==1
uint8_t rfTxChannel_sendComplete;
uint8_t rfTxChannel_state;
uint8_t rfTxChannel_len;
Ir_Protocol_Data_t	rfTxChannel_proto;
uint16_t	rfTxChannel_buf[MAX_NR_TIMES];
uint8_t rfTxChannel_repeatCount;
uint8_t rfTxChannel_stopSend;

void sns_rfTransceive_TX_done_callback(uint8_t channel)
{
	rfTxChannel_sendComplete = TRUE;
}
#endif

void sns_rfTransceive_Init(void)
{
#if IR_RX_ENABLE==1
	StdCan_Set_class(rfTxMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(rfTxMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	rfTxMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_RFTRANSCEIVE;
	rfTxMsg.Header.ModuleId = sns_rfTransceive_ID;
	rfTxMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_IR;
	rfTxMsg.Length = 8;
	
	rfRxChannel_newData = FALSE;
	rfRxChannel_len = 0;
	rfRxChannel_proto.timeout=0;
	rfRxChannel_proto.data=0;
	rfRxChannel_proto.repeats=0;
	rfRxChannel_proto.protocol=0;
#endif

#if IR_TX_ENABLE==1
	rfTxChannel_sendComplete = FALSE;
	rfTxChannel_len = 0;
	rfTxChannel_proto.data=0;
	rfTxChannel_proto.repeats=0;
	rfTxChannel_proto.framecnt=0;
	rfTxChannel_proto.protocol=0;
	rfTxChannel_repeatCount = 0;
#endif

	IrTransceiver_Init();
	/* TX-pin must be set in case transmitter is nexa */
	gpio_set_out(sns_rfTransceive_TX_PIN);
#if IR_TX_ACTIVE_LOW==1
	gpio_set_pin(sns_rfTransceive_TX_PIN);
#else
	gpio_clr_pin(sns_rfTransceive_TX_PIN);
#endif

#if IR_RX_ENABLE==1
	IrTransceiver_InitRxChannel(0, rfRxChannel_buf, sns_rfTransceive_RX_done_callback, sns_rfTransceive_RX_PCINT, sns_rfTransceive_RX_PIN);
	rfRxChannel_state = sns_rfTransceive_STATE_RECEIVING;
#endif
	
#if IR_TX_ENABLE==1
	IrTransceiver_InitTxChannel(0, sns_rfTransceive_TX_done_callback, sns_rfTransceive_TX_PIN);
	rfTxChannel_state = sns_rfTransceive_STATE_IDLE;
#endif
}


///////////// DEBUG!!!!
#if (sns_rfTransceive_SEND_DEBUG==1)
StdCan_Msg_t irTxMsg;
void send_debug(uint16_t *buffer, uint8_t startindex, uint8_t endindex) {

	/* the protocol is unknown so the raw ir-data is sent, makes it easier to develop a new protocol */

	StdCan_Set_class(irTxMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(irTxMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	irTxMsg.Length = 8;
	irTxMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_RFTRANSCEIVE;
	irTxMsg.Header.ModuleId = 0;
	irTxMsg.Header.Command = CAN_MODULE_CMD_IRRECEIVE_IRRAW;
	while (StdCan_Put(&irTxMsg) != StdCan_Ret_OK) {}
	  _delay_ms(1);
	//_------------------
	uint8_t i= startindex;
	while(i != endindex) {
	  irTxMsg.Length = 2;
	  irTxMsg.Data[0] = (buffer[i]>>8)&0xff;
	  irTxMsg.Data[1] = (buffer[i]>>0)&0xff;
	  i++;
	  if (i == MAX_NR_TIMES) {
	    i=0;
	  }
	  if(i != endindex) {
	    irTxMsg.Length = 4;
	    irTxMsg.Data[2] = (buffer[i]>>8)&0xff;
	    irTxMsg.Data[3] = (buffer[i]>>0)&0xff;
	    i++;
	    if (i == MAX_NR_TIMES) {
	      i=0;
	    }
	    if(i != endindex) {
	      irTxMsg.Length = 6;
	      irTxMsg.Data[4] = (buffer[i]>>8)&0xff;
	      irTxMsg.Data[5] = (buffer[i]>>0)&0xff;
	      i++;
	      if (i == MAX_NR_TIMES) {
		i=0;
	      }
	      if(i != endindex) {
		irTxMsg.Length = 8;
		irTxMsg.Data[6] = (buffer[i]>>8)&0xff;
		irTxMsg.Data[7] = (buffer[i]>>0)&0xff;
		i++;
		if (i == MAX_NR_TIMES) {
		  i=0;
		}
	      }
	    }
	  }
	  /* buffers will be filled when sending more than 2-3 messages, so retry until sent */
	  while (StdCan_Put(&irTxMsg) != StdCan_Ret_OK) {}
	  _delay_ms(1);
	}
}
#endif

void sns_rfTransceive_Process(void)
{
#if IR_RX_ENABLE==1
		switch (rfRxChannel_state)
		{
		case sns_rfTransceive_STATE_IDLE:
		{
			/* If known protocol and timeout is not 0 (0 means burst) */
			if (rfRxChannel_proto.protocol != IR_PROTO_UNKNOWN && rfRxChannel_proto.timeout != 0) {
				/* Send button release command on CAN */
				rfTxMsg.Data[0] = CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_RELEASED;
				rfTxMsg.Data[1] = rfRxChannel_proto.protocol;
				/* Data content is kept from last transmit (pressed) */

				StdCan_Put(&rfTxMsg);
			}
			rfRxChannel_state = sns_rfTransceive_STATE_START_RECEIVE;
			break;
		}

		case sns_rfTransceive_STATE_START_RECEIVE:
			cli();
			rfRxChannel_newData = FALSE;
			sei();
			rfRxChannel_state = sns_rfTransceive_STATE_RECEIVING;
			
			break;

		case sns_rfTransceive_STATE_RECEIVING:
			if (rfRxChannel_newData == TRUE) {
				cli();
				rfRxChannel_newData = FALSE;
				sei();
				/* Let protocol driver parse and then send on CAN */
				uint8_t res2 = parseProtocol(rfRxChannel_buf, rfRxChannel_len, rfRxChannel_index, &rfRxChannel_proto);
				if (res2 == IR_OK && rfRxChannel_proto.protocol != IR_PROTO_UNKNOWN) 
				{
//gpio_clr_pin(EXP_B);
					//send_debug(rfRxChannel_buf, rfRxChannel_len);
					/* If timeout is 0, protocol is burst protocol */
					if (rfRxChannel_proto.timeout > 0)
					{
						rfTxMsg.Data[0] = CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_PRESSED;
						rfRxChannel_state = sns_rfTransceive_STATE_START_PAUSE;
					}
					else
					{
						rfTxMsg.Data[0] = CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_BURST;
						rfRxChannel_state = sns_rfTransceive_STATE_START_RECEIVE;
					}
					rfTxMsg.Data[1] = rfRxChannel_proto.protocol;
					rfTxMsg.Data[2] = (rfRxChannel_proto.data>>40)&0xff;
					rfTxMsg.Data[3] = (rfRxChannel_proto.data>>32)&0xff;
					rfTxMsg.Data[4] = (rfRxChannel_proto.data>>24)&0xff;
					rfTxMsg.Data[5] = (rfRxChannel_proto.data>>16)&0xff;
					rfTxMsg.Data[6] = (rfRxChannel_proto.data>>8)&0xff;
					rfTxMsg.Data[7] = rfRxChannel_proto.data&0xff;

					StdCan_Put(&rfTxMsg);
				}
				else if (rfRxChannel_proto.protocol == IR_PROTO_UNKNOWN)
				{
#if (sns_rfTransceive_SEND_DEBUG==1)
					//send_debug(rfRxChannel_buf, rfRxChannel_len);
					//rfRxChannel_proto.timeout=300;
#endif
					rfRxChannel_state = sns_rfTransceive_STATE_START_RECEIVE;
				}
#if (sns_rfTransceive_SEND_DEBUG==1)
				else if (res2 == IR_SEND_DEBUG) 
				{
					send_debug(rfRxChannel_buf, rfRxChannel_proto.data & 0xFFu, rfRxChannel_index);
					rfRxChannel_state = sns_rfTransceive_STATE_START_RECEIVE;
				}
#endif
			}
			break;

		case sns_rfTransceive_STATE_START_PAUSE:
			/* set a timer so we can send release button event when no new RF is arriving */
			Timer_SetTimeout(sns_rfTransceive_RX_REPEATE_TIMER, rfRxChannel_proto.timeout, TimerTypeOneShot, 0);
			rfRxChannel_state = sns_rfTransceive_STATE_PAUSING;
			break;

		case sns_rfTransceive_STATE_PAUSING:
			/* reset timer if new IR arrived */
			if (rfRxChannel_newData == TRUE) {
				cli();
				rfRxChannel_newData = FALSE;
				sei();

				Ir_Protocol_Data_t	protoDummy;
				if (parseProtocol(rfRxChannel_buf, rfRxChannel_len, rfRxChannel_index, &protoDummy) == IR_OK) {
					if (protoDummy.protocol == rfRxChannel_proto.protocol) {
						/* re-set timer so we can send release button event when no new RF is arriving */
						Timer_SetTimeout(sns_rfTransceive_RX_REPEATE_TIMER, rfRxChannel_proto.timeout, TimerTypeOneShot, 0);
					}
				}
			}

			if (Timer_Expired(sns_rfTransceive_RX_REPEATE_TIMER)) {
				rfRxChannel_state = sns_rfTransceive_STATE_IDLE;
			}
			break;

		default:
			break;
		}
#endif

#if IR_TX_ENABLE==1
		switch (rfTxChannel_state)
		{
		case sns_rfTransceive_STATE_IDLE:
		{
			/* transmission is started when a command is received on can */
			rfTxChannel_stopSend = FALSE;
			rfTxChannel_repeatCount = 0;
			rfTxChannel_proto.framecnt = 0;
			break;
		}

		case sns_rfTransceive_STATE_START_TRANSMIT:
		{
			/* Expand protocol. */
			if (expandProtocol(rfTxChannel_buf, &rfTxChannel_len, &rfTxChannel_proto) != IR_OK) {
				/* Failed to expand protocol -> enter idle state. */
				rfTxChannel_state = sns_rfTransceive_STATE_IDLE;
				break;
			}

			/* Start RF transmission. */
			IrTransceiver_Transmit(0, rfTxChannel_buf, 0, rfTxChannel_len, rfTxChannel_proto.modfreq);

			/* Enter transmitting state. */
			rfTxChannel_state = sns_rfTransceive_STATE_TRANSMITTING;
			break;
		}

		case sns_rfTransceive_STATE_TRANSMITTING:
		{
			if (rfTxChannel_sendComplete == TRUE)
			{
				cli();
				rfTxChannel_sendComplete = FALSE;
				sei();

				rfTxChannel_state = sns_rfTransceive_STATE_START_PAUSE;
			}
			break;
		}

		case sns_rfTransceive_STATE_START_PAUSE:
		{
			if (rfTxChannel_repeatCount < rfTxChannel_proto.repeats)
			{
				rfTxChannel_repeatCount++;
			}

			Timer_SetTimeout(sns_rfTransceive_TX_REPEATE_TIMER, rfTxChannel_proto.timeout, TimerTypeOneShot, 0);

			if (rfTxChannel_proto.framecnt != 255)
			{
				rfTxChannel_proto.framecnt++;
			}

			rfTxChannel_state = sns_rfTransceive_STATE_PAUSING;
			break;
		}

		case sns_rfTransceive_STATE_PAUSING:
		{
			if (Timer_Expired(sns_rfTransceive_TX_REPEATE_TIMER))
			{
				rfTxChannel_state = sns_rfTransceive_STATE_START_TRANSMIT;
			}

			/* Transmission is stopped when such command is recevied on can */
			if (rfTxChannel_stopSend == TRUE && rfTxChannel_repeatCount >= rfTxChannel_proto.repeats)
			{
				rfTxChannel_state = sns_rfTransceive_STATE_IDLE;
			}
			break;
		}
		default:
			break;
		}
#endif
}

void sns_rfTransceive_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS && 
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_RFTRANSCEIVE &&
		rxMsg->Header.ModuleId == sns_rfTransceive_ID)
	{
		switch (rxMsg->Header.Command)
		{
#if IR_TX_ENABLE==1
			case CAN_MODULE_CMD_PHYSICAL_IR:
			{
				if (rfTxChannel_state == sns_rfTransceive_STATE_IDLE && 
					(rxMsg->Data[0] == CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_PRESSED ||
					rxMsg->Data[0] == CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_BURST))
				{
					rfTxChannel_stopSend = (uint8_t)(rxMsg->Data[0] == CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_BURST);

					rfTxChannel_proto.protocol = rxMsg->Data[1];
					rfTxChannel_proto.data = rxMsg->Data[2];
					rfTxChannel_proto.data = rfTxChannel_proto.data<<8;
					rfTxChannel_proto.data |= rxMsg->Data[3];
					rfTxChannel_proto.data = rfTxChannel_proto.data<<8;
					rfTxChannel_proto.data |= rxMsg->Data[4];
					rfTxChannel_proto.data = rfTxChannel_proto.data<<8;
					rfTxChannel_proto.data |= rxMsg->Data[5];
					rfTxChannel_proto.data = rfTxChannel_proto.data<<8;
					rfTxChannel_proto.data |= rxMsg->Data[6];
					rfTxChannel_proto.data = rfTxChannel_proto.data<<8;
					rfTxChannel_proto.data |= rxMsg->Data[7];

					rfTxChannel_state = sns_rfTransceive_STATE_START_TRANSMIT;
				}
				else if (rfTxChannel_state != sns_rfTransceive_STATE_IDLE && rxMsg->Data[0] == CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_RELEASED)
				{
					rfTxChannel_stopSend = TRUE;
				}
			}
#endif
		}
	}
}

void sns_rfTransceive_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_RFTRANSCEIVE;
	txMsg.Header.ModuleId = sns_rfTransceive_ID;
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
