
#include "sns_power.h"

static uint32_t volatile PreviusTimerValue, lastMeasurment;

#ifdef sns_power_10000_PULSES_PER_KWH
  static uint8_t volatile tmpCounter=0;
#endif
static uint8_t volatile StoreInEEPROM = 0;
static uint8_t sns_power_ReportInterval = (uint8_t)sns_power_SEND_PERIOD;
static uint16_t volatile MeasurmentBuffer[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static uint8_t volatile MeasurmentBufferPointer=0;
static uint32_t volatile EnergyCounter=0;
#if sns_power_SEND_1_MIN_AVG == 1
  static uint16_t volatile avgCounter = 0;
#endif

#ifdef POWER_SNS_PIN_ch2
  static uint32_t volatile PreviusTimerValue_ch2, lastMeasurment_ch2;

  #ifdef sns_power_10000_PULSES_PER_KWH
    static uint8_t volatile tmpCounter_ch2=0;
  #endif
  static uint16_t volatile MeasurmentBuffer_ch2[32]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  static uint8_t volatile MeasurmentBufferPointer_ch2=0;
  static uint32_t volatile EnergyCounter_ch2=0;
  #if sns_power_SEND_1_MIN_AVG == 1
    static uint16_t volatile avgCounter_ch2 = 0;
  #endif
#endif
#if sns_power_USEEEPROM==1
  #include "sns_power_eeprom.h"
  struct eeprom_sns_power EEMEM eeprom_sns_power = 
  {
	  {
		  ///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file. 
		  (uint8_t)sns_power_SEND_PERIOD,	// reportInterval
		  0,	// EnergyCounterUpper
		  0,	// EnergyCounterLower
		  #ifdef POWER_SNS_PIN_ch2
		    0,	// EnergyCounterUpper_ch2
		    0,	// EnergyCounterLower_ch2
		  #endif
	  },
	  0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
  }; 
#endif

#if sns_power_SEND_1_MIN_AVG == 1
  void sns_power_timer_callback(uint8_t timer) 
  {
	  StdCan_Msg_t txMsg;
	  StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	  StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	  txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_POWER;
	  txMsg.Header.ModuleId = sns_power_ID;
	  txMsg.Header.Command = CAN_MODULE_CMD_POWER_AVGPOWER;
	  txMsg.Length = 2;
	  cli();
	  txMsg.Data[0] = (uint8_t)((avgCounter>>8) & 0xff);
	  txMsg.Data[1] = (uint8_t)(avgCounter & 0xff);
	  avgCounter = 0;
	  sei();
	  while (StdCan_Put(&txMsg) != StdCan_Ret_OK);

	  #ifdef POWER_SNS_PIN_ch2
	    txMsg.Length = 2;
	    txMsg.Header.ModuleId = sns_power_ID_ch2;
	    cli();
	    txMsg.Data[0] = (uint8_t)((avgCounter_ch2>>8) & 0xff);
	    txMsg.Data[1] = (uint8_t)(avgCounter_ch2 & 0xff);
	    avgCounter = 0;
	    sei();
	    while (StdCan_Put(&txMsg) != StdCan_Ret_OK);

	  #endif
  }
#endif

void sns_power_pcint_callback(uint8_t id, uint8_t status) 
{
	if (status == 0)
	{
		if (Timer_GetTicks() - PreviusTimerValue >= 16)
		{
			MeasurmentBufferPointer++;
			if (MeasurmentBufferPointer >= 32) MeasurmentBufferPointer = 0;
			lastMeasurment =  Timer_GetTicks() - PreviusTimerValue;
			MeasurmentBuffer[MeasurmentBufferPointer] = (uint16_t) (lastMeasurment & 0x0000FFFF);
			PreviusTimerValue = Timer_GetTicks();
			#if sns_power_100_PULSES_PER_KWH == 1
				EnergyCounter+=10;
				if (EnergyCounter % 1024 == 0)
					StoreInEEPROM = 1;	
			#endif
			#if sns_power_1000_PULSES_PER_KWH == 1
				EnergyCounter++;
				if (EnergyCounter % 1024 == 0)
					StoreInEEPROM = 1;	
			#endif
			#if sns_power_10000_PULSES_PER_KWH == 1
				tmpCounter++;
				#if sns_power_SEND_1_MIN_AVG == 1
				avgCounter++;
				#endif
				if(tmpCounter >= 10) {
					EnergyCounter++;
					if (EnergyCounter % 1024 == 0)
						StoreInEEPROM = 1;
					tmpCounter = 0;
				}
			#endif

#ifdef sns_power_LED_PIN
			gpio_toggle_pin(sns_power_LED_PIN);	// toggle pin
#endif
		}
	}
}

#ifdef POWER_SNS_PIN_ch2
void sns_power_pcint_callback_ch2(uint8_t id, uint8_t status) 
{
	if (status == 0)
	{
		if (Timer_GetTicks() - PreviusTimerValue_ch2 >= 16)
		{
			MeasurmentBufferPointer_ch2++;
			if (MeasurmentBufferPointer_ch2 >= 32) MeasurmentBufferPointer_ch2 = 0;
			lastMeasurment_ch2 =  Timer_GetTicks() - PreviusTimerValue_ch2;
			MeasurmentBuffer_ch2[MeasurmentBufferPointer_ch2] = (uint16_t) (lastMeasurment_ch2 & 0x0000FFFF);
			PreviusTimerValue_ch2 = Timer_GetTicks();
			#if sns_power_100_PULSES_PER_KWH == 1
				EnergyCounter_ch2+=10;
			#endif
			#if sns_power_1000_PULSES_PER_KWH == 1
				EnergyCounter_ch2++;
			#endif
			#if sns_power_10000_PULSES_PER_KWH == 1
				tmpCounter_ch2++;
				#if sns_power_SEND_1_MIN_AVG == 1
				avgCounter_ch2++;
				#endif
				if(tmpCounter_ch2 >= 10) {
					EnergyCounter_ch2++;
					tmpCounter_ch2 = 0;
				}
			#endif
#ifdef sns_power_LED_PIN
			gpio_toggle_pin(sns_power_LED_PIN);	// toggle pin
#endif
		}
	}
}
#endif

void sns_power_Init(void)
{
#if sns_power_USEEEPROM==1
	if (EEDATA_OK)
	{
	  ///TODO: Use stored data to set initial values for the module
	  sns_power_ReportInterval = eeprom_read_byte(EEDATA.reportInterval);
	  EnergyCounter = eeprom_read_word(EEDATA16.EnergyCounterLower);
	  EnergyCounter += (((uint32_t)(eeprom_read_word(EEDATA16.EnergyCounterUpper)))<<16);
	    #ifdef POWER_SNS_PIN_ch2
	      EnergyCounter_ch2 = eeprom_read_word(EEDATA16.EnergyCounterLower_ch2);
	      EnergyCounter_ch2 += (((uint32_t)(eeprom_read_word(EEDATA16.EnergyCounterUpper_ch2)))<<16);
	    #endif
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_byte_crc(EEDATA.reportInterval, sns_power_SEND_PERIOD, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.EnergyCounterUpper, 0, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.EnergyCounterLower, 0, WITHOUT_CRC);
	  #ifdef POWER_SNS_PIN_ch2
	    eeprom_write_word_crc(EEDATA16.EnergyCounterUpper_ch2, 0, WITHOUT_CRC);
	    eeprom_write_word_crc(EEDATA16.EnergyCounterLower_ch2, 0, WITHOUT_CRC);
	  #endif
	  EEDATA_UPDATE_CRC;
	  sns_power_ReportInterval = eeprom_read_byte(EEDATA.reportInterval);
	}
#endif  
	///Initialize hardware etc
	gpio_set_in(POWER_SNS_PIN);	// Set to input
#if sns_power_PIN_PULLUP==1
	gpio_set_pullup(POWER_SNS_PIN);	// Enable pull-up
#endif

	Pcint_SetCallbackPin(sns_power_PCINT, POWER_SNS_PIN, &sns_power_pcint_callback);

	MeasurmentBufferPointer = 0;
#ifdef POWER_SNS_PIN_ch2
	gpio_set_in(POWER_SNS_PIN_ch2);	// Set to input
#if sns_power_PIN_PULLUP_ch2==1
	gpio_set_pullup(POWER_SNS_PIN_ch2);	// Enable pull-up
#endif
	
	Pcint_SetCallbackPin(sns_power_PCINT_ch2, POWER_SNS_PIN_ch2, &sns_power_pcint_callback_ch2);

	MeasurmentBufferPointer_ch2 = 0;
#endif
	Timer_SetTimeout(sns_power_SEND_TIMER, sns_power_ReportInterval*1000 , TimerTypeFreeRunning, 0);
#if sns_power_SEND_1_MIN_AVG == 1
	Timer_SetTimeout(sns_power_SEND_TIMER_1_MIN_AVG, 60000-10 , TimerTypeFreeRunning, &sns_power_timer_callback);
#endif

#ifdef sns_power_LED_PIN
	gpio_set_out(sns_power_LED_PIN);	// Set to output
	gpio_clr_pin(sns_power_LED_PIN);	// clear pin
#endif
}

void sns_power_Process(void)
{
	if (StoreInEEPROM == 1)
	{
		StoreInEEPROM = 0;
		
		eeprom_write_word_crc(EEDATA16.EnergyCounterUpper, (uint16_t)((EnergyCounter>>16) & 0xffff), WITHOUT_CRC);
		eeprom_write_word_crc(EEDATA16.EnergyCounterLower, (uint16_t)(EnergyCounter & 0xffff), WITH_CRC);
	#ifdef POWER_SNS_PIN_ch2
		eeprom_write_word_crc(EEDATA16.EnergyCounterUpper_ch2, (uint16_t)((EnergyCounter_ch2>>16) & 0xffff), WITHOUT_CRC);
		eeprom_write_word_crc(EEDATA16.EnergyCounterLower_ch2, (uint16_t)(EnergyCounter_ch2 & 0xffff), WITH_CRC);
	#endif
	}
	StdCan_Msg_t txMsg;
	if (Timer_Expired(sns_power_SEND_TIMER)) {
		//4 times average
		/*uint32_t Avg4 = MeasurmentBuffer[MeasurmentBufferPointer] + MeasurmentBuffer[MeasurmentBufferPointer-1] + MeasurmentBuffer[MeasurmentBufferPointer-2] + MeasurmentBuffer[MeasurmentBufferPointer-3];
		Avg4 = (360000/(Avg4/4));
		*/
		//32 times average
		uint32_t Avg32 = 0;
		for (uint8_t i = 0; i<32;i++) {
			 Avg32 += MeasurmentBuffer[i]; 
		}
		Avg32 /= 32;
		#if sns_power_100_PULSES_PER_KWH == 1
			Avg32 = (36000000UL/Avg32);
		#endif
		#if sns_power_1000_PULSES_PER_KWH == 1
			Avg32 = (3600000/Avg32);
		#endif
		#if sns_power_10000_PULSES_PER_KWH == 1
			Avg32 = (360000/Avg32);
		#endif

			
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_POWER;
		txMsg.Header.ModuleId = sns_power_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_ELECTRICPOWER;
		txMsg.Length = 8;
		#if sns_power_100_PULSES_PER_KWH == 1
			uint32_t tmp = 36000000UL/MeasurmentBuffer[MeasurmentBufferPointer];
		#endif
		#if sns_power_1000_PULSES_PER_KWH == 1
			uint32_t tmp = 3600000/MeasurmentBuffer[MeasurmentBufferPointer];
		#endif
		#if sns_power_10000_PULSES_PER_KWH == 1
			uint32_t tmp = 360000/MeasurmentBuffer[MeasurmentBufferPointer];
		#endif
		txMsg.Data[0] = (uint8_t)((tmp>>8) & 0xff);
		txMsg.Data[1] = (uint8_t)(tmp & 0xff);
		txMsg.Data[5] = (uint8_t)EnergyCounter & 0xff;
		txMsg.Data[4] = (uint8_t)(EnergyCounter >> 8) & 0xff;
		txMsg.Data[3] = (uint8_t)(EnergyCounter >> 16) & 0xff;
		txMsg.Data[2] = (uint8_t)(EnergyCounter >> 24) & 0xff;
		txMsg.Data[6] = (uint8_t)((Avg32>>8) & 0xff);
		txMsg.Data[7] = (uint8_t)(Avg32 & 0xff);
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
		
	#ifdef POWER_SNS_PIN_ch2
		uint32_t Avg32_ch2 = 0;
		for (uint8_t i = 0; i<32;i++) {
			 Avg32_ch2 += MeasurmentBuffer_ch2[i]; 
		}
		Avg32_ch2 /= 32;
		#if sns_power_100_PULSES_PER_KWH == 1
			Avg32_ch2 = (36000000UL/Avg32_ch2);
		#endif
		#if sns_power_1000_PULSES_PER_KWH == 1
			Avg32_ch2 = (3600000/Avg32_ch2);
		#endif
		#if sns_power_10000_PULSES_PER_KWH == 1
			Avg32_ch2 = (360000/Avg32_ch2);
		#endif
			
		txMsg.Header.ModuleId = sns_power_ID_ch2;
		#if sns_power_100_PULSES_PER_KWH == 1
			tmp = 36000000UL/MeasurmentBuffer_ch2[MeasurmentBufferPointer_ch2];
		#endif
		#if sns_power_1000_PULSES_PER_KWH == 1
			tmp = 3600000/MeasurmentBuffer_ch2[MeasurmentBufferPointer_ch2];
		#endif
		#if sns_power_10000_PULSES_PER_KWH == 1
			tmp = 360000/MeasurmentBuffer_ch2[MeasurmentBufferPointer_ch2];
		#endif
		txMsg.Data[0] = (uint8_t)((tmp>>8) & 0xff);
		txMsg.Data[1] = (uint8_t)(tmp & 0xff);
		txMsg.Data[5] = (uint8_t)EnergyCounter_ch2 & 0xff;
		txMsg.Data[4] = (uint8_t)(EnergyCounter_ch2 >> 8) & 0xff;
		txMsg.Data[3] = (uint8_t)(EnergyCounter_ch2 >> 16) & 0xff;
		txMsg.Data[2] = (uint8_t)(EnergyCounter_ch2 >> 24) & 0xff;
		txMsg.Data[6] = (uint8_t)((Avg32_ch2>>8) & 0xff);
		txMsg.Data[7] = (uint8_t)(Avg32_ch2 & 0xff);
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
		
	#endif
		
		
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
	
#ifdef POWER_SNS_PIN_ch2	
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_POWER &&
		rxMsg->Header.ModuleId == sns_power_ID_ch2)
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
			txMsg.Header.ModuleId = sns_power_ID_ch2;
			txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL;
			txMsg.Length = 1;
			txMsg.Data[0] = sns_power_ReportInterval;
			StdCan_Put(&txMsg);
			break;
		case CAN_MODULE_CMD_POWER_SETENERGY:
			if (rxMsg->Length == 4)
			{
				EnergyCounter_ch2 = rxMsg->Data[3];
				EnergyCounter_ch2 += ((uint32_t)rxMsg->Data[2])<<8;
				EnergyCounter_ch2 += ((uint32_t)rxMsg->Data[1])<<16;
				EnergyCounter_ch2 += ((uint32_t)rxMsg->Data[0])<<24;
			}
			StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
			StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
			txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_POWER;
			txMsg.Header.ModuleId = sns_power_ID_ch2;
			txMsg.Header.Command = CAN_MODULE_CMD_POWER_SETENERGY;
			txMsg.Length = 1;
			txMsg.Data[3] = (uint8_t)EnergyCounter_ch2 & 0xff;
			txMsg.Data[2] = (uint8_t)(EnergyCounter_ch2 >> 8) & 0xff;
			txMsg.Data[1] = (uint8_t)(EnergyCounter_ch2 >> 16) & 0xff;
			txMsg.Data[0] = (uint8_t)(EnergyCounter_ch2 >> 24) & 0xff;
			StdCan_Put(&txMsg);
			
			break;
		}
	}
#endif
	
}

void sns_power_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_POWER;
	txMsg.Header.ModuleId = sns_power_ID;
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
	
	
#ifdef POWER_SNS_PIN_ch2	
	txMsg.Header.ModuleId = sns_power_ID_ch2;
	txMsg.Data[5] = ModuleSequenceNumber;
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
#endif
}
