
#include "sns_water.h"

static uint8_t volatile sns_water_status = LOW;
static uint8_t sns_water_ReportInterval = (uint8_t)sns_water_SEND_PERIOD_S;
static uint32_t volatile sns_water_TickCnt=0;
static uint32_t volatile sns_water_TimePrevTick;
static uint32_t volatile sns_water_Buffer[4]= {0x0000ffff,0x0000ffff,0x0000ffff,0x0000ffff};
static uint8_t volatile sns_water_BufPointer=0;
static uint32_t volatile sns_water_StoreCnt=0;

StdCan_Msg_t txMsg;

#if sns_water_USEEEPROM==1
#include "sns_water_eeprom.h"
struct eeprom_sns_water EEMEM eeprom_sns_water =
{
	{
		  (uint8_t)sns_water_SEND_PERIOD_S,	// reportInterval
		  0,	// VolumeCounterUpper
		  0		// VolumeCounterLower
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
};
#endif

void sns_water_Init(void)
{
#if sns_water_USEEEPROM==1
	if (EEDATA_OK)
	{  // Use stored data to set initial values for the module
printf("ok\n");
	  sns_water_ReportInterval = eeprom_read_byte(EEDATA.reportInterval);
	  sns_water_TickCnt = eeprom_read_word(EEDATA16.VolumeCounterLower);
	  sns_water_TickCnt += (((uint32_t)(eeprom_read_word(EEDATA16.VolumeCounterUpper)))<<16);
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
printf("nok;");
	  eeprom_write_byte_crc(EEDATA.reportInterval, sns_water_SEND_PERIOD_S, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.VolumeCounterUpper, 0, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.VolumeCounterLower, 0, WITHOUT_CRC);
	  EEDATA_UPDATE_CRC;
	}
#endif

	ADC_Init();
	sns_water_TimePrevTick = Timer_GetTicks();
	Timer_SetTimeout(sns_water_ADPOLL_TIMER, sns_water_ADPOLL_PERIOD_MS, TimerTypeFreeRunning, 0);
	Timer_SetTimeout(sns_water_SEND_TIMER, sns_water_SEND_PERIOD_S*1000, TimerTypeFreeRunning, 0);

	uint16_t ADvalue = ADC_Get(sns_water_AD_CHANNEL);
	if (ADvalue > (sns_water_HIGH_THRESHOLD_MV*1024UL/5000UL))
	{
		sns_water_status = HIGH;
	}

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_WATER;
	txMsg.Header.ModuleId = sns_water_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_FLOW;
	txMsg.Length = 6;
}

void sns_water_Process(void)
{
	/*
	Notes 
	maybe its better to skip the poll timer and always poll when in process
	*/
	uint8_t oldstatus = sns_water_status;
	if (Timer_Expired(sns_water_ADPOLL_TIMER))
	{
		uint16_t ADvalue = ADC_Get(sns_water_AD_CHANNEL);
		if (sns_water_status == LOW && ADvalue > (sns_water_HIGH_THRESHOLD_MV*1024UL/5000UL))
		{
			sns_water_status = HIGH;
		}
		else if (sns_water_status == HIGH && ADvalue < (sns_water_LOW_THRESHOLD_MV*1024UL/5000UL))
		{
			sns_water_status = LOW;
		}
	}

	/* If status changed == one tick */
	if (oldstatus != sns_water_status)
	{
		sns_water_TickCnt += 1;

		sns_water_Buffer[sns_water_BufPointer] = Timer_GetTicks() - sns_water_TimePrevTick;
		if (sns_water_BufPointer++ == 4)
		{
			sns_water_BufPointer = 0;
		}

		sns_water_TimePrevTick = Timer_GetTicks();

#if sns_water_USEEEPROM==1
		if ((sns_water_TickCnt % 2048) == 0)
		{
			eeprom_write_word_crc(EEDATA16.VolumeCounterUpper, (uint16_t)((sns_water_TickCnt>>16) & 0xffff), WITHOUT_CRC);
			eeprom_write_word_crc(EEDATA16.VolumeCounterLower, (uint16_t)(sns_water_TickCnt & 0xffff), WITH_CRC);
		}
#endif
	}

	if (Timer_Expired(sns_water_SEND_TIMER))
	{
		uint32_t flow4 = 0;
		for (uint8_t i = 0; i<4;i++) {
			 flow4 += sns_water_Buffer[i]; 
		}
		flow4 = flow4/4;
		/* The flow is ml per tick / time between ticks */
		flow4 = sns_water_UL_PER_TICK/flow4;

		txMsg.Data[0] = (uint8_t)((flow4>>8) & 0xff);
		txMsg.Data[1] = (uint8_t)(flow4 & 0xff);
		txMsg.Data[5] = (uint8_t)(sns_water_TickCnt*sns_water_UL_PER_TICK/1000) & 0xff;
		txMsg.Data[4] = (uint8_t)((sns_water_TickCnt*sns_water_UL_PER_TICK/1000) >> 8) & 0xff;
		txMsg.Data[3] = (uint8_t)((sns_water_TickCnt*sns_water_UL_PER_TICK/1000) >> 16) & 0xff;
		txMsg.Data[2] = (uint8_t)((sns_water_TickCnt*sns_water_UL_PER_TICK/1000) >> 24) & 0xff;
#if CAN_PRINTF==1
//		txMsg.Data[5] = (uint8_t)sns_water_TickCnt & 0xff;
//		txMsg.Data[4] = (uint8_t)(sns_water_TickCnt >> 8) & 0xff;
//		txMsg.Data[3] = (uint8_t)(sns_water_TickCnt >> 16) & 0xff;
//		txMsg.Data[2] = (uint8_t)(sns_water_TickCnt >> 24) & 0xff;
#endif
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);

		/* TODO How to handle no flow? (means no ticks, no new time-diffs) */
		/* This solution starts to fill buffer with older values if ticks come in slower than we send out data */
		if (Timer_GetTicks() - sns_water_TimePrevTick > sns_water_SEND_PERIOD_S*1000)
		{
			sns_water_Buffer[sns_water_BufPointer] = Timer_GetTicks() - sns_water_TimePrevTick;
			if (sns_water_BufPointer++ == 4)
			{
				sns_water_BufPointer = 0;
			}
		}
	}
}

