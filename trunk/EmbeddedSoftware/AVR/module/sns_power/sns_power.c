
#include "sns_power.h"

uint32_t volatile PreviusTimerValue, lastMeasurment;
uint16_t volatile EnergyCounter;
uint8_t volatile tmpCounter;
uint8_t sns_power_ReportInterval = (uint8_t)sns_power_SEND_PERIOD;
uint16_t volatile MeasurmentBuffer[32];
uint8_t volatile MeasurmentBufferPointer;

#ifdef sns_power_USEEEPROM
#include "sns_power_eeprom.h"
struct eeprom_sns_power EEMEM eeprom_sns_power = 
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file. 
		(uint8_t)sns_power_SEND_PERIOD,	// reportInterval
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 
#endif


void sns_power_pcint_callback(uint8_t id, uint8_t status) 
{
	//if (gpio_get_state(POWER_SNS_PIN) == 0)
	if (status == 0)
	{
		if (Timer_GetTicks() - PreviusTimerValue >= 16)
		{
			lastMeasurment =  Timer_GetTicks() - PreviusTimerValue;
			PreviusTimerValue = Timer_GetTicks();
			tmpCounter++;
			if(tmpCounter >= 10) {
				EnergyCounter++;
				tmpCounter = 0;
			}
		}
	}
}
void sns_power_Init(void)
{
#ifdef sns_power_USEEEPROM
	if (EEDATA_OK)
	{
	  ///TODO: Use stored data to set initial values for the module
	  sns_power_ReportInterval = eeprom_read_byte(EEDATA.reportInterval);
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_byte_crc(EEDATA.reportInterval, sns_power_SEND_PERIOD, WITHOUT_CRC);
	  EEDATA_UPDATE_CRC;
	  sns_power_ReportInterval = eeprom_read_byte(EEDATA.reportInterval);
	}
#endif  
	///TODO: Initialize hardware etc here
	gpio_set_in(POWER_SNS_PIN);	// Set to input
	gpio_set_pin(POWER_SNS_PIN);	// Enable pull-up
	
	// Enable IO-pin interrupt
	//PCICR |= (1<<POWER_SNS_PCIE);
	//POWER_SNS_PCMSK |= (1<<POWER_SNS_PCINT);
	Pcint_SetCallbackPin(sns_power_PCINT, POWER_SNS_PIN, &sns_power_pcint_callback);

	MeasurmentBufferPointer = 0;
	Timer_SetTimeout(sns_power_SEND_TIMER, sns_power_ReportInterval*1000 , TimerTypeFreeRunning, 0);
}

void sns_power_Process(void)
{
	if (lastMeasurment != 0) {
	      MeasurmentBufferPointer++;
	      MeasurmentBuffer[MeasurmentBufferPointer] = (uint16_t) lastMeasurment;//(uint16_t) 360000/lastMeasurment;
	      lastMeasurment=0;
	}
	StdCan_Msg_t txMsg;
	///TODO: Stuff that needs doing is done here
	if (Timer_Expired(sns_power_SEND_TIMER)) {
		//4 times average
		uint32_t Avg4 = MeasurmentBuffer[MeasurmentBufferPointer] + MeasurmentBuffer[MeasurmentBufferPointer-1] + MeasurmentBuffer[MeasurmentBufferPointer-2] + MeasurmentBuffer[MeasurmentBufferPointer-3];
		Avg4 = (360000/(Avg4/4));
		//32 times average
		uint32_t Avg32 = 0;
		for (uint8_t i = 0; i<32;i++) {
			 Avg32 += MeasurmentBuffer[i]; 
		}
		Avg32 /= 32;
		Avg32 = (360000/Avg32);

		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS); ///TODO: Change this to the actual class type
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_POWER; ///TODO: Change this to the actual module type
		txMsg.Header.ModuleId = sns_power_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_ELECTRICPOWER;
		txMsg.Length = 8;
		uint32_t tmp = 360000/MeasurmentBuffer[MeasurmentBufferPointer];
		txMsg.Data[0] = (uint8_t)((tmp>>8) & 0xff);
		txMsg.Data[1] = (uint8_t)(tmp & 0xff);
		txMsg.Data[2] = (uint8_t)((EnergyCounter>>8) & 0xff);
		txMsg.Data[3] = (uint8_t)(EnergyCounter & 0xff);
		txMsg.Data[4] = (uint8_t)((Avg4>>8) & 0xff);
		txMsg.Data[5] = (uint8_t)((Avg4) & 0xff);
		txMsg.Data[6] = (uint8_t)((Avg32>>8) & 0xff);
		txMsg.Data[7] = (uint8_t)(Avg32 & 0xff);
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
	}
}

void sns_power_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_POWER &&
		rxMsg->Header.ModuleId == sns_power_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL:
		if (rxMsg->Length > 0)
		{
			sns_power_ReportInterval = rxMsg->Data[0];
			Timer_SetTimeout(sns_power_SEND_TIMER, sns_power_ReportInterval*1000 , TimerTypeFreeRunning, 0);
		}
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_POWER;
		txMsg.Header.ModuleId = sns_power_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL;
		txMsg.Length = 1;
		txMsg.Data[0] = sns_power_ReportInterval;
		StdCan_Put(&txMsg);
		break;
		case CAN_MODULE_CMD_POWER_SETENERGY:
		if (rxMsg->Length == 2)
		{
			EnergyCounter = rxMsg->Data[1];
			EnergyCounter += ((uint16_t)rxMsg->Data[0])<<8;
		}
		break;
		}
	}
}

void sns_power_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_POWER; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = sns_power_ID;
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
