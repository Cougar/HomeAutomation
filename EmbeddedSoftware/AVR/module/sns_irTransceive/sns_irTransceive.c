
#include "sns_irTransceive.h"

#ifdef sns_irTransceive_USEEEPROM
#include "sns_irTransceive_eeprom.h"
struct eeprom_sns_irTransceive EEMEM eeprom_sns_irTransceive =
{
	{
		/* Define initialization values on the EEPROM variables here.
		This will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips.
		Write the values in the exact same order as the struct is defined in the *.h file. */
		CAN_MODULE_ENUM_IRTRANSCEIVE_IRCONFIG_DIRECTION_AUTO,
		0,
		CAN_MODULE_ENUM_IRTRANSCEIVE_IRCONFIG_DIRECTION_AUTO,
		0,
		CAN_MODULE_ENUM_IRTRANSCEIVE_IRCONFIG_DIRECTION_AUTO,
		0,
	},
	0	/* crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts */
};
#endif

#if IR_RX_ENABLE==1
struct {
	uint8_t				state;
	uint16_t			modfreq;
	uint16_t			*rxbuf;
	uint8_t				rxlen;
	uint8_t				timerNum;
	Ir_Protocol_Data_t	proto;
	uint8_t				newData;
} irRxChannel[IR_SUPPORTED_NUM_CHANNELS];
#endif

#if IR_TX_ENABLE==1
struct {
	uint8_t				state;
	uint16_t			modfreq;
	uint16_t			*txbuf;
	uint8_t				txlen;
	uint8_t				timerNum;
	uint8_t				repeatCount;
	Ir_Protocol_Data_t	proto;
	uint8_t				stopSend;
	uint8_t				sendComplete;
	/* Pronto params */
	uint8_t				sendingPronto;
	uint8_t				onceSeqLen;
	uint8_t				repSeqLen;
	uint8_t				expectedSeqNr;
} irTxChannel[IR_SUPPORTED_NUM_CHANNELS];
#endif

uint16_t	buf[IR_SUPPORTED_NUM_CHANNELS][MAX_NR_TIMES];

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

#define sns_irTransceive_BaseFrq (4145146UL)

#ifndef sns_irTransceive_PRONTO_SUPPORT
#define sns_irTransceive_PRONTO_SUPPORT 0
#endif

/*
 * PRONTO HEX Routines
 */
