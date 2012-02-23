
#include "sns_rfTransceive.h"

StdCan_Msg_t		rfTxMsg;

uint8_t rfRxChannel_newData;
uint8_t rfRxChannel_rxlen;
uint8_t rfRxChannel_state;
Ir_Protocol_Data_t	rfRxChannel_proto;
uint16_t	rfRxChannel_rxbuf[MAX_NR_TIMES];

uint8_t rfTxChannel_sendComplete;
uint8_t rfTxChannel_state;
uint8_t rfTxChannel_txlen;
Ir_Protocol_Data_t	rfTxChannel_proto;
uint16_t	rfTxChannel_rxbuf[MAX_NR_TIMES];
uint8_t rfTxChannel_repeatCount;


#if IR_RX_ENABLE==1
void sns_rfTransceive_RX_done_callback(uint8_t channel, uint16_t *buffer, uint8_t len)
{
	rfRxChannel_newData = TRUE;
	rfRxChannel_rxlen = len;
}
#endif


#if IR_TX_ENABLE==1
void sns_rfTransceive_TX_done_callback(uint8_t channel)
{
	rfTxChannel_sendComplete = TRUE;
}
#endif

void sns_rfTransceive_Init(void)
{
	StdCan_Set_class(rfTxMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(rfTxMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	rfTxMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_RFTRANSCEIVE;
	rfTxMsg.Header.ModuleId = sns_rfTransceive_ID;
	rfTxMsg.Length = 8;
	
	
	rfRxChannel_newData = FALSE;
	rfRxChannel_rxlen = 0;
	rfRxChannel_proto.timeout=0;
	rfRxChannel_proto.data=0;
	rfRxChannel_proto.repeats=0;
	rfRxChannel_proto.protocol=0;
	
	rfTxChannel_sendComplete = FALSE;
	rfTxChannel_txlen = 0;
	rfTxChannel_proto.data=0;
	rfTxChannel_proto.repeats=0;
	rfTxChannel_proto.framecnt=0;
	rfTxChannel_proto.protocol=0;
	rfTxChannel_repeatCount = 0;

	IrTransceiver_Init();
	/* TX-pin must be set in case transmitter is nexa */
	gpio_set_out(sns_rfTransceive_TX_PIN);
#if IR_TX_ACTIVE_LOW==1
	gpio_set_pin(sns_rfTransceive_TX_PIN);
#else
	gpio_clr_pin(sns_rfTransceive_TX_PIN);
#endif

	IrTransceiver_InitRxChannel(0, rfRxChannel_rxbuf, sns_rfTransceive_RX_done_callback, sns_rfTransceive_RX_PCINT, sns_rfTransceive_RX_PIN);
	rfRxChannel_state = sns_rfTransceive_STATE_RECEIVING;
	
	IrTransceiver_InitTxChannel(0, sns_rfTransceive_TX_done_callback, sns_rfTransceive_TX_PIN);
	rfTxChannel_state = sns_rfTransceive_STATE_IDLE;
}

void sns_rfTransceive_Process(void)
{
#if IR_RX_ENABLE==1
		switch (rfRxChannel_state)
		{
		case sns_rfTransceive_STATE_IDLE:
		{
			if (rfTxChannel_proto.protocol != IR_PROTO_UNKNOWN) {
				/* Send button release command on CAN */
				rfTxMsg.Header.Command = CAN_MODULE_CMD_RFTRANSCEIVE_DATASTOP;
				rfTxMsg.Data[0] = 0;
				rfTxMsg.Data[1] = rfRxChannel_proto.protocol;
				rfTxMsg.Data[2] = (rfRxChannel_proto.data>>40)&0xff;
				rfTxMsg.Data[3] = (rfRxChannel_proto.data>>32)&0xff;
				rfTxMsg.Data[4] = (rfRxChannel_proto.data>>24)&0xff;
				rfTxMsg.Data[5] = (rfRxChannel_proto.data>>16)&0xff;
				rfTxMsg.Data[6] = (rfRxChannel_proto.data>>8)&0xff;
				rfTxMsg.Data[7] = rfRxChannel_proto.data&0xff;

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
				uint8_t res2 = parseProtocol(rfRxChannel_rxbuf, rfRxChannel_rxlen, &rfRxChannel_proto);
				if (res2 == IR_OK && rfRxChannel_proto.protocol != IR_PROTO_UNKNOWN) {
					rfTxMsg.Header.Command = CAN_MODULE_CMD_RFTRANSCEIVE_DATASTART;
					rfTxMsg.Data[0] = 0;
					rfTxMsg.Data[1] = rfRxChannel_proto.protocol;
					rfTxMsg.Data[2] = (rfRxChannel_proto.data>>40)&0xff;
					rfTxMsg.Data[3] = (rfRxChannel_proto.data>>32)&0xff;
					rfTxMsg.Data[4] = (rfRxChannel_proto.data>>24)&0xff;
					rfTxMsg.Data[5] = (rfRxChannel_proto.data>>16)&0xff;
					rfTxMsg.Data[6] = (rfRxChannel_proto.data>>8)&0xff;
					rfTxMsg.Data[7] = rfRxChannel_proto.data&0xff;

					StdCan_Put(&rfTxMsg);

					rfRxChannel_state = sns_rfTransceive_STATE_START_PAUSE;
				}
				else if (rfRxChannel_proto.protocol == IR_PROTO_UNKNOWN)
				{
#if (sns_rfTransceive_SEND_DEBUG==1)
					//send_debug(rfRxChannel_rxbuf, rfRxChannel_rxlen);
					//rfRxChannel_proto.timeout=300;
#endif
					rfRxChannel_state = sns_rfTransceive_STATE_START_RECEIVE;
				}
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
				IrTransceiver_ResetRx(0); //TODO??
				rfRxChannel_newData = FALSE;
				sei();

				Ir_Protocol_Data_t	protoDummy;
				if (parseProtocol(rfRxChannel_rxbuf, rfRxChannel_rxlen, &protoDummy) == IR_OK) {
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

#if 0
//#if IR_TX_ENABLE==1
		switch (irTxChannel[channel].state)
		{
		case sns_irTransceive_STATE_IDLE:
		{
			/* transmission is started when a command is received on can */
			irTxChannel[channel].stopSend = FALSE;
			irTxChannel[channel].repeatCount = 0;
			irTxChannel[channel].proto.framecnt = 0;
			irTxChannel[channel].sendingPronto = FALSE;
			break;
		}

		case sns_irTransceive_STATE_START_TRANSMIT:
		{
			/* Expand protocol. */
			if (expandProtocol(irTxChannel[channel].txbuf, &irTxChannel[channel].txlen, &irTxChannel[channel].proto) != IR_OK) {
				/* Failed to expand protocol -> enter idle state. */
				irTxChannel[channel].state = sns_irTransceive_STATE_IDLE;
				break;
			}

			/* Start IR transmission. */
			/* TODO Send respone. Function call may fail when different modulation frequencies are used simultaneously. */
			IrTransceiver_Transmit(channel, irTxChannel[channel].txbuf, 0, irTxChannel[channel].txlen, irTxChannel[channel].proto.modfreq);

			/* Enter transmitting state. */
			irTxChannel[channel].state = sns_irTransceive_STATE_TRANSMITTING;
			break;
		}

		case sns_irTransceive_STATE_START_TRANSMIT_PRONTO:
		{
			/* Start IR transmission. */
			if(IrTransceiver_Transmit(channel, irTxChannel[channel].txbuf, 0, irTxChannel[channel].onceSeqLen - 1, irTxChannel[channel].proto.modfreq) < 0) {
				/* Send error code. */
				pronto_sendResponse(channel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ABORTED);

				/* Enter idle state. */
				irTxChannel[channel].state = sns_irTransceive_STATE_IDLE;
				break;
			}

			/* Enter transmitting state. */
			irTxChannel[channel].sendingPronto = TRUE;
			irTxChannel[channel].state = sns_irTransceive_STATE_TRANSMITTING;
			break;
		}

		case sns_irTransceive_STATE_PRONTO_REPEAT:
		{
			/* Check if done. */
			if ((irTxChannel[channel].repeatCount >= irTxChannel[channel].proto.repeats && irTxChannel[channel].proto.repeats != 0xFF) || irTxChannel[channel].stopSend) {
				/* Pronto transmission was stopped/completed */
				pronto_sendResponse(channel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_STOPPED);
				irTxChannel[channel].state = sns_irTransceive_STATE_IDLE;
				break;
			}

			if (irTxChannel[channel].repeatCount < 255) {
				irTxChannel[channel].repeatCount++;
			}

			/* Repeat sequence exists? */
			if (irTxChannel[channel].repSeqLen != 0) {
				/* Use repeat seq */
				uint16_t offset = ((uint16_t)irTxChannel[channel].onceSeqLen);
				/* Don't transmit last passive. last passive handled by timer */
				IrTransceiver_Transmit(channel, irTxChannel[channel].txbuf, offset, ((uint16_t)irTxChannel[channel].repSeqLen) - 1, irTxChannel[channel].proto.modfreq);
			}
			else {
				/* Use once seq */
				/* Don't transmit last passive. last passive handled by timer */
				IrTransceiver_Transmit(channel, irTxChannel[channel].txbuf, 0, irTxChannel[channel].txlen - 1, irTxChannel[channel].proto.modfreq);
			}
			irTxChannel[channel].state = sns_irTransceive_STATE_TRANSMITTING;
		}

		case sns_irTransceive_STATE_TRANSMITTING:
		{
			if (irTxChannel[channel].sendComplete == TRUE)
			{
				cli();
				irTxChannel[channel].sendComplete = FALSE;
				sei();

				if (irTxChannel[channel].sendingPronto)
				{
					/* First repeat, or no repeat sequence defined => use last passive time in once seq */
					if(irTxChannel[channel].repeatCount == 0 || irTxChannel[channel].repSeqLen == 0)
					{
						/* Use last passive time as timeout */
						uint16_t lastPasTime = irTxChannel[channel].txbuf[irTxChannel[channel].onceSeqLen - 1] / 1000;
						Timer_SetTimeout(irTxChannel[channel].timerNum, lastPasTime==0 ? 1 : lastPasTime, TimerTypeOneShot, 0);
					}
					/* Second, or later repeat => use last passive time in repeat seq */
					else
					{
						/* Use last passive time as timeout */
						uint16_t lastPasTime = irTxChannel[channel].txbuf[irTxChannel[channel].txlen - 1]  / 1000;
						Timer_SetTimeout(irTxChannel[channel].timerNum, lastPasTime==0 ? 1 : lastPasTime, TimerTypeOneShot, 0);
					}
					irTxChannel[channel].state = sns_irTransceive_STATE_PAUSING;
				}
				else
				{
					irTxChannel[channel].state = sns_irTransceive_STATE_START_PAUSE;
				}
			}
			break;
		}

		case sns_irTransceive_STATE_START_PAUSE:
		{
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
		}

		case sns_irTransceive_STATE_PAUSING:
		{
			if (irTxChannel[channel].sendingPronto)
			{
				if (Timer_Expired(irTxChannel[channel].timerNum))
				{
					irTxChannel[channel].state = sns_irTransceive_STATE_PRONTO_REPEAT;
				}
				break;
			}

			if (Timer_Expired(irTxChannel[channel].timerNum))
			{
				irTxChannel[channel].state = sns_irTransceive_STATE_START_TRANSMIT;
			}

			/* Transmission is stopped when such command is recevied on can */
			if (irTxChannel[channel].stopSend == TRUE && irTxChannel[channel].repeatCount >= irTxChannel[channel].proto.repeats)
			{
				/* Non-pronto transmission was stopped/completed */
				pronto_sendResponse(channel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_STOPPED);

				irTxChannel[channel].state = sns_irTransceive_STATE_IDLE;
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
		case CAN_MODULE_CMD_RFTRANSCEIVE_DATASTART:
		{
		}
		break;
		case CAN_MODULE_CMD_RFTRANSCEIVE_DATASTOP:
		{
		}
		break;
		case CAN_MODULE_CMD_RFTRANSCEIVE_DATABURST:
		{
		}
		break;
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

	txMsg.Data[0] = NODE_HW_ID_BYTE0;
	txMsg.Data[1] = NODE_HW_ID_BYTE1;
	txMsg.Data[2] = NODE_HW_ID_BYTE2;
	txMsg.Data[3] = NODE_HW_ID_BYTE3;

	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;

	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}
