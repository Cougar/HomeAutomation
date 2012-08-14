
#include "sns_water.h"

static uint8_t volatile sns_water_status = LOW;
static uint32_t volatile sns_water_liter_cnt=0;
static uint32_t volatile sns_water_microliter_cnt=0;
static uint32_t volatile sns_water_TimePrevTick;
static uint32_t volatile sns_water_Buffer[4]= {0x0000ffff,0x0000ffff,0x0000ffff,0x0000ffff};
static uint8_t volatile sns_water_Buf_Pointer=0;

/* Set this parameter to 1000 to send volume in ml instead of liters, useful for debugging */
#define MIKROLITERINLITER 1000000

/* When printf is enabled the number of ticks are sent on can instead of volume */
#if CAN_PRINTF==1
uint32_t sns_water_debug_cnt=0;
#endif

StdCan_Msg_t txMsg;

#if sns_water_USEEEPROM==1
#include "sns_water_eeprom.h"
struct eeprom_sns_water EEMEM eeprom_sns_water =
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file.
		0xAB,		// x
		0x1234		// y
		0x12345678	// z
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
};
#endif

void sns_water_Init(void)
{
#if sns_water_USEEEPROM==1
	if (EEDATA_OK)
	{
	  ///TODO: Use stored data to set initial values for the module
	  blablaX = eeprom_read_byte(EEDATA.x);
	  blablaY = eeprom_read_word(EEDATA16.y);
	  blablaZ = eeprom_read_dword(EEDATA32.y);
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_byte_crc(EEDATA.x, 0xAB, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.y, 0x1234, WITHOUT_CRC);
	  eeprom_write_dword_crc(EEDATA32.y, 0x12345678, WITHOUT_CRC);
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
	
	/* If status changed */
	if (oldstatus != sns_water_status)
	{
		sns_water_microliter_cnt += sns_water_UL_PER_TICK;
		/* If microliter counter has more than one liter */
		while (sns_water_microliter_cnt >= 1000)
		{
			/* Add one liter to liter counter */
			sns_water_liter_cnt += 1;
			/* Decrease microliter counter with the volume added to liter counter */
			sns_water_microliter_cnt -= 1000;
		}
		
		sns_water_Buffer[sns_water_Buf_Pointer] = Timer_GetTicks() - sns_water_TimePrevTick;
		if (sns_water_Buf_Pointer++ == 4)
		{
			sns_water_Buf_Pointer = 0;
		}
		
		sns_water_TimePrevTick = Timer_GetTicks();

#if CAN_PRINTF==1
		sns_water_debug_cnt += 1;
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
		txMsg.Data[5] = (uint8_t)sns_water_liter_cnt & 0xff;
		txMsg.Data[4] = (uint8_t)(sns_water_liter_cnt >> 8) & 0xff;
		txMsg.Data[3] = (uint8_t)(sns_water_liter_cnt >> 16) & 0xff;
		txMsg.Data[2] = (uint8_t)(sns_water_liter_cnt >> 24) & 0xff;
#if CAN_PRINTF==1
		txMsg.Data[5] = (uint8_t)sns_water_debug_cnt & 0xff;
		txMsg.Data[4] = (uint8_t)(sns_water_debug_cnt >> 8) & 0xff;
		txMsg.Data[3] = (uint8_t)(sns_water_debug_cnt >> 16) & 0xff;
		txMsg.Data[2] = (uint8_t)(sns_water_debug_cnt >> 24) & 0xff;
#endif
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);

		/* TODO How to handle no flow? (means no ticks, no new time-diffs) */
		/* This solution starts to fill buffer with older values if ticks come in slower than we send out data */
		if (Timer_GetTicks() - sns_water_TimePrevTick > sns_water_SEND_PERIOD_S*2000)
		{
			sns_water_Buffer[sns_water_Buf_Pointer] = Timer_GetTicks() - sns_water_TimePrevTick;
			if (sns_water_Buf_Pointer++ == 4)
			{
				sns_water_Buf_Pointer = 0;
			}
		}
#if CAN_PRINTF==1
		//printf("c:%d\n", sns_water_debug_cnt);
#endif
	}
}

void sns_water_HandleMessage(StdCan_Msg_t *rxMsg)
{
#if 0
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_WATER &&
		rxMsg->Header.ModuleId == sns_water_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_DUMMY:
		
		break;
		}
	}
#endif
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
