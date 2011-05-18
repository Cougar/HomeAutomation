
#include "sns_counter.h"

#ifdef sns_counter_DEBOUNCE_TIME_MS
static uint32_t volatile PreviousTimerValue = 0;
#endif
#ifdef sns_counter_USEEEPROM
static uint8_t volatile StoreInEEPROM = 0;
#endif
static uint8_t sns_counter_ReportInterval = (uint8_t)sns_counter_SEND_PERIOD;
static uint32_t volatile Count0 = 0;

#ifdef sns_counter_USEEEPROM
#include "sns_counter_eeprom.h"
struct eeprom_sns_counter EEMEM eeprom_sns_counter =
{
	{
		(uint8_t)sns_counter_SEND_PERIOD,	// reportInterval
		0					// Count0
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
};
#endif

uint8_t sns_counter_save_data(void)
{
#ifdef sns_counter_USEEEPROM
	eeprom_write_byte_crc(EEDATA.reportInterval, sns_counter_SEND_PERIOD, WITHOUT_CRC);
	eeprom_write_dword_crc(EEDATA32.Count0, 0, WITHOUT_CRC);
	EEDATA_UPDATE_CRC;
	return 0;
#else
	return 2;
#endif
}

void sns_counter_send_counter(void)
{
	StdCan_Msg_t txMsg;
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_COUNTER;
	txMsg.Header.ModuleId = sns_counter_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_COUNTER;
	txMsg.Length = 5;
	txMsg.Data[0] = 0;	// CH0
	txMsg.Data[4] = (uint8_t)Count0 & 0xff;
	txMsg.Data[3] = (uint8_t)(Count0 >> 8) & 0xff;
	txMsg.Data[2] = (uint8_t)(Count0 >> 16) & 0xff;
	txMsg.Data[1] = (uint8_t)(Count0 >> 24) & 0xff;
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}

void sns_counter_pcint_callback(uint8_t id, uint8_t status)
{
	if (status == 0)
	{
#ifdef sns_counter_DEBOUNCE_TIME_MS
		if (Timer_GetTicks() - PreviousTimerValue >= sns_counter_DEBOUNCE_TIME_MS)
		{
			PreviousTimerValue = Timer_GetTicks();
#endif
			Count0++;
			sns_counter_send_counter();
#if defined sns_counter_USEEEPROM && defined sns_counter_CH0_EEPROM_FREQ
			if (Count0 %sns_counter_CH0_EEPROM_FREQ == 0)
				StoreInEEPROM = 1;
#endif
#ifdef sns_counter_DEBOUNCE_TIME_MS
		}
#endif
	}
}

void sns_counter_Init(void)
{
#ifdef sns_counter_USEEEPROM
	if (EEDATA_OK)
	{
		//Use stored data to set initial values for the module
		sns_counter_ReportInterval = eeprom_read_byte(EEDATA.reportInterval);
		Count0 = eeprom_read_dword(EEDATA32.Count0);
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
		sns_counter_save_data();
	}
#endif
	//Initialize hardware etc here
	gpio_set_in(sns_counter_CH0);	// Set to input
	gpio_set_pin(sns_counter_CH0);	// Enable pull-up

	Pcint_SetCallbackPin(sns_counter_PCINT_CH0, sns_counter_CH0 , &sns_counter_pcint_callback);
#ifdef sns_counter_SEND_TIMER
	Timer_SetTimeout(sns_counter_SEND_TIMER, sns_counter_ReportInterval*1000 , TimerTypeFreeRunning, 0);
#endif
}

void sns_counter_Process(void)
{
#ifdef sns_counter_USEEEPROM
	if (StoreInEEPROM == 1)
	{
		StoreInEEPROM = 0;
		eeprom_write_dword_crc(EEDATA32.Count0, (uint32_t)(Count0), WITH_CRC);
	}
#endif
#ifdef sns_counter_SEND_TIMER
	if (Timer_Expired(sns_counter_SEND_TIMER))
		sns_counter_send_counter();
#endif
}

void sns_counter_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_COUNTER &&
		rxMsg->Header.ModuleId == sns_counter_ID)
	{
		StdCan_Msg_t txMsg;

		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_COUNTER;
		txMsg.Header.ModuleId = sns_counter_ID;

		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL:
			if (rxMsg->Length > 0)
			{
				sns_counter_ReportInterval = rxMsg->Data[0];
				Timer_SetTimeout(sns_counter_SEND_TIMER, sns_counter_ReportInterval*1000 , TimerTypeFreeRunning, 0);
			}
			txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL;
			txMsg.Length = 1;
			txMsg.Data[0] = sns_counter_ReportInterval;
			StdCan_Put(&txMsg);
			break;
		case CAN_MODULE_CMD_COUNTER_SETCOUNTER:
			if (rxMsg->Length == 5 && rxMsg->Data[0] == 0)
			{
				Count0 = rxMsg->Data[4];
				Count0 += ((uint32_t)rxMsg->Data[3])<<8;
				Count0 += ((uint32_t)rxMsg->Data[2])<<16;
				Count0 += ((uint32_t)rxMsg->Data[1])<<24;
			}
			txMsg.Header.Command = CAN_MODULE_CMD_COUNTER_SETCOUNTER;
			txMsg.Length = 5;
			txMsg.Data[0] = 0;	// CH0
			txMsg.Data[4] = (uint8_t)Count0 & 0xff;
			txMsg.Data[3] = (uint8_t)(Count0 >> 8) & 0xff;
			txMsg.Data[2] = (uint8_t)(Count0 >> 16) & 0xff;
			txMsg.Data[1] = (uint8_t)(Count0 >> 24) & 0xff;
			StdCan_Put(&txMsg);
			break;
		}
	}
}

void sns_counter_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_COUNTER;
	txMsg.Header.ModuleId = sns_counter_ID;
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
