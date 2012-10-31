
#include "sns_ultrasonic.h"
#include <util/delay.h>

uint8_t sns_ultrasonic_ReportInterval = (uint8_t)sns_ultrasonic_DEFAULT_REPORTINTERVAL;
uint16_t sns_ultrasonic_BottomLevel = (uint16_t)sns_ultrasonic_DEFAULT_BOTTOMLEVEL;
uint16_t sns_ultrasonic_TopLevel = (uint16_t)sns_ultrasonic_DEFAULT_TOPLEVEL;
uint8_t sns_ultrasonic_Mode = (uint8_t)sns_ultrasonic_DEFAULT_MODE;
uint16_t sns_ultrasonic_Measurement = 0u;
uint8_t sns_ultrasonic_Measurement_flag = 0;
uint8_t sns_ultrasonic_MeasurementState = 0;
uint16_t lastCaptures[4];

#if sns_ultrasonic_USEEEPROM==1
#include "sns_ultrasonic_eeprom.h"
struct eeprom_sns_ultrasonic EEMEM eeprom_sns_ultrasonic =
{
	{
		//Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file.
		sns_ultrasonic_DEFAULT_REPORTINTERVAL,		// reportIntervall
		sns_ultrasonic_DEFAULT_BOTTOMLEVEL,
		sns_ultrasonic_DEFAULT_TOPLEVEL,
		sns_ultrasonic_DEFAULT_MODE,
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
};
#endif

void StartMeasure_callback(uint8_t timer);

/*********************************************************************//**
Function: ISR(TIMER1_CAPT_vect )
Purpose:  Executed when pin change on Input capture pin.
Input:    -
Returns:  -
**************************************************************************/
ISR(TIMER1_CAPT_vect) {
	static uint16_t last = 0;
	static uint8_t index = 0;
	if (sns_ultrasonic_MeasurementState == 1) {
		last =  ICR1;
		TCCR1B=0x83u;	//prescaler=64, input noice reduction and trigg on falling edge
		sns_ultrasonic_MeasurementState = 2;
	}	
	else if (sns_ultrasonic_MeasurementState == 2){
		TCCR1B=0xC3u;	//prescaler=64, input noice reduction and trigg on rising edge
		lastCaptures[index] = ICR1 - last;
		sns_ultrasonic_MeasurementState = 0;
		Timer_SetTimeout(sns_ultrasonic_MEASURE_TIMER, 10 , TimerTypeOneShot, &StartMeasure_callback);
		index++;
		if (index >= 4) {
			uint8_t flags_max = 0;
			uint8_t flags_min = 0;
			uint16_t temp;
			index = 0;
			temp = lastCaptures[0];
			flags_max = 0x01;
			if (lastCaptures[1] >= temp) {
			  flags_max = 0x02;
			  temp = lastCaptures[1];
			}
			if (lastCaptures[2] >= temp) {
			  flags_max = 0x04;
			  temp = lastCaptures[2];
			}
			if (lastCaptures[3] >= temp) {
			  flags_max = 0x08;
			}
			
			temp = lastCaptures[0];
			flags_min = 0x01;
			if (lastCaptures[1] <= temp) {
			  flags_min = 0x02;
			  temp = lastCaptures[1];
			}
			if (lastCaptures[2] <= temp) {
			  flags_min = 0x04;
			  temp = lastCaptures[2];
			}
			if (lastCaptures[3] <= temp) {
			  flags_min = 0x08;
			}
			if (flags_max == flags_min) 
			{
			  temp = 2*temp;
			} else {
			  
			  temp = 0;
			  if (((flags_max | flags_min) & 0x01u) == 0) {
			    temp += lastCaptures[0];
			  }
			  if (((flags_max | flags_min) & 0x02u) == 0) {
			    temp += lastCaptures[1];
			  }
			  if (((flags_max | flags_min) & 0x04u) == 0) {
			    temp += lastCaptures[2];
			  }
			  if (((flags_max | flags_min) & 0x08u) == 0) {
			    temp += lastCaptures[3];
			  }
			}
			sns_ultrasonic_Measurement = temp >> 1; //devide by 2 to the the average
			sns_ultrasonic_Measurement_flag = 1;
		}
	}
} /* ISR(TIMER1_CAPT_vect) */


void StartMeasure_callback(uint8_t timer)
{
	gpio_clr_pin(sns_ultrasonic_Trigger_PIN);
	sns_ultrasonic_MeasurementState = 1;
	_delay_us(20);
	gpio_set_pin(sns_ultrasonic_Trigger_PIN);
}

void sns_ultrasonic_Init(void)
{
#if sns_ultrasonic_USEEEPROM==1
	if (EEDATA_OK)
	{
		//Use stored data to set initial values for the module
		sns_ultrasonic_ReportInterval = eeprom_read_byte(EEDATA.reportInterval);
		sns_ultrasonic_BottomLevel = eeprom_read_word(EEDATA16.BottomLevel);
		sns_ultrasonic_TopLevel = eeprom_read_word(EEDATA16.TopLevel);
		sns_ultrasonic_Mode = eeprom_read_byte(EEDATA.mode);
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
		eeprom_write_byte_crc(EEDATA.reportInterval, sns_ultrasonic_DEFAULT_REPORTINTERVAL, WITHOUT_CRC);
		eeprom_write_word_crc(EEDATA16.BottomLevel, sns_ultrasonic_BottomLevel, WITHOUT_CRC);
		eeprom_write_word_crc(EEDATA16.TopLevel, sns_ultrasonic_TopLevel, WITHOUT_CRC);
		eeprom_write_byte_crc(EEDATA.mode, sns_ultrasonic_Mode, WITHOUT_CRC);
		EEDATA_UPDATE_CRC;
	}
#endif
	gpio_set_pin(sns_ultrasonic_Trigger_PIN);
	gpio_set_out(sns_ultrasonic_Trigger_PIN);
	gpio_set_pin(sns_ultrasonic_Trigger_PIN);
	gpio_set_in(EXP_D);	//Set trigg input to input
	Timer_SetTimeout(sns_ultrasonic_REPORT_TIMER, sns_ultrasonic_ReportInterval*1000 , TimerTypeFreeRunning, 0);
	Timer_SetTimeout(sns_ultrasonic_MEASURE_TIMER, 10 , TimerTypeOneShot, &StartMeasure_callback);
	TCCR1B=0xC3u;	//prescaler=64, input noice reduction and trigg on rising edge
	TIMSK1 |= 0x20u; //enable input capture interrupt
}

void sns_ultrasonic_Process(void)
{
	static uint16_t lastvalue = 0;
	///TODO: Stuff that needs doing is done here
	if (Timer_Expired(sns_ultrasonic_REPORT_TIMER)) {
	  uint16_t temp;
		switch (sns_ultrasonic_Mode) {
		case CAN_MODULE_ENUM_ULTRASONIC_ULTASONICCONFIG_SENSORMODE_LENGTH:
			
			temp = (uint16_t)((float)sns_ultrasonic_Measurement*1.36f);
			//temp = lastCaptures[3];
			StdCan_Msg_t txMsg;
			StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
			StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
			txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_ULTRASONIC;
			txMsg.Header.ModuleId = sns_ultrasonic_ID;
			txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_DISTANCE;
			txMsg.Length = 3;
			txMsg.Data[0] = 0;
			txMsg.Data[1] = (temp>>8)&0xff;
			txMsg.Data[2] = (temp)&0xff;
			StdCan_Put(&txMsg);
			break;
		case CAN_MODULE_ENUM_ULTRASONIC_ULTASONICCONFIG_SENSORMODE_PERCENT:
			temp = (uint16_t)((float)sns_ultrasonic_Measurement*1.36f);
			if (temp > sns_ultrasonic_BottomLevel && temp < sns_ultrasonic_TopLevel) {
				temp = temp - sns_ultrasonic_BottomLevel;
				temp = (uint16_t)(((float)temp/(sns_ultrasonic_TopLevel-sns_ultrasonic_BottomLevel))*10000);
				StdCan_Msg_t txMsg;
				StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
				StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
				txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_ULTRASONIC;
				txMsg.Header.ModuleId = sns_ultrasonic_ID;
				txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_PERCENT;
				txMsg.Length = 3;
				txMsg.Data[0] = 0;
				txMsg.Data[1] = (temp>>8)&0xff;
				txMsg.Data[2] = (temp)&0xff;
				StdCan_Put(&txMsg);
			}
			break;
		}   
	}
	if (sns_ultrasonic_Measurement_flag == 1) {
	    sns_ultrasonic_Measurement_flag = 0;
	    if (lastvalue > (sns_ultrasonic_Measurement + 5) || lastvalue < (sns_ultrasonic_Measurement - 5)) {
	        uint16_t temp;
	      lastvalue = sns_ultrasonic_Measurement;
	      switch (sns_ultrasonic_Mode) {
		case CAN_MODULE_ENUM_ULTRASONIC_ULTASONICCONFIG_SENSORMODE_LENGTH:
			
			temp = (uint16_t)((float)sns_ultrasonic_Measurement*1.36f);
			//temp = lastCaptures[3];
			StdCan_Msg_t txMsg;
			StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
			StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
			txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_ULTRASONIC;
			txMsg.Header.ModuleId = sns_ultrasonic_ID;
			txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_DISTANCE;
			txMsg.Length = 3;
			txMsg.Data[0] = 0;
			txMsg.Data[1] = (temp>>8)&0xff;
			txMsg.Data[2] = (temp)&0xff;
			StdCan_Put(&txMsg);
			break;
		case CAN_MODULE_ENUM_ULTRASONIC_ULTASONICCONFIG_SENSORMODE_PERCENT:
			temp = (uint16_t)((float)sns_ultrasonic_Measurement*1.36f);
			if (temp > sns_ultrasonic_BottomLevel && temp < sns_ultrasonic_TopLevel) {
				temp = temp - sns_ultrasonic_BottomLevel;
				temp = (uint16_t)(((float)temp/(sns_ultrasonic_TopLevel-sns_ultrasonic_BottomLevel))*10000);
				StdCan_Msg_t txMsg;
				StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
				StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
				txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_ULTRASONIC;
				txMsg.Header.ModuleId = sns_ultrasonic_ID;
				txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_PERCENT;
				txMsg.Length = 3;
				txMsg.Data[0] = 0;
				txMsg.Data[1] = (temp>>8)&0xff;
				txMsg.Data[2] = (temp)&0xff;
				StdCan_Put(&txMsg);
			}
			break;
	      }
	    }
	}
}

void sns_ultrasonic_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_ULTRASONIC &&
		rxMsg->Header.ModuleId == sns_ultrasonic_ID)
	{
	  	StdCan_Msg_t txMsg;
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL:
			if (rxMsg->Length > 0)
			{
				sns_ultrasonic_ReportInterval = rxMsg->Data[0];
				#if sns_ultrasonic_USEEEPROM==1
				eeprom_write_byte_crc(EEDATA.reportInterval, sns_ultrasonic_ReportInterval, WITHOUT_CRC);
				EEDATA_UPDATE_CRC;
				#endif
				Timer_SetTimeout(sns_ultrasonic_REPORT_TIMER, sns_ultrasonic_ReportInterval*1000 , TimerTypeFreeRunning, 0);
			}
			StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
			StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
			txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_ULTRASONIC;
			txMsg.Header.ModuleId = sns_ultrasonic_ID;
			txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL;
			txMsg.Length = 1;

			txMsg.Data[0] = sns_ultrasonic_ReportInterval;

			StdCan_Put(&txMsg);
			break;
		case CAN_MODULE_CMD_ULTRASONIC_ULTASONICCONFIG:
			if (rxMsg->Length > 5)
			{
				sns_ultrasonic_BottomLevel = ((uint16_t)rxMsg->Data[1]<<8) + rxMsg->Data[2];
				sns_ultrasonic_TopLevel = ((uint16_t)rxMsg->Data[3]<<8) + rxMsg->Data[4];
				sns_ultrasonic_Mode = rxMsg->Data[5];
				if (sns_ultrasonic_Mode != CAN_MODULE_ENUM_ULTRASONIC_ULTASONICCONFIG_SENSORMODE_LENGTH && sns_ultrasonic_Mode != CAN_MODULE_ENUM_ULTRASONIC_ULTASONICCONFIG_SENSORMODE_PERCENT) {
					sns_ultrasonic_Mode = CAN_MODULE_ENUM_ULTRASONIC_ULTASONICCONFIG_SENSORMODE_LENGTH;
				}
				#if sns_ultrasonic_USEEEPROM==1
				eeprom_write_word_crc(EEDATA16.BottomLevel, sns_ultrasonic_BottomLevel, WITHOUT_CRC);
				eeprom_write_word_crc(EEDATA16.TopLevel, sns_ultrasonic_TopLevel, WITHOUT_CRC);
				eeprom_write_byte_crc(EEDATA.mode, sns_ultrasonic_Mode, WITHOUT_CRC);
				EEDATA_UPDATE_CRC;
				#endif
			}
			StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
			StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
			txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_ULTRASONIC;
			txMsg.Header.ModuleId = sns_ultrasonic_ID;
			txMsg.Header.Command = CAN_MODULE_CMD_ULTRASONIC_ULTASONICCONFIG;
			txMsg.Length = 6;

			txMsg.Data[0] = 0u;
			txMsg.Data[1] = (uint8_t)((sns_ultrasonic_BottomLevel>>8)&0xff);
			txMsg.Data[2] = (uint8_t)(sns_ultrasonic_BottomLevel&0xff);
			txMsg.Data[3] = (uint8_t)((sns_ultrasonic_TopLevel>>8)&0xff);;
			txMsg.Data[4] = (uint8_t)(sns_ultrasonic_TopLevel&0xff);
			txMsg.Data[5] = sns_ultrasonic_Mode;
			StdCan_Put(&txMsg);
			break;
		}
	}
}

void sns_ultrasonic_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_ULTRASONIC;
	txMsg.Header.ModuleId = sns_ultrasonic_ID;
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