static void pronto_sendResponse(uint8_t channel, uint8_t responseValue)
{
	StdCan_Msg_t msg;
	StdCan_Set_class(msg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(msg.Header, DIRECTIONFLAG_FROM_OWNER);
	msg.Header.ModuleType = CAN_MODULE_TYPE_SNS_IRTRANSCEIVE;
	msg.Header.ModuleId = sns_irTransceive_ID;
	/* Send response command */
	msg.Length = 3;
	msg.Header.Command = CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTORESPONSE;
	msg.Data[0] = ((channel<<4)|0xf);
	msg.Data[1] = 0xF0;
	msg.Data[2] = (uint8_t)responseValue;
	while (StdCan_Put(&msg) != StdCan_Ret_OK) {}
	_delay_ms(1);
}

#if sns_irTransceive_PRONTO_SUPPORT==1
volatile uint32_t sns_irTransceive_LastPronto=0;
static uint8_t activeChannel = 0;
#define sns_irTransceive_MAXTIMING (16*1000)

static void pronto_sendData(uint16_t *buffer, uint8_t len, uint8_t channel, uint16_t modfreq)
{
	uint32_t currentTime = Timer_GetTicks();

	StdCan_Msg_t msg;

	StdCan_Set_class(msg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(msg.Header, DIRECTIONFLAG_FROM_OWNER);
	msg.Header.ModuleType = CAN_MODULE_TYPE_SNS_IRTRANSCEIVE;
	msg.Header.ModuleId = sns_irTransceive_ID;

	/* Send timing command */
	msg.Length = 5;
	msg.Header.Command = CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOTIMING;

	if ((currentTime < sns_irTransceive_LastPronto+sns_irTransceive_MAXTIMING) && (sns_irTransceive_LastPronto != 0))
	{
		/* Calculate elapsed time since last pronto packet */
		uint32_t elapsedTime = ((currentTime - sns_irTransceive_LastPronto) * 1000) & 0x00FFFFFFUL; /* 24bit value */

		/* Remove length of the last received sequence */
		uint32_t sequenceLength = IR_MAX_PULSE_WIDTH; /* timeout value not included in buffer */
		for (uint8_t i = 0; i < len; i++) {
			sequenceLength += (uint32_t)buffer[i];
		}
		if (sequenceLength <= elapsedTime) {
			elapsedTime -= sequenceLength;
		}

		msg.Data[0] = ((channel<<4)|0xf);
		msg.Data[1] = 0x10;
		msg.Data[2] = (uint8_t)(elapsedTime >> 16);
		msg.Data[3] = (uint8_t)(elapsedTime >> 8);
		msg.Data[4] = (uint8_t)(elapsedTime >> 0);
		while (StdCan_Put(&msg) != StdCan_Ret_OK) {}
		_delay_ms(1);
	}
	sns_irTransceive_LastPronto = currentTime;

	/* Calculate frequency divider */
	uint16_t divider = (uint16_t)(sns_irTransceive_BaseFrq / (1000*(uint32_t)modfreq));

	/* Send pronto start */
	msg.Length = 8;
	msg.Header.Command = CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOSTART;

	msg.Data[0] = ((channel<<4)|0x0);	/* Channel and Prontoformat 0x0000 */
	msg.Data[1] = 0x00;
	msg.Data[2] = (divider & 0xFF00) >> 8;	/* Freq divider */
	msg.Data[3] = (divider & 0x00FF) >> 0;
	msg.Data[4] = 0x00;	/* Once seq length, first byte always 0 */
	msg.Data[5] = (len + 1) / 2;
	msg.Data[6] = 0x00;	/* Repeat seq length, always 0 for ir receive */
	msg.Data[7] = 0x00;

	while (StdCan_Put(&msg) != StdCan_Ret_OK) {}
	_delay_ms(1);

	/* Send pronto data */
	//uint16_t divider = ((1000000ULL*modfreq)/sns_irTransceive_BaseFrq);
	msg.Header.Command = CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA1;
	uint8_t seqNr = 0;
	/* Counter for buffer */
	uint8_t i = 0;
	/* Counter for filling can frame data */
	uint8_t j = 0;
	/* Remember byte if data does not fit 8 bit */
	uint8_t mem = 0;
	while (i < len)
	{
		if (mem > 0)
		{
			/* If byte was memorized then buffer it */
			msg.Data[j] = mem;
			/* Clear memory */
			mem = 0;
		}
		else
		{
			/* Convert µs to pronto hex modulation pulses */
			uint16_t data = (uint16_t)(((uint32_t)buffer[i] * (uint32_t)modfreq) / 1000);

			/* Protocol compression */
			if (data >= 256)
			{
				/* If data does not fit into 8 bit, then split, memorize low byte */
				mem = data&0xff;
				/* Buffer high byte */
				msg.Data[j] = (data>>8)&0xff;
				j++;
				if (j==8)
				{
					/* If send buffer is full, send frame */
					/* can buffers will be filled when sending more than 2-3 messages, so retry until sent */
					while (StdCan_Put(&msg) != StdCan_Ret_OK) {}
					_delay_ms(1);
					j=0;
					seqNr = (seqNr + 1) % 16;
					msg.Header.Command = CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA1 + seqNr;
				}
				/* Set complementary burst pair to 0 to indicate 16bit transmission */
				msg.Data[j] = 0;
			}
			else
			{
				msg.Data[j] = data&0xff;
			}

			i++;
		}

		j++;
		if (j==8)
		{
			/* If send buffer is full, send frame */
			/* can buffers will be filled when sending more than 2-3 messages, so retry until sent */
			while (StdCan_Put(&msg) != StdCan_Ret_OK) {}
			_delay_ms(1);
			j=0;
			seqNr = (seqNr + 1) % 16;
			msg.Header.Command = CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA1 + seqNr;
		}
	}

	/* End with max pulse TODO fix uglyness */
	msg.Data[j] = ((IR_MAX_PULSE_WIDTH/((1000000ULL*109)/sns_irTransceive_BaseFrq))>>8) & 0xFF;
	j++;
	if (j==8)
	{
		/* If send buffer is full, send frame */
		/* can buffers will be filled when sending more than 2-3 messages, so retry until sent */
		while (StdCan_Put(&msg) != StdCan_Ret_OK) {}
		_delay_ms(1);
		j=0;
		seqNr = (seqNr + 1) % 16;
		msg.Header.Command = CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA1 + seqNr;
	}
	msg.Data[j] = 0;
	j++;
	msg.Data[j] = (IR_MAX_PULSE_WIDTH/((1000000ULL*109)/sns_irTransceive_BaseFrq))&0xff;
	j++;
	if (j==8)
	{
		/* If send buffer is full, send frame */
		/* can buffers will be filled when sending more than 2-3 messages, so retry until sent */
		while (StdCan_Put(&msg) != StdCan_Ret_OK) {}
		_delay_ms(1);
		j=0;
		seqNr = (seqNr + 1) % 16;
		msg.Header.Command = CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA1 + seqNr;
	}

	/* Msg command kept from for loop, but increase 16 to send ProntoEnd */
	msg.Header.Command = CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND1 + seqNr;
	msg.Length = j;
	/* data kept from loop, if any */
	/* buffers will be filled when sending more than 2-3 messages, so retry until sent */
	while (StdCan_Put(&msg) != StdCan_Ret_OK) {}
	_delay_ms(1);
}


static int decodeProntoData(int channel, uint8_t data)
{
	static uint8_t waitingForLSB = FALSE;
	static uint8_t previousValueWas16bit = FALSE;

	if (irTxChannel[channel].txlen >= MAX_NR_TIMES) {
		/* Max length exceeded */
		return -1;
	}

	if (irTxChannel[channel].txlen == 0) {
		waitingForLSB = FALSE;
		if (data == 0) {
			/* Cannot start with 0x00 */
			return -2;
		}
	}

	/* 0x00 means that previously received time is MSB of 16bit value, and that next byte will be LSB */
	if (data == 0 && !waitingForLSB && previousValueWas16bit==FALSE) {
		/* Convert previous time to MSB of the 16bit value. next received byte will become LSB */
		irTxChannel[channel].txbuf[irTxChannel[channel].txlen - 1] <<= 8;

		/* Flag that next received byte is LSB rather than a new timing value */
		waitingForLSB = TRUE;
	}

	/* Non-zero value! is this part of a 16bit-value? use the data as LSB for previously received time */
	else if (waitingForLSB) {
		/* Let this byte be LSB of previous 16bit value */
		irTxChannel[channel].txbuf[irTxChannel[channel].txlen - 1] |= data;

		/* We've now completed reception of the 16bit value */
		waitingForLSB = FALSE;

		previousValueWas16bit = TRUE;
	}

	/* Non-zero value! simply new 8bit value */
	else {
		/* Check if we're already done */
		if (irTxChannel[channel].txlen >= (((uint16_t)irTxChannel[channel].onceSeqLen + (uint16_t)irTxChannel[channel].repSeqLen))) {
			/* Too much data received, not necessarily a problem in case frames are padded */
			return 2;
		}

		irTxChannel[channel].txbuf[irTxChannel[channel].txlen] = data;

		previousValueWas16bit = FALSE;

		irTxChannel[channel].txlen++;
	}

	return 1;
}

#endif


#if IR_RX_ENABLE==1
void sns_irTransceive_RX_done_callback(uint8_t channel, uint16_t *buffer, uint8_t len)
{
	if (channel < IR_SUPPORTED_NUM_CHANNELS)
	{
		irRxChannel[channel].newData = TRUE;
		irRxChannel[channel].rxlen = len;
	}
}
#endif


#if IR_TX_ENABLE==1
void sns_irTransceive_TX_done_callback(uint8_t channel)
{
	if (channel < IR_SUPPORTED_NUM_CHANNELS)
	{
		irTxChannel[channel].sendComplete = TRUE;
	}
}
#endif


static void sns_irTransceive_setConfig(uint8_t channel, uint8_t config, uint8_t power, uint16_t modfreq)
{
	if (channel < IR_SUPPORTED_NUM_CHANNELS && config <= CAN_MODULE_ENUM_IRTRANSCEIVE_IRCONFIG_DIRECTION_RECEIVE && power < 4)
	{
#if IR_RX_ENABLE==1
		if (config == CAN_MODULE_ENUM_IRTRANSCEIVE_IRCONFIG_DIRECTION_RECEIVE)
		{
#if IR_TX_ENABLE==1
			irTxChannel[channel].state = sns_irTransceive_STATE_DISABLED;
#endif
			irRxChannel[channel].modfreq = modfreq;
			irRxChannel[channel].rxbuf = buf[channel];
			switch (channel)
			{
				case 0:
					gpio_clr_pin(sns_irTransceive_VCC_EN0_PIN);
					IrTransceiver_InitRxChannel(channel, irRxChannel[channel].rxbuf, sns_irTransceive_RX_done_callback, sns_irTransceive_RX0_PCINT, sns_irTransceive_RX0_PIN);
					break;
				case 1:
					gpio_clr_pin(sns_irTransceive_VCC_EN1_PIN);
					IrTransceiver_InitRxChannel(channel, irRxChannel[channel].rxbuf, sns_irTransceive_RX_done_callback, sns_irTransceive_RX1_PCINT, sns_irTransceive_RX1_PIN);
					break;
				case 2:
					gpio_clr_pin(sns_irTransceive_VCC_EN2_PIN);
					IrTransceiver_InitRxChannel(channel, irRxChannel[channel].rxbuf, sns_irTransceive_RX_done_callback, sns_irTransceive_RX2_PCINT, sns_irTransceive_RX2_PIN);
					break;
			}
			irRxChannel[channel].state = sns_irTransceive_STATE_RECEIVING;
		}
#endif

#if IR_TX_ENABLE==1
		if (config == CAN_MODULE_ENUM_IRTRANSCEIVE_IRCONFIG_DIRECTION_TRANSMIT)
		{
#if IR_RX_ENABLE==1
			irRxChannel[channel].state = sns_irTransceive_STATE_DISABLED;
#endif
			irTxChannel[channel].modfreq = modfreq;
			irTxChannel[channel].txbuf = buf[channel];
			switch (channel)
			{
				case 0:
#if IR_RX_ENABLE==1
					gpio_set_pin(sns_irTransceive_VCC_EN0_PIN);
					IrTransceiver_DeInitRxChannel(channel, sns_irTransceive_RX0_PCINT, sns_irTransceive_RX0_PIN);
#endif
					IrTransceiver_InitTxChannel(channel, sns_irTransceive_TX_done_callback, sns_irTransceive_TX0_PIN);

#if sns_irTransceive_ENABLE_PCA95xx==1
					Pca95xx_set_out(sns_irTransceive_TX0_PWRl);
					Pca95xx_set_out(sns_irTransceive_TX0_PWRh);
					if (power&0x1)
					{
						Pca95xx_set_in(sns_irTransceive_TX0_PWRl);
					}
					if (power&0x2)
					{
						Pca95xx_set_in(sns_irTransceive_TX0_PWRh);
					}
#endif
					break;
				case 1:
#if IR_RX_ENABLE==1
					gpio_set_pin(sns_irTransceive_VCC_EN1_PIN);
					IrTransceiver_DeInitRxChannel(channel, sns_irTransceive_RX1_PCINT, sns_irTransceive_RX1_PIN);
#endif
					IrTransceiver_InitTxChannel(channel, sns_irTransceive_TX_done_callback, sns_irTransceive_TX1_PIN);

#if sns_irTransceive_ENABLE_PCA95xx==1
					Pca95xx_set_out(sns_irTransceive_TX1_PWRl);
					Pca95xx_set_out(sns_irTransceive_TX1_PWRh);
					if (power&0x1)
					{
						Pca95xx_set_in(sns_irTransceive_TX1_PWRl);
					}
					if (power&0x2)
					{
						Pca95xx_set_in(sns_irTransceive_TX1_PWRh);
					}
#endif
					break;
				case 2:
#if IR_RX_ENABLE==1
					gpio_set_pin(sns_irTransceive_VCC_EN2_PIN);
					IrTransceiver_DeInitRxChannel(channel, sns_irTransceive_RX2_PCINT, sns_irTransceive_RX2_PIN);
#endif
					IrTransceiver_InitTxChannel(channel, sns_irTransceive_TX_done_callback, sns_irTransceive_TX2_PIN);

#if sns_irTransceive_ENABLE_PCA95xx==1
					Pca95xx_set_out(sns_irTransceive_TX2_PWRl);
					Pca95xx_set_out(sns_irTransceive_TX2_PWRh);
					if (power&0x1)
					{
						Pca95xx_set_in(sns_irTransceive_TX2_PWRl);
					}
					if (power&0x2)
					{
						Pca95xx_set_in(sns_irTransceive_TX2_PWRh);
					}
#endif
					break;
			}
			irTxChannel[channel].state = sns_irTransceive_STATE_START_IDLE;
		}
#endif
	}
}


void sns_irTransceive_Init(void)
{
	StdCan_Set_class(irTxMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(irTxMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	irTxMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_IRTRANSCEIVE;
	irTxMsg.Header.ModuleId = sns_irTransceive_ID;
	irTxMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_IR;
	irTxMsg.Length = 6;

	for (uint8_t i = 0; i < IR_SUPPORTED_NUM_CHANNELS; i++)
	{
#if IR_RX_ENABLE==1
		irRxChannel[i].state = sns_irTransceive_STATE_DISABLED;
		irRxChannel[i].newData = FALSE;
		irRxChannel[i].rxlen = 0;
		irRxChannel[i].proto.timeout=0;
		irRxChannel[i].proto.data=0;
		irRxChannel[i].proto.repeats=0;
		irRxChannel[i].proto.protocol=0;
#endif

#if IR_TX_ENABLE==1
		irTxChannel[i].state = sns_irTransceive_STATE_DISABLED;
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
	if (IR_SUPPORTED_NUM_CHANNELS>=1) irRxChannel[0].timerNum=sns_irTransceive_RX0_REPEATE_TIMER;
	if (IR_SUPPORTED_NUM_CHANNELS>=2) irRxChannel[1].timerNum=sns_irTransceive_RX1_REPEATE_TIMER;
	if (IR_SUPPORTED_NUM_CHANNELS>=3) irRxChannel[2].timerNum=sns_irTransceive_RX2_REPEATE_TIMER;
#endif

#if IR_TX_ENABLE==1
	if (IR_SUPPORTED_NUM_CHANNELS>=1) irTxChannel[0].timerNum=sns_irTransceive_TX0_REPEATE_TIMER;
	if (IR_SUPPORTED_NUM_CHANNELS>=2) irTxChannel[1].timerNum=sns_irTransceive_TX1_REPEATE_TIMER;
	if (IR_SUPPORTED_NUM_CHANNELS>=3) irTxChannel[2].timerNum=sns_irTransceive_TX2_REPEATE_TIMER;
#endif

	IrTransceiver_Init();
	/* Configure all TX VCC pins as outputs and disable them */
	gpio_set_out(sns_irTransceive_VCC_EN0_PIN);
	gpio_set_out(sns_irTransceive_VCC_EN1_PIN);
	gpio_set_out(sns_irTransceive_VCC_EN2_PIN);
	gpio_set_pin(sns_irTransceive_VCC_EN0_PIN);
	gpio_set_pin(sns_irTransceive_VCC_EN1_PIN);
	gpio_set_pin(sns_irTransceive_VCC_EN2_PIN);

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
#endif

	/* IR tx power pins on PCA95xx */
#if sns_irTransceive_ENABLE_PCA95xx==1
	Pca95xx_Init(0);

	Pca95xx_clr_pin(sns_irTransceive_TX0_PWRl);
	Pca95xx_set_out(sns_irTransceive_TX0_PWRl);
	Pca95xx_clr_pin(sns_irTransceive_TX0_PWRh);
	Pca95xx_set_out(sns_irTransceive_TX0_PWRh);

	Pca95xx_clr_pin(sns_irTransceive_TX1_PWRl);
	Pca95xx_set_out(sns_irTransceive_TX1_PWRl);
	Pca95xx_clr_pin(sns_irTransceive_TX1_PWRh);
	Pca95xx_set_out(sns_irTransceive_TX1_PWRh);

	Pca95xx_clr_pin(sns_irTransceive_TX2_PWRl);
	Pca95xx_set_out(sns_irTransceive_TX2_PWRl);
	Pca95xx_clr_pin(sns_irTransceive_TX2_PWRh);
	Pca95xx_set_out(sns_irTransceive_TX2_PWRh);
#endif

#ifdef sns_irTransceive_USEEEPROM
	if (EEDATA_OK)
	{
	  /* Use stored data to set initial values for the module */
		sns_irTransceive_setConfig(0, eeprom_read_byte(EEDATA.ch0_config), eeprom_read_byte(EEDATA.ch0_txpower), eeprom_read_word(EEDATA16.ch0_modfreq));
		sns_irTransceive_setConfig(1, eeprom_read_byte(EEDATA.ch1_config), eeprom_read_byte(EEDATA.ch1_txpower), eeprom_read_word(EEDATA16.ch1_modfreq));
		sns_irTransceive_setConfig(2, eeprom_read_byte(EEDATA.ch2_config), eeprom_read_byte(EEDATA.ch2_txpower), eeprom_read_word(EEDATA16.ch2_modfreq));
	}
	else
	{
	/* The CRC of the EEPROM is not correct, store default values and update CRC */
		eeprom_write_byte_crc(EEDATA.ch0_config, CAN_MODULE_ENUM_IRTRANSCEIVE_IRCONFIG_DIRECTION_AUTO, WITHOUT_CRC);
		eeprom_write_byte_crc(EEDATA.ch0_txpower, 0, WITHOUT_CRC);
		eeprom_write_word_crc(EEDATA16.ch0_modfreq, sns_irTransceive_BaseFrq/38000UL, WITHOUT_CRC);
		eeprom_write_byte_crc(EEDATA.ch1_config, CAN_MODULE_ENUM_IRTRANSCEIVE_IRCONFIG_DIRECTION_AUTO, WITHOUT_CRC);
		eeprom_write_byte_crc(EEDATA.ch1_txpower, 0, WITHOUT_CRC);
		eeprom_write_word_crc(EEDATA16.ch1_modfreq, sns_irTransceive_BaseFrq/38000UL, WITHOUT_CRC);
		eeprom_write_byte_crc(EEDATA.ch2_config, CAN_MODULE_ENUM_IRTRANSCEIVE_IRCONFIG_DIRECTION_AUTO, WITHOUT_CRC);
		eeprom_write_byte_crc(EEDATA.ch2_txpower, 0, WITHOUT_CRC);
		eeprom_write_word_crc(EEDATA16.ch2_modfreq, sns_irTransceive_BaseFrq/38000UL, WITHOUT_CRC);
		EEDATA_UPDATE_CRC;
	}
#endif
}

void sns_irTransceive_Process(void)
{
	for (uint8_t channel=0; channel < IR_SUPPORTED_NUM_CHANNELS; channel++)
	{

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
				/* TODO: move this line to the RX callback */
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
#elif sns_irTransceive_PRONTO_SUPPORT==1
					pronto_sendData(irRxChannel[channel].rxbuf, irRxChannel[channel].rxlen, channel, irRxChannel[channel].modfreq);
					irRxChannel[channel].proto.timeout=1;
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

		default:
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
		{
			if (expandProtocol(irTxChannel[channel].txbuf, &irTxChannel[channel].txlen, &irTxChannel[channel].proto) == IR_OK) {
				IrTransceiver_Transmit(channel, irTxChannel[channel].txbuf, 0, irTxChannel[channel].txlen);
				irTxChannel[channel].state = sns_irTransceive_STATE_TRANSMITTING;
			}
			else {
				irTxChannel[channel].state = sns_irTransceive_STATE_IDLE;
			}
			break;
		}

		case sns_irTransceive_STATE_START_TRANSMIT_PRONTO:
		{
			/* Start IR transmission. */
			IrTransceiver_Transmit(channel, irTxChannel[channel].txbuf, 0, irTxChannel[channel].onceSeqLen - 1);

			/* Enter transmitting state. */
			irTxChannel[channel].sendingPronto = TRUE;
			irTxChannel[channel].stopSend = FALSE;
			irTxChannel[channel].state = sns_irTransceive_STATE_TRANSMITTING;
			break;
		}

		case sns_irTransceive_STATE_PRONTO_REPEAT:
		{
			/* Check if done. */
			if ((irTxChannel[channel].repeatCount >= irTxChannel[channel].proto.repeats && irTxChannel[channel].proto.repeats != 0xFF) || irTxChannel[channel].stopSend) {
				/* Pronto transmission was stopped/completed */
				pronto_sendResponse(channel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_STOPPED);
				irTxChannel[channel].state = sns_irTransceive_STATE_START_IDLE;
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
				IrTransceiver_Transmit(channel, &irTxChannel[channel].txbuf[offset], 0, ((uint16_t)irTxChannel[channel].repSeqLen) - 1);
			}
			else {
				/* Use once seq */
				/* Don't transmit last passive. last passive handled by timer */
				IrTransceiver_Transmit(channel, irTxChannel[channel].txbuf, 0, irTxChannel[channel].txlen - 1);
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
				/* TODO: Not sure we want this response when sending non-pronto data, but probably doesn't hurt? /jm */
				pronto_sendResponse(channel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_STOPPED);

				irTxChannel[channel].state = sns_irTransceive_STATE_START_IDLE;
			}
			break;
		}

		case sns_irTransceive_STATE_START_IDLE:
			irTxChannel[channel].stopSend = FALSE;
			irTxChannel[channel].repeatCount = 0;
			irTxChannel[channel].proto.framecnt = 0;
			irTxChannel[channel].sendingPronto = FALSE;
			irTxChannel[channel].state = sns_irTransceive_STATE_IDLE;
			break;

		default:
			break;
		}
#endif

	}
}


/* Handle incoming CAN data */
void sns_irTransceive_HandleMessage(StdCan_Msg_t *rxMsg)
{
	/* Sanity check. */
	if (StdCan_Ret_class(rxMsg->Header) != CAN_MODULE_CLASS_SNS ||
		StdCan_Ret_direction(rxMsg->Header) != DIRECTIONFLAG_TO_OWNER ||
		rxMsg->Header.ModuleType != CAN_MODULE_TYPE_SNS_IRTRANSCEIVE ||
		rxMsg->Header.ModuleId != sns_irTransceive_ID) return;

	switch (rxMsg->Header.Command)
	{
#if IR_TX_ENABLE==1
	case CAN_MODULE_CMD_PHYSICAL_IR:
	{
		/* Get IR channel. */
		uint8_t channel = rxMsg->Data[0] >> 4;

		/* Sanity check. */
		if (channel >= IR_SUPPORTED_NUM_CHANNELS) {
			/* Invalid channel */
			break;
		}

		if ((0xf&rxMsg->Data[0]) == CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_PRESSED)
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
		else if ((0xf&rxMsg->Data[0]) == CAN_MODULE_ENUM_PHYSICAL_IR_STATUS_RELEASED)
		{
			if (irTxChannel[channel].state != sns_irTransceive_STATE_IDLE)
			{
				irTxChannel[channel].stopSend = TRUE;
			}
		}
		break;
	}

#if sns_irTransceive_PRONTO_SUPPORT==1
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOSTART:
	{
		/* Get IR channel. */
		uint8_t channel = rxMsg->Data[0] >> 4;

		/* Sanity check. */
		if (channel >= IR_SUPPORTED_NUM_CHANNELS) {
			/* Invalid channel */
			pronto_sendResponse(channel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ERROR);
			break;
		}
		if (irTxChannel[channel].state != sns_irTransceive_STATE_IDLE) {
			/* We're already transmitting on this channel */
			pronto_sendResponse(channel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ABORTED);
			break;
		}

		/* Check if format is supported. */
		if ((((uint16_t)(rxMsg->Data[0] & 0x0F) << 8) | (uint16_t)(rxMsg->Data[1])) != 0) {
			/* Invalid pronto format */
			pronto_sendResponse(channel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ERROR);
			break;
		}

		/* Extract received data. */

		uint32_t divider = (rxMsg->Data[2] << 8) | (rxMsg->Data[3] << 0);
		if (divider == 0) {
			/* Invalid wFrqDiv value */
			pronto_sendResponse(channel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ERROR);
			break;
		}
		uint32_t freqkHz = (sns_irTransceive_BaseFrq / (divider*1000));
		if (freqkHz == 0) {
			/* Invalid wFrqDiv value */
			pronto_sendResponse(channel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ERROR);
			break;
		}
		irTxChannel[channel].proto.modfreq = freqkHz;

		/* TODO: reconfigure with new modfreq, if no other channels sending pronto */

		/* Convert lenghts to bytes */
		irTxChannel[channel].onceSeqLen = 2*((((uint16_t)rxMsg->Data[4]) << 8) | (((uint16_t)rxMsg->Data[5]) << 0));
		irTxChannel[channel].repSeqLen = 2*((((uint16_t)rxMsg->Data[6]) << 8) | (((uint16_t)rxMsg->Data[7]) << 0));

		/* Enter prepering pronto state and clear transmit buffer. */
		irTxChannel[channel].state = sns_irTransceive_STATE_PREPARING_PRONTO;
		irTxChannel[channel].txlen = 0;
		irTxChannel[channel].expectedSeqNr = 0;
		activeChannel = channel;

		/* We're now waiting for pronto data */
		pronto_sendResponse(channel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_WAITING);
		break;
	}

	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOSTOP:
	{
		/* Get IR channel. */
		uint8_t channel = rxMsg->Data[0] >> 4;

		/* Sanity check. */
		if (channel >= IR_SUPPORTED_NUM_CHANNELS) {
			/* Invalid channel */
			pronto_sendResponse(channel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ERROR);
			break;
		}

		irTxChannel[channel].stopSend = TRUE;

		/* State check. If we're not currently transmitting anything, the STOPPED reponse will never occur. */
		/* TODO: better way to check this? /jm */
		if (irTxChannel[activeChannel].state != sns_irTransceive_STATE_START_TRANSMIT_PRONTO &&
			irTxChannel[activeChannel].state != sns_irTransceive_STATE_TRANSMITTING &&
			irTxChannel[activeChannel].state != sns_irTransceive_STATE_PRONTO_REPEAT &&
			irTxChannel[activeChannel].state != sns_irTransceive_STATE_PAUSING &&
			irTxChannel[activeChannel].state != sns_irTransceive_STATE_START_PAUSE)
		{
			/* Nothing to stop. Not in TX state. Respond that we're already stopped */
			pronto_sendResponse(activeChannel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ALREADYSTOPPED);
			break;
		}

		/* Response will be sent when transmission has been completed (i.e. stopped) */
		break;
	}

	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOCONTINUE:
	{
		/* Get IR channel. */
		uint8_t channel = rxMsg->Data[0] >> 4;

		/* Sanity check. */
		if (channel >= IR_SUPPORTED_NUM_CHANNELS) {
			/* Invalid channel */
			pronto_sendResponse(channel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ERROR);
			break;
		}

		/* State check. We must already be in some transmit state */
		/* TODO: better way to check this? /jm */
		if (irTxChannel[activeChannel].state != sns_irTransceive_STATE_START_TRANSMIT_PRONTO &&
			irTxChannel[activeChannel].state != sns_irTransceive_STATE_TRANSMITTING &&
			irTxChannel[activeChannel].state != sns_irTransceive_STATE_PRONTO_REPEAT &&
			irTxChannel[activeChannel].state != sns_irTransceive_STATE_PAUSING &&
			irTxChannel[activeChannel].state != sns_irTransceive_STATE_START_PAUSE)
		{
			/* Too late to continue! Not in TX state anymore */
			pronto_sendResponse(activeChannel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ALREADYSTOPPED);
			break;
		}

		/* Reset repeat counter, i.e. request additional repeats. */
		irTxChannel[channel].repeatCount = 0;

		/* The repeat period was extended. Still transmitting */
		pronto_sendResponse(channel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_TRANSMITTING);
		break;
	}

	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA1:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA2:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA3:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA4:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA5:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA6:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA7:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA8:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA9:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA10: /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA11: /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA12: /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA13: /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA14: /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA15: /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA16:
	{
		if (irTxChannel[activeChannel].state != sns_irTransceive_STATE_PREPARING_PRONTO) {
			/* Invalid state. CAN msg probably out of order */
			pronto_sendResponse(activeChannel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ERROR);
			break;
		}

		if ((rxMsg->Header.Command - CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTODATA1) != (irTxChannel[activeChannel].expectedSeqNr % 16)) {
			/* Unexpected CAN msg, out of order */
			pronto_sendResponse(activeChannel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ERROR);
			irTxChannel[activeChannel].state = sns_irTransceive_STATE_START_IDLE;
			break;
		}

		/* Decode pronto data */
		for (uint8_t i = 0; i < rxMsg->Length; i++) {
			if (decodeProntoData(activeChannel, rxMsg->Data[i]) != 1) {
				/* Decode error */
				pronto_sendResponse(activeChannel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ERROR);
				irTxChannel[activeChannel].state = sns_irTransceive_STATE_START_IDLE;
				return;
			}
		}

		irTxChannel[activeChannel].expectedSeqNr++;

		/* We're still waiting for more pronto data, or for pronto end */
		pronto_sendResponse(activeChannel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_WAITING);
		break;
	}
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND1:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND2:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND3:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND4:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND5:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND6:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND7:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND8:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND9:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND10:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND11:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND12:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND13:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND14:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND15:	 /* Fall through */
	case CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND16:	 /* Fall through */
	{
		if (irTxChannel[activeChannel].state != sns_irTransceive_STATE_PREPARING_PRONTO) {
			/* Invalid state. CAN msg probably out of order */
			pronto_sendResponse(activeChannel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ERROR);
			break;
		}

		if (((rxMsg->Header.Command - CAN_MODULE_CMD_IRTRANSCEIVE_IRPRONTOEND1) % 16) != (irTxChannel[activeChannel].expectedSeqNr % 16)) {
			/* Unexpected CAN msg, out of order */
			pronto_sendResponse(activeChannel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ERROR);
			irTxChannel[activeChannel].state = sns_irTransceive_STATE_START_IDLE;
			break;
		}

		/* Decode remaining pronto data (if any). -1 because last byte is repeat count */
		for (uint16_t i = 0; i < rxMsg->Length - 1; i++) {
			if (decodeProntoData(activeChannel, rxMsg->Data[i]) < 0) {
				/* Decode error */
				pronto_sendResponse(activeChannel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ERROR);
				irTxChannel[activeChannel].state = sns_irTransceive_STATE_START_IDLE;
				return;
			}
		}

		/* Sanity check */
		if (irTxChannel[activeChannel].txlen != (((uint16_t)irTxChannel[activeChannel].onceSeqLen + (uint16_t)irTxChannel[activeChannel].repSeqLen))) {
			/* Length doesn't match the expected length */
			pronto_sendResponse(activeChannel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_ERROR);
			irTxChannel[activeChannel].state = sns_irTransceive_STATE_IDLE;
			break;
		}

		/* Last byte determines nr of repeats. 0xFF means INF, 0 means no repeats */
		irTxChannel[activeChannel].proto.repeats = rxMsg->Data[rxMsg->Length - 1];
		irTxChannel[activeChannel].proto.timeout = 1; /* TODO: timer is buggy and doesn't support timeout 0, so we're forced to use 1ms extra between repeats */

		/* Convert pronto data to µs */
		/* TODO: calculate actual value instead of 27 */
		for (uint16_t i = 0; i < irTxChannel[activeChannel].txlen; i++) {
			uint32_t newVal = ((uint32_t)irTxChannel[activeChannel].txbuf[i]) * 27UL;
			if (newVal > 0xFFFFUL) {
				newVal = 0xFFFFUL;
			}
			irTxChannel[activeChannel].txbuf[i] = (uint16_t)newVal;
		}

		/* Send IR data */
		irTxChannel[activeChannel].state = sns_irTransceive_STATE_START_TRANSMIT_PRONTO;

		/* We're now transmitting the pronto data */
		pronto_sendResponse(activeChannel, CAN_MODULE_ENUM_IRTRANSCEIVE_IRPRONTORESPONSE_RESPONSE_TRANSMITTING);
		break;
	}

/* TODO: In IR state machine add sending a response frame when ir stops sending, also implement pronto repeat */
#endif
#endif

	case CAN_MODULE_CMD_IRTRANSCEIVE_IRCONFIG:
	{
		/* Get IR channel. */
		uint8_t channel = rxMsg->Data[0] >> 4;

		/* Sanity check. */
		if (channel >= IR_SUPPORTED_NUM_CHANNELS) {
			/* Invalid channel */
			break;
		}

		uint8_t config = rxMsg->Data[0] & 0x0f;
		uint8_t power = rxMsg->Data[1]>>6;
		uint32_t divider = ((uint16_t)rxMsg->Data[2] << 8) | ((uint16_t)rxMsg->Data[3] << 0);
		if (divider == 0) {
			/* Invalid modfreq */
			break;
		}
		uint32_t modfreq = (sns_irTransceive_BaseFrq / (divider*1000));
		if (modfreq == 0) {
			/* Invalid modfreq */
			break;
		}
		sns_irTransceive_setConfig(channel, config, power, modfreq);

#ifdef sns_irTransceive_USEEEPROM
		if (channel < IR_SUPPORTED_NUM_CHANNELS && config <= CAN_MODULE_ENUM_IRTRANSCEIVE_IRCONFIG_DIRECTION_RECEIVE && power < 4)
		{
			switch (channel)
			{
				case 0:
					eeprom_write_byte_crc(EEDATA.ch0_config, config, WITHOUT_CRC);
					eeprom_write_byte_crc(EEDATA.ch0_txpower, power, WITHOUT_CRC);
					eeprom_write_word_crc(EEDATA16.ch0_modfreq, modfreq, WITHOUT_CRC);
					break;
				case 1:
					eeprom_write_byte_crc(EEDATA.ch1_config, config, WITHOUT_CRC);
					eeprom_write_byte_crc(EEDATA.ch1_txpower, power, WITHOUT_CRC);
					eeprom_write_word_crc(EEDATA16.ch1_modfreq, modfreq, WITHOUT_CRC);
					break;
				case 2:
					eeprom_write_byte_crc(EEDATA.ch2_config, config, WITHOUT_CRC);
					eeprom_write_byte_crc(EEDATA.ch2_txpower, power, WITHOUT_CRC);
					eeprom_write_word_crc(EEDATA16.ch2_modfreq, modfreq, WITHOUT_CRC);
					break;
				default:
					break;
			}
			EEDATA_UPDATE_CRC;
		}
#endif
		break;
	}

	default:
		break;

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
