
#include "sns_counter.h"

#ifdef sns_counter_CH0_DEBOUNCE_TIME_MS==1 | sns_counter_CH1_DEBOUNCE_TIME_MS==1 | sns_counter_CH2_DEBOUNCE_TIME_MS==1 | sns_counter_CH3_DEBOUNCE_TIME_MS==1 | sns_counter_CH4_DEBOUNCE_TIME_MS==1 | sns_counter_CH5_DEBOUNCE_TIME_MS==1 | sns_counter_CH6_DEBOUNCE_TIME_MS==1 | sns_counter_CH7_DEBOUNCE_TIME_MS==1
static uint32_t volatile PreviousTimerValue[8];
#endif
#if sns_counter_USEEEPROM==1
static uint8_t volatile StoreInEEPROM = 0;
#endif
static uint8_t sns_counter_ReportInterval = (uint8_t)sns_counter_SEND_PERIOD;
static uint32_t volatile Count[8];

#define	IO_LOW		0

#if sns_counter_USEEEPROM==1
#include "sns_counter_eeprom.h"
struct eeprom_sns_counter EEMEM eeprom_sns_counter =
{
	{
		(uint8_t)sns_counter_SEND_PERIOD,	// reportInterval
#ifdef sns_counter_CH0
		0,					// Count[0]
#endif
#ifdef sns_counter_CH1
		0,					// Count[1]
#endif
#ifdef sns_counter_CH2
		0,					// Count[2]
#endif
#ifdef sns_counter_CH3
		0,					// Count[3]
#endif
#ifdef sns_counter_CH4
		0,					// Count[4]
#endif
#ifdef sns_counter_CH5
		0,					// Count[5]
#endif
#ifdef sns_counter_CH6
		0,					// Count[6]
#endif
#ifdef sns_counter_CH7
		0,					// Count[7]
#endif
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
};
#endif

uint8_t sns_counter_save_data(void)
{
#if sns_counter_USEEEPROM==1
	eeprom_write_byte_crc(EEDATA.reportInterval, sns_counter_SEND_PERIOD, WITHOUT_CRC);
#ifdef sns_counter_CH0
	eeprom_write_dword_crc(EEDATA32.Count[0], 0, WITHOUT_CRC);
#endif
#ifdef sns_counter_CH1
	eeprom_write_dword_crc(EEDATA32.Count[1], 0, WITHOUT_CRC);
#endif
#ifdef sns_counter_CH2
	eeprom_write_dword_crc(EEDATA32.Count[2], 0, WITHOUT_CRC);
#endif
#ifdef sns_counter_CH3
	eeprom_write_dword_crc(EEDATA32.Count[3], 0, WITHOUT_CRC);
#endif
#ifdef sns_counter_CH4
	eeprom_write_dword_crc(EEDATA32.Count[4], 0, WITHOUT_CRC);
#endif
#ifdef sns_counter_CH5
	eeprom_write_dword_crc(EEDATA32.Count[5], 0, WITHOUT_CRC);
#endif
#ifdef sns_counter_CH6
	eeprom_write_dword_crc(EEDATA32.Count[6], 0, WITHOUT_CRC);
#endif
#ifdef sns_counter_CH7
	eeprom_write_dword_crc(EEDATA32.Count[7], 0, WITHOUT_CRC);
#endif
	EEDATA_UPDATE_CRC;
	return 0;
#else
	return 2;
#endif
}

void sns_counter_send_counter(uint8_t index)
{
	StdCan_Msg_t txMsg;
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_COUNTER;
	txMsg.Header.ModuleId = sns_counter_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_COUNTER;
	txMsg.Length = 5;
	txMsg.Data[0] = index;
	txMsg.Data[4] = (uint8_t)Count[index] & 0xff;
	txMsg.Data[3] = (uint8_t)(Count[index] >> 8) & 0xff;
	txMsg.Data[2] = (uint8_t)(Count[index] >> 16) & 0xff;
	txMsg.Data[1] = (uint8_t)(Count[index] >> 24) & 0xff;
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}

/*

Callback function that is called from pin-change driver on AVR IO
id gives which pin that changed, status is the new state of the changed pin

*/
void sns_counter_pcint_callback(uint8_t id, uint8_t status)
{
#ifdef sns_counter_CH0
	if (id == sns_counter_PCINT_CH0) {
		if (status == IO_LOW) {
#if sns_counter_CH0_DEBOUNCE_TIME_MS
			if (Timer_GetTicks() - PreviousTimerValue[0] >= sns_counter_CH0_DEBOUNCE_TIME_MS)
			{
				PreviousTimerValue[0] = Timer_GetTicks();
#endif
				Count[0]++;
				sns_counter_send_counter(0);
#if defined sns_counter_USEEEPROM && defined sns_counter_CH0_EEPROM_FREQ
				if (Count[0] % sns_counter_CH0_EEPROM_FREQ == 0)
					StoreInEEPROM = 1;
#endif
#if sns_counter_CH0_DEBOUNCE_TIME_MS
			}
#endif
		}
	}
#endif
#ifdef sns_counter_CH1
	if (id == sns_counter_PCINT_CH1) {
		if (status == IO_LOW) {
#if sns_counter_CH1_DEBOUNCE_TIME_MS
			if (Timer_GetTicks() - PreviousTimerValue[1] >= sns_counter_CH1_DEBOUNCE_TIME_MS)
			{
				PreviousTimerValue[1] = Timer_GetTicks();
#endif
				Count[1]++;
				sns_counter_send_counter(1);
#if defined sns_counter_USEEEPROM && defined sns_counter_CH1_EEPROM_FREQ
				if (Count[1] % sns_counter_CH1_EEPROM_FREQ == 0)
					StoreInEEPROM = 1;
#endif
#if sns_counter_CH1_DEBOUNCE_TIME_MS
			}
#endif
		}
	}
#endif
#ifdef sns_counter_CH2
	if (id == sns_counter_PCINT_CH2) {
		if (status == IO_LOW) {
#if sns_counter_CH2_DEBOUNCE_TIME_MS
			if (Timer_GetTicks() - PreviousTimerValue[2] >= sns_counter_CH2_DEBOUNCE_TIME_MS)
			{
				PreviousTimerValue[2] = Timer_GetTicks();
#endif
				Count[2]++;
				sns_counter_send_counter(2);
#if defined sns_counter_USEEEPROM && defined sns_counter_CH2_EEPROM_FREQ
				if (Count[2] % sns_counter_CH2_EEPROM_FREQ == 0)
					StoreInEEPROM = 1;
#endif
#if sns_counter_CH2_DEBOUNCE_TIME_MS
			}
#endif
		}
	}
#endif
#ifdef sns_counter_CH3
	if (id == sns_counter_PCINT_CH3) {
		if (status == IO_LOW) {
#if sns_counter_CH3_DEBOUNCE_TIME_MS
			if (Timer_GetTicks() - PreviousTimerValue[3] >= sns_counter_CH3_DEBOUNCE_TIME_MS)
			{
				PreviousTimerValue[3] = Timer_GetTicks();
#endif
				Count[3]++;
				sns_counter_send_counter(3);
#if defined sns_counter_USEEEPROM && defined sns_counter_CH3_EEPROM_FREQ
				if (Count[3] % sns_counter_CH3_EEPROM_FREQ == 0)
					StoreInEEPROM = 1;
#endif
#if sns_counter_CH3_DEBOUNCE_TIME_MS
			}
#endif
		}
	}
#endif
#ifdef sns_counter_CH4
	if (id == sns_counter_PCINT_CH4) {
		if (status == IO_LOW) {
#if sns_counter_CH4_DEBOUNCE_TIME_MS
			if (Timer_GetTicks() - PreviousTimerValue[4] >= sns_counter_CH4_DEBOUNCE_TIME_MS)
			{
				PreviousTimerValue[4] = Timer_GetTicks();
#endif
				Count[4]++;
				sns_counter_send_counter(4);
#if defined sns_counter_USEEEPROM && defined sns_counter_CH4_EEPROM_FREQ
				if (Count[4] % sns_counter_CH4_EEPROM_FREQ == 0)
					StoreInEEPROM = 1;
#endif
#if sns_counter_CH5_DEBOUNCE_TIME_MS
			}
#endif
		}
	}
#endif
#ifdef sns_counter_CH5
	if (id == sns_counter_PCINT_CH5) {
		if (status == IO_LOW) {
#if sns_counter_CH5_DEBOUNCE_TIME_MS
			if (Timer_GetTicks() - PreviousTimerValue[5] >= sns_counter_CH5_DEBOUNCE_TIME_MS)
			{
				PreviousTimerValue[5] = Timer_GetTicks();
#endif
				Count[5]++;
				sns_counter_send_counter(5);
#if defined sns_counter_USEEEPROM && defined sns_counter_CH5_EEPROM_FREQ
				if (Count[5] % sns_counter_CH5_EEPROM_FREQ == 0)
					StoreInEEPROM = 1;
#endif
#if sns_counter_CH5_DEBOUNCE_TIME_MS
			}
#endif
		}
	}
#endif
#ifdef sns_counter_CH6
	if (id == sns_counter_PCINT_CH6) {
		if (status == IO_LOW) {
#if sns_counter_CH6_DEBOUNCE_TIME_MS
			if (Timer_GetTicks() - PreviousTimerValue[6] >= sns_counter_CH6_DEBOUNCE_TIME_MS)
			{
				PreviousTimerValue[6] = Timer_GetTicks();
#endif
				Count[6]++;
				sns_counter_send_counter(6);
#if defined sns_counter_USEEEPROM && defined sns_counter_CH6_EEPROM_FREQ
				if (Count[6] % sns_counter_CH6_EEPROM_FREQ == 0)
					StoreInEEPROM = 1;
#endif
#if sns_counter_CH6_DEBOUNCE_TIME_MS
			}
#endif
		}
	}
#endif
#ifdef sns_counter_CH7
	if (id == sns_counter_PCINT_CH7) {
		if (status == IO_LOW) {
#if sns_counter_CH7_DEBOUNCE_TIME_MS
			if (Timer_GetTicks() - PreviousTimerValue[7] >= sns_counter_CH7_DEBOUNCE_TIME_MS)
			{
				PreviousTimerValue[7] = Timer_GetTicks();
#endif
				Count[7]++;
				sns_counter_send_counter(7);
#if defined sns_counter_USEEEPROM && defined sns_counter_CH7_EEPROM_FREQ
				if (Count[7] % sns_counter_CH7_EEPROM_FREQ == 0)
					StoreInEEPROM = 1;
#endif
#if sns_counter_CH7_DEBOUNCE_TIME_MS
			}
#endif
		}
	}
#endif
}

void sns_counter_Init(void)
{
#if sns_counter_USEEEPROM==1
	if (EEDATA_OK)
	{
		//Use stored data to set initial values for the module
		sns_counter_ReportInterval = eeprom_read_byte(EEDATA.reportInterval);
#ifdef sns_counter_CH0
		Count[0] = eeprom_read_dword(EEDATA32.Count[0]);
#endif
#ifdef sns_counter_CH1
		Count[1] = eeprom_read_dword(EEDATA32.Count[1]);
#endif
#ifdef sns_counter_CH2
		Count[2] = eeprom_read_dword(EEDATA32.Count[2]);
#endif
#ifdef sns_counter_CH3
		Count[3] = eeprom_read_dword(EEDATA32.Count[3]);
#endif
#ifdef sns_counter_CH4
		Count[4] = eeprom_read_dword(EEDATA32.Count[4]);
#endif
#ifdef sns_counter_CH5
		Count[5] = eeprom_read_dword(EEDATA32.Count[5]);
#endif
#ifdef sns_counter_CH6
		Count[6] = eeprom_read_dword(EEDATA32.Count[6]);
#endif
#ifdef sns_counter_CH7
		Count[7] = eeprom_read_dword(EEDATA32.Count[7]);
#endif
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
		sns_counter_save_data();
	}
#endif
	//Initialize hardware etc here
#ifdef sns_counter_CH0
	#if (sns_counter_CH0_pullup == 1)
		gpio_set_pullup(sns_counter_CH0);
	#else
		gpio_clr_pullup(sns_counter_CH0);
	#endif
		gpio_set_in(sns_counter_CH0);
		Pcint_SetCallbackPin(sns_counter_PCINT_CH0, sns_counter_CH0 , &sns_counter_pcint_callback);
#endif
#ifdef sns_counter_CH1
	#if (sns_counter_CH1_pullup == 1)
		gpio_set_pullup(sns_counter_CH1);
	#else
		gpio_clr_pullup(sns_counter_CH1);
	#endif
		gpio_set_in(sns_counter_CH1);
		Pcint_SetCallbackPin(sns_counter_PCINT_CH1, sns_counter_CH1 , &sns_counter_pcint_callback);
#endif
#ifdef sns_counter_CH2
	#if (sns_counter_CH2_pullup == 1)
		gpio_set_pullup(sns_counter_CH2);
	#else
		gpio_clr_pullup(sns_counter_CH2);
	#endif
		gpio_set_in(sns_counter_CH2);
		Pcint_SetCallbackPin(sns_counter_PCINT_CH2, sns_counter_CH2 , &sns_counter_pcint_callback);
#endif
#ifdef sns_counter_CH3
	#if (sns_counter_CH3_pullup == 1)
		gpio_set_pullup(sns_counter_CH3);
	#else
		gpio_clr_pullup(sns_counter_CH3);
	#endif
		gpio_set_in(sns_counter_CH3);
		Pcint_SetCallbackPin(sns_counter_PCINT_CH3, sns_counter_CH3 , &sns_counter_pcint_callback);
#endif
#ifdef sns_counter_CH4
	#if (sns_counter_CH4_pullup == 1)
		gpio_set_pullup(sns_counter_CH4);
	#else
		gpio_clr_pullup(sns_counter_CH4);
	#endif
		gpio_set_in(sns_counter_CH4);
		Pcint_SetCallbackPin(sns_counter_PCINT_CH4, sns_counter_CH4 , &sns_counter_pcint_callback);
#endif
#ifdef sns_counter_CH5
	#if (sns_counter_CH5_pullup == 1)
		gpio_set_pullup(sns_counter_CH5);
	#else
		gpio_clr_pullup(sns_counter_CH5);
	#endif
		gpio_set_in(sns_counter_CH5);
		Pcint_SetCallbackPin(sns_counter_PCINT_CH5, sns_counter_CH5 , &sns_counter_pcint_callback);
#endif
#ifdef sns_counter_CH6
	#if (sns_counter_CH6_pullup == 1)
		gpio_set_pullup(sns_counter_CH6);
	#else
		gpio_clr_pullup(sns_counter_CH6);
	#endif
		gpio_set_in(sns_counter_CH6);
		Pcint_SetCallbackPin(sns_counter_PCINT_CH6, sns_counter_CH6 , &sns_counter_pcint_callback);
#endif
#ifdef sns_counter_CH7
	#if (sns_counter_CH7_pullup == 1)
		gpio_set_pullup(sns_counter_CH7);
	#else
		gpio_clr_pullup(sns_counter_CH7);
	#endif
		gpio_set_in(sns_counter_CH7);
		Pcint_SetCallbackPin(sns_counter_PCINT_CH7, sns_counter_CH7 , &sns_counter_pcint_callback);
#endif

#ifdef sns_counter_SEND_TIMER
	Timer_SetTimeout(sns_counter_SEND_TIMER, sns_counter_ReportInterval*1000 , TimerTypeFreeRunning, 0);
#endif
}

void sns_counter_Process(void)
{
#if sns_counter_USEEEPROM==1
	if (StoreInEEPROM == 1)
	{
		StoreInEEPROM = 0;
		eeprom_write_dword_crc(EEDATA32.Count[0], (uint32_t)(Count[0]), WITH_CRC);
	}
#endif
#ifdef sns_counter_SEND_TIMER
	if (Timer_Expired(sns_counter_SEND_TIMER)) {
	#ifdef sns_counter_CH0
		sns_counter_send_counter(0);
	#endif
	#ifdef sns_counter_CH1
		sns_counter_send_counter(1);
	#endif
	#ifdef sns_counter_CH2
		sns_counter_send_counter(2);
	#endif
	#ifdef sns_counter_CH3
		sns_counter_send_counter(3);
	#endif
	#ifdef sns_counter_CH4
		sns_counter_send_counter(4);
	#endif
	#ifdef sns_counter_CH5
		sns_counter_send_counter(5);
	#endif
	#ifdef sns_counter_CH6
		sns_counter_send_counter(6);
	#endif
	#ifdef sns_counter_CH7
		sns_counter_send_counter(7);
	#endif
	}
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
			if (rxMsg->Length != 5)
				break;
			if (
#ifdef sns_counter_CH0
				(rxMsg->Data[0] != 0) &&
#endif
#ifdef sns_counter_CH1
				(rxMsg->Data[0] != 1) &&
#endif
#ifdef sns_counter_CH2
				(rxMsg->Data[0] != 2) &&
#endif
#ifdef sns_counter_CH3
				(rxMsg->Data[0] != 3) &&
#endif
#ifdef sns_counter_CH4
				(rxMsg->Data[0] != 4) &&
#endif
#ifdef sns_counter_CH5
				(rxMsg->Data[0] != 5) &&
#endif
#ifdef sns_counter_CH6
				(rxMsg->Data[0] != 6) &&
#endif
#ifdef sns_counter_CH7
				(rxMsg->Data[0] != 7) &&
#endif
				1)
				break;
			Count[rxMsg->Data[0]] = rxMsg->Data[4];
			Count[rxMsg->Data[0]] += ((uint32_t)rxMsg->Data[3])<<8;
			Count[rxMsg->Data[0]] += ((uint32_t)rxMsg->Data[2])<<16;
			Count[rxMsg->Data[0]] += ((uint32_t)rxMsg->Data[1])<<24;
			txMsg.Header.Command = CAN_MODULE_CMD_COUNTER_SETCOUNTER;
			txMsg.Length = 5;
			txMsg.Data[0] = rxMsg->Data[0];	// CH
			txMsg.Data[4] = (uint8_t)Count[rxMsg->Data[0]] & 0xff;
			txMsg.Data[3] = (uint8_t)(Count[rxMsg->Data[0]] >> 8) & 0xff;
			txMsg.Data[2] = (uint8_t)(Count[rxMsg->Data[0]] >> 16) & 0xff;
			txMsg.Data[1] = (uint8_t)(Count[rxMsg->Data[0]] >> 24) & 0xff;
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

	uint32_t HwId=BIOS_GetHwId();
	txMsg.Data[0] = HwId&0xff;
	txMsg.Data[1] = (HwId>>8)&0xff;
	txMsg.Data[2] = (HwId>>16)&0xff;
	txMsg.Data[3] = (HwId>>24)&0xff;

	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;

	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}
