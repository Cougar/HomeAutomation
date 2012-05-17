
#include "act_SlowPWM.h"

#if act_SlowPWM_USEEEPROM==1
#include "act_SlowPWM_eeprom.h"
struct eeprom_act_SlowPWM EEMEM eeprom_act_SlowPWM = 
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file. 
		0xABcd,	// x
		0x1234,	// y
		0x00,
		0x14	//Reportinteval 20 sec.
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 
#endif

uint8_t pwmDefaultState,pwmDefaultStartState,act_SlowPWM_ReportInterval;
uint16_t pwmDefaultValue;
uint16_t pwmPeriod, pwmValue;
uint8_t pwmStatus;

#define PWM_LOW 0
#define PWM_HIGH 1
#define PWM_OFF 2
void updatePWM_callback(uint8_t timer) {
	//pwmValue = DEFAULT_PWM_VALUE;
StdCan_Msg_t txMsg;
	if (pwmStatus == PWM_HIGH) { //Pwm output is low
		gpio_clr_pin(PWM_PIN);	//Sets low
		pwmStatus = PWM_LOW;
		Timer_SetTimeout(timer, pwmValue, TimerTypeOneShot, &updatePWM_callback);

/*
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_SLOWPWM;
		txMsg.Header.ModuleId = act_SlowPWM_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL;
		txMsg.Length = 1;

		txMsg.Data[0] = 10;
		StdCan_Put(&txMsg);
		*/
	} else if (pwmStatus == PWM_LOW){ //Pwm output is high
		gpio_set_pin(PWM_PIN);	//Sets high
		pwmStatus = PWM_HIGH;
		Timer_SetTimeout(timer, pwmPeriod - pwmValue, TimerTypeOneShot, &updatePWM_callback);
/*
StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_SLOWPWM;
		txMsg.Header.ModuleId = act_SlowPWM_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL;
		txMsg.Length = 1;

		txMsg.Data[0] = 20;
		StdCan_Put(&txMsg);
*/	} else	//PWM_OFF
	{
	  if (pwmDefaultState == CAN_MODULE_ENUM_SLOWPWM_CONFIG_DEFAULTSTATE_LOW)
	  {
	    gpio_set_out(PWM_PIN);
	    gpio_clr_pin(PWM_PIN);
	  } else if (pwmDefaultState == CAN_MODULE_ENUM_SLOWPWM_CONFIG_DEFAULTSTATE_HIGH)
	  {
	    gpio_set_out(PWM_PIN);
	    gpio_set_pin(PWM_PIN);
	  } else if (pwmDefaultState == CAN_MODULE_ENUM_SLOWPWM_CONFIG_DEFAULTSTATE_TRISTATE)
	  {
	    gpio_set_in(PWM_PIN);
	    gpio_clr_pin(PWM_PIN);
	  }
	}
}


void pwmInit(void) {
	if (pwmDefaultState == CAN_MODULE_ENUM_SLOWPWM_CONFIG_DEFAULTSTATE_LOW)
	  {
	    gpio_set_out(PWM_PIN);
	    gpio_clr_pin(PWM_PIN);
	  } else if (pwmDefaultState == CAN_MODULE_ENUM_SLOWPWM_CONFIG_DEFAULTSTATE_HIGH)
	  {
	    gpio_set_out(PWM_PIN);
	    gpio_set_pin(PWM_PIN);
	  } else if (pwmDefaultState == CAN_MODULE_ENUM_SLOWPWM_CONFIG_DEFAULTSTATE_TRISTATE)
	  {
	    gpio_set_in(PWM_PIN);
	    gpio_clr_pin(PWM_PIN);
	  }

	pwmValue = pwmDefaultValue;
	pwmStatus = PWM_OFF;
	if (pwmDefaultStartState==CAN_MODULE_ENUM_SLOWPWM_CONFIG_STARTUPSTATE_ON) {
	    Timer_SetTimeout(act_SlowPWM_TIMER, pwmValue, TimerTypeOneShot, &updatePWM_callback);
	    pwmStatus = PWM_LOW;
	    gpio_set_out(PWM_PIN);
	    gpio_clr_pin(PWM_PIN);
	}
}

