
#include "sns_power.h"

static uint32_t volatile PreviusTimerValue, lastMeasurment;

static uint8_t volatile tmpCounter=0, StoreInEEPROM = 0;
static uint8_t sns_power_ReportInterval = (uint8_t)sns_power_SEND_PERIOD;
static uint16_t volatile MeasurmentBuffer[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static uint8_t volatile MeasurmentBufferPointer=0;
static uint32_t volatile EnergyCounter=0;

#ifdef sns_power_USEEEPROM
#include "sns_power_eeprom.h"
struct eeprom_sns_power EEMEM eeprom_sns_power = 
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file. 
		(uint8_t)sns_power_SEND_PERIOD,	// reportInterval
		0,	// EnergyCounterUpper
		0,	// EnergyCounterLower
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
			MeasurmentBufferPointer++;
			if (MeasurmentBufferPointer >= 32) MeasurmentBufferPointer = 0;
			lastMeasurment =  Timer_GetTicks() - PreviusTimerValue;
			MeasurmentBuffer[MeasurmentBufferPointer] = (uint16_t) (lastMeasurment & 0x0000FFFF);
			PreviusTimerValue = Timer_GetTicks();
			tmpCounter++;
			if(tmpCounter >= 10) {
				EnergyCounter++;
				if (EnergyCounter % 1024 == 0)
					StoreInEEPROM = 1;
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
	  EnergyCounter = eeprom_read_word(EEDATA16.EnergyCounterLower);
	  EnergyCounter += (((uint32_t)(eeprom_read_word(EEDATA16.EnergyCounterUpper)))<<16);
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_byte_crc(EEDATA.reportInterval, sns_power_SEND_PERIOD, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.EnergyCounterUpper, 0, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.EnergyCounterLower, 0, WITHOUT_CRC);
	  EEDATA_UPDATE_CRC;
	  sns_power_ReportInterval = eeprom_read_byte(EEDATA.reportInterval);
	}
#endif  
	///TODO: Initialize hardware etc here
	gpio_set_in(POWER_SNS_PIN);	// Set to input
	gpio_set_pin(POWER_SNS_PIN);	// Enable pull-up
	
	Pcint_SetCallbackPin(sns_power_PCINT, POWER_SNS_PIN, &sns_power_pcint_callback);

	MeasurmentBufferPointer = 0;
	Timer_SetTimeout(sns_power_SEND_TIMER, sns_power_ReportInterval*1000 , TimerTypeFreeRunning, 0);
}

void sns_power_Process(void)
{
	if (StoreInEEPROM == 1)
	{
		StoreInEEPROM = 0;
		eeprom_write_word_crc(EEDATA16.EnergyCounterUpper, (uint16_t)((EnergyCounter>>16) & 0xffff), WITHOUT_CRC);
		eeprom_write_word_crc(EEDATA16.EnergyCounterLower, (uint16_t)(EnergyCounter & 0xffff), WITH_CRC);
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
		txMsg.Data[5] = (uint8_t)EnergyCounter & 0xff;
		txMsg.Data[4] = (uint8_t)(EnergyCounter >> 8) & 0xff;
		txMsg.Data[3] = (uint8_t)(EnergyCounter >> 16) & 0xff;
		txMsg.Data[2] = (uint8_t)(EnergyCounter >> 24) & 0xff;
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
StdCan_Msg_t txMsg;
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL:
			if (rxMsg->Length > 0)
			{
				sns_power_ReportInterval = rxMsg->Data[0];
				Timer_SetTimeout(sns_power_SEND_TIMER, sns_power_ReportInterval*1000 , TimerTypeFreeRunning, 0);
			}
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
			if (rxMsg->Length == 4)
			{
				EnergyCounter = rxMsg->Data[3];
				EnergyCounter += ((uint32_t)rxMsg->Data[2])<<8;
				EnergyCounter += ((uint32_t)rxMsg->Data[1])<<16;
				EnergyCounter += ((uint32_t)rxMsg->Data[0])<<24;
			}
			StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
			StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
			txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_POWER;
			txMsg.Header.ModuleId = sns_power_ID;
			txMsg.Header.Command = CAN_MODULE_CMD_POWER_SETENERGY;
			txMsg.Length = 1;
			txMsg.Data[3] = (uint8_t)EnergyCounter & 0xff;
			txMsg.Data[2] = (uint8_t)(EnergyCounter >> 8) & 0xff;
			txMsg.Data[1] = (uint8_t)(EnergyCounter >> 16) & 0xff;
			txMsg.Data[0] = (uint8_t)(EnergyCounter >> 24) & 0xff;
			StdCan_Put(&txMsg);
			
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