void sns_water_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_WATER &&
		rxMsg->Header.ModuleId == sns_water_ID)
	{
		StdCan_Msg_t txMsg2;
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL:
			if (rxMsg->Length > 0)
			{
				sns_water_ReportInterval = rxMsg->Data[0];
				Timer_SetTimeout(sns_water_SEND_TIMER, sns_water_ReportInterval*1000 , TimerTypeFreeRunning, 0);
			}
			StdCan_Set_class(txMsg2.Header, CAN_MODULE_CLASS_SNS);
			StdCan_Set_direction(txMsg2.Header, DIRECTIONFLAG_FROM_OWNER);
			txMsg2.Header.ModuleType = CAN_MODULE_TYPE_SNS_WATER;
			txMsg2.Header.ModuleId = sns_water_ID;
			txMsg2.Header.Command = CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL;
			txMsg2.Length = 1;
			txMsg2.Data[0] = sns_water_ReportInterval;
			StdCan_Put(&txMsg2);
			break;
		case CAN_MODULE_CMD_WATER_SETVOLUME:
			if (rxMsg->Length == 4)
			{
				sns_water_TickCnt = (rxMsg->Data[3]*1000/sns_water_UL_PER_TICK);
				sns_water_TickCnt += ((uint32_t)(rxMsg->Data[2]*1000/sns_water_UL_PER_TICK))<<8;
				sns_water_TickCnt += ((uint32_t)(rxMsg->Data[1]*1000/sns_water_UL_PER_TICK))<<16;
				sns_water_TickCnt += ((uint32_t)(rxMsg->Data[0]*1000/sns_water_UL_PER_TICK))<<24;
			}
			StdCan_Set_class(txMsg2.Header, CAN_MODULE_CLASS_SNS);
			StdCan_Set_direction(txMsg2.Header, DIRECTIONFLAG_FROM_OWNER);
			txMsg2.Header.ModuleType = CAN_MODULE_TYPE_SNS_WATER;
			txMsg2.Header.ModuleId = sns_water_ID;
			txMsg2.Header.Command = CAN_MODULE_CMD_WATER_SETVOLUME;
			txMsg2.Length = 1;
			txMsg2.Data[3] = (uint8_t)(sns_water_TickCnt*sns_water_UL_PER_TICK/1000) & 0xff;
			txMsg2.Data[2] = (uint8_t)((sns_water_TickCnt*sns_water_UL_PER_TICK/1000) >> 8) & 0xff;
			txMsg2.Data[1] = (uint8_t)((sns_water_TickCnt*sns_water_UL_PER_TICK/1000) >> 16) & 0xff;
			txMsg2.Data[0] = (uint8_t)((sns_water_TickCnt*sns_water_UL_PER_TICK/1000) >> 24) & 0xff;
			StdCan_Put(&txMsg2);
			
			break;
		}
	}
}

void sns_water_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_WATER;
	txMsg.Header.ModuleId = sns_water_ID;
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