void act_SlowPWM_Init(void)
{
#if act_SlowPWM_USEEEPROM==1
	if (EEDATA_OK)
	{
	  ;
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
		eeprom_write_word_crc(EEDATA16.PwmPeriod, DEFAULT_PWM_PERIOD , WITHOUT_CRC);
		eeprom_write_word_crc(EEDATA16.defaultPwmValue, DEFAULT_PWM_VALUE , WITHOUT_CRC);
		eeprom_write_byte_crc(EEDATA.defaultStates, 0xa0 , WITHOUT_CRC);
		eeprom_write_byte_crc(EEDATA.ReportInterval, 0x14 , WITHOUT_CRC);
		EEDATA_UPDATE_CRC;
	}
	pwmDefaultValue = eeprom_read_word(EEDATA16.defaultPwmValue);
	pwmPeriod = eeprom_read_word(EEDATA16.PwmPeriod);
	pwmDefaultState = (eeprom_read_byte(EEDATA.defaultStates)>>6) & 0x03;
	pwmDefaultStartState = ((eeprom_read_byte(EEDATA.defaultStates)>>5) & 0x01);
	act_SlowPWM_ReportInterval = (eeprom_read_byte(EEDATA.ReportInterval));
	Timer_SetTimeout(act_SlowPWM_SEND_TIMER, act_SlowPWM_ReportInterval*1000 , TimerTypeFreeRunning, 0);
#endif  
	///TODO: Initialize hardware etc here
	pwmInit();
}

void act_SlowPWM_Process(void)
{
	///TODO: Stuff that needs doing is done here
	if (Timer_Expired(act_SlowPWM_SEND_TIMER)) {
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_SLOWPWM;
		txMsg.Header.ModuleId = act_SlowPWM_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_PWM;
		txMsg.Length = 3;
		txMsg.Data[0] = PWMSENSORID;
		txMsg.Data[1] = (pwmValue>>8)&0xff;
		txMsg.Data[2] = (pwmValue)&0xff;
		StdCan_Put(&txMsg);
	}
}

void act_SlowPWM_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
			StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
			rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_SLOWPWM &&
			rxMsg->Header.ModuleId == act_SlowPWM_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_PHYSICAL_PWM:
			if (rxMsg->Data[0] == PWMSENSORID)
			{
				if (rxMsg->Length == 3)
				{
					pwmValue= (rxMsg->Data[1]<<8) + rxMsg->Data[2];
					if (pwmValue > pwmPeriod)
						pwmValue = pwmPeriod;
					Timer_SetTimeout(act_SlowPWM_TIMER, pwmValue, TimerTypeOneShot, &updatePWM_callback);
					pwmStatus = PWM_LOW;
					gpio_set_out(PWM_PIN);
					gpio_clr_pin(PWM_PIN);
				} else
				{	
					rxMsg->Data[1] = (pwmValue>>8)&0xff;
					rxMsg->Data[2] = (pwmValue)&0xff;
					StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
					rxMsg->Length = 3;
					while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
				}
			}
		break;
		case CAN_MODULE_CMD_SLOWPWM_CONFIG:
			if (rxMsg->Length == 5)
			{
				eeprom_write_word_crc(EEDATA16.PwmPeriod, (rxMsg->Data[0]<<8) + rxMsg->Data[1] , WITHOUT_CRC);
				eeprom_write_word_crc(EEDATA16.defaultPwmValue, (rxMsg->Data[2]<<8) + rxMsg->Data[3] , WITHOUT_CRC);
				eeprom_write_byte_crc(EEDATA.defaultStates, rxMsg->Data[2] , WITH_CRC);
				pwmDefaultValue = eeprom_read_word(EEDATA16.defaultPwmValue);
				pwmPeriod = eeprom_read_word(EEDATA16.PwmPeriod);
				pwmDefaultState = (eeprom_read_byte(EEDATA.defaultStates)>>6) & 0x03;
				pwmDefaultStartState = ((eeprom_read_byte(EEDATA.defaultStates)>>5) & 0x01);
			} else
			{
				rxMsg->Data[0] = (eeprom_read_word(EEDATA16.PwmPeriod)>>8)&0xff;
				rxMsg->Data[1] = (eeprom_read_word(EEDATA16.PwmPeriod))&0xff;
				rxMsg->Data[2] = (eeprom_read_word(EEDATA16.defaultPwmValue)>>8)&0xff;
				rxMsg->Data[3] = (eeprom_read_word(EEDATA16.defaultPwmValue))&0xff;
				rxMsg->Data[4] = eeprom_read_byte(EEDATA.defaultStates);
				StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
				rxMsg->Length = 5;
				while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
			}
		break;
		
		case CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL:
		if (rxMsg->Length > 0)
		{
			act_SlowPWM_ReportInterval = rxMsg->Data[0];
			eeprom_write_byte_crc(EEDATA.ReportInterval, act_SlowPWM_ReportInterval , WITH_CRC);
			Timer_SetTimeout(act_SlowPWM_SEND_TIMER, act_SlowPWM_ReportInterval*1000 , TimerTypeFreeRunning, 0);
		}

		StdCan_Msg_t txMsg;

		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_SLOWPWM;
		txMsg.Header.ModuleId = act_SlowPWM_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL;
		txMsg.Length = 1;

		txMsg.Data[0] = act_SlowPWM_ReportInterval;
		StdCan_Put(&txMsg);
		break;
		}
	}

}

void act_SlowPWM_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_SLOWPWM; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = act_SlowPWM_ID;
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
