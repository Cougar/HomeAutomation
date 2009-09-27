
#include "act_softPWM.h"


uint16_t currentTimer = 0;
uint16_t maxTimer = 10;
uint8_t	resolution = 1;
uint8_t act_softPWM_ReportInterval;
uint8_t currentSendChannelId = 0;


#ifdef act_softPWM_USEEEPROM
#include "act_softPWM_eeprom.h"
struct eeprom_act_softPWM EEMEM eeprom_act_softPWM = 
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file. 
		10000,	// x
		10,
		0x14	//Reportinteval 20 sec.
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 
#endif

uint16_t pwmValue[NUMBEROFCHANNELS];
float pwmValueFloat[NUMBEROFCHANNELS];

void act_softPWM_Init(void)
{
#ifdef act_softPWM_USEEEPROM
	if (EEDATA_OK)
	{
	  ;
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
		eeprom_write_word_crc(EEDATA16.maxTimer, 10000 , WITHOUT_CRC);
		eeprom_write_byte_crc(EEDATA.resolution, 0x10 , WITHOUT_CRC);
		eeprom_write_byte_crc(EEDATA.ReportInterval, 0x14 , WITHOUT_CRC);
		EEDATA_UPDATE_CRC;
	}
	maxTimer = eeprom_read_word(EEDATA16.maxTimer);
	resolution = eeprom_read_byte(EEDATA.resolution);
	Timer_SetTimeout(act_softPWM_SEND_TIMER, act_softPWM_ReportInterval*1000 , TimerTypeFreeRunning, 0);
#endif  
	///TODO: Initialize hardware etc here
uint8_t i;
for (i=0, i < NUMBEROFCHANNELS; i++) {
	pwmValue[i] = 0;
	pwmValueFloat[i] = 0;
}
#ifdef PIN_0
	gpio_set_out(PIN_0);
	gpio_clr_pin(PIN_0);
#endif
#ifdef PIN_1
	gpio_set_out(PIN_1);
	gpio_clr_pin(PIN_1);
#endif
#ifdef PIN_2
	gpio_set_out(PIN_2);
	gpio_clr_pin(PIN_2);
#endif
#ifdef PIN_3
	gpio_set_out(PIN_3);
	gpio_clr_pin(PIN_3);
#endif
#ifdef PIN_4
	gpio_set_out(PIN_4);
	gpio_clr_pin(PIN_4);
#endif
#ifdef PIN_5
	gpio_set_out(PIN_5);
	gpio_clr_pin(PIN_5);
#endif
#ifdef PIN_6
	gpio_set_out(PIN_6);
	gpio_clr_pin(PIN_6);
#endif
#ifdef PIN_7
	gpio_set_out(PIN_7);
	gpio_clr_pin(PIN_7);
#endif
	
	Timer_SetTimeout(act_softPWM_TIMER, resolution, TimerTypeFreeRunning, NULL);
}

void act_softPWM_Process(void)
{
	if (Timer_Expired(act_softPWM_TIMER)) {
		currentTimer++;
		if (currentTimer == maxTimer) {
			currentTimer=0;
			#ifdef PIN_0 
				gpio_set_pin(PIN_0);
			#endif
			#ifdef PIN_1 
				gpio_set_pin(PIN_1);
			#endif
			#ifdef PIN_2 
				gpio_set_pin(PIN_2);
			#endif
			#ifdef PIN_3 
				gpio_set_pin(PIN_3);
			#endif
			#ifdef PIN_4 
				gpio_set_pin(PIN_4);
			#endif
			#ifdef PIN_5 
				gpio_set_pin(PIN_5);
			#endif
			#ifdef PIN_6 
				gpio_set_pin(PIN_6);
			#endif
			#ifdef PIN_7 
				gpio_set_pin(PIN_7);
			#endif
		}
		#ifdef PIN_0
		if (currentTimer >= pwmValue[0]) {
			gpio_clr_pin(PIN_0);
		}
		#endif
		#ifdef PIN_1
		if (currentTimer >= pwmValue[1]) {
			gpio_clr_pin(PIN_1);
		}
		#endif
		#ifdef PIN_2
		if (currentTimer >= pwmValue[2]) {
			gpio_clr_pin(PIN_2);
		}
		#endif
		#ifdef PIN_3
		if (currentTimer >= pwmValue[3]) {
			gpio_clr_pin(PIN_3);
		}
		#endif
		#ifdef PIN_4
		if (currentTimer >= pwmValue[4]) {
			gpio_clr_pin(PIN_4);
		}
		#endif
		#ifdef PIN_5
		if (currentTimer >= pwmValue[5]) {
			gpio_clr_pin(PIN_5);
		}
		#endif
		#ifdef PIN_6
		if (currentTimer >= pwmValue[6]) {
			gpio_clr_pin(PIN_6);
		}
		#endif
		#ifdef PIN_7
		if (currentTimer >= pwmValue[7]) {
			gpio_clr_pin(PIN_7);
		}
		#endif
	}
	if (Timer_Expired(act_softPWM_SEND_TIMER)) {
		while(1)
		{
			if (0
			    #ifdef PIN_0
			    || currentSendChannelId == 0
			    #endif
			    #ifdef PIN_1
			    || currentSendChannelId == 1
			    #endif
			    #ifdef PIN_2
			    || currentSendChannelId == 2
			    #endif
			    #ifdef PIN_3
			    || currentSendChannelId == 3
			    #endif
			    #ifdef PIN_4
			    || currentSendChannelId == 4
			    #endif
			    #ifdef PIN_5
			    || currentSendChannelId == 5
			    #endif
			    #ifdef PIN_6
			    || currentSendChannelId == 6
			    #endif
			    #ifdef PIN_7
			    || currentSendChannelId == 7
			    #endif
			) {
				break;
			}
			currentSendChannelId++;
			if (currentSendChannelId >= NUMBEROFCHANNELS)
				currentSendChannelId=0;
		}
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_SOFTPWM;
		txMsg.Header.ModuleId = act_softPWM_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_PWM;
		txMsg.Length = 3;
		txMsg.Data[0] = currentSendChannelId;
		txMsg.Data[1] = (pwmValue[currentSendChannelId]>>8)&0xff;
		txMsg.Data[2] = (pwmValue[currentSendChannelId])&0xff;
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
		currentSendChannelId++;
		if (currentSendChannelId >= NUMBEROFCHANNELS)
			currentSendChannelId=0;
	}
}



void act_softPWM_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT && 
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_SOFTPWM && 
		rxMsg->Header.ModuleId == act_softPWM_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_PHYSICAL_PWM:
			if (rxMsg->Length == 3)
			{ 
				if (rxMsg->Data[0] < NUMBEROFCHANNELS) {
					pwmValueFloat[rxMsg->Data[0]] = ((rxMsg->Data[1]<<8) + rxMsg->Data[2])/64;
					pwmValue[rxMsg->Data[0]] = (uint16_t)(pwmValueFloat[rxMsg->Data[0]]*maxTimer/(resolution*100));
					rxMsg->Data[1] = (uint8_t)0x00ff & (((uint32_t)(pwmValueFloat[rxMsg->Data[0]]*64))>>8);
					rxMsg->Data[2] = (uint8_t)0x00ff & ((uint32_t)pwmValueFloat[rxMsg->Data[0]]*64);
					StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
					rxMsg->Length = 3;
					while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
				}
			} else if (rxMsg->Length == 1) {
				rxMsg->Data[1] = (uint8_t)0x00ff & (((uint32_t)(pwmValueFloat[rxMsg->Data[0]]*64))>>8);
				rxMsg->Data[2] = (uint8_t)0x00ff & ((uint32_t)pwmValueFloat[rxMsg->Data[0]]*64);
				StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
				rxMsg->Length = 3;
				while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
			}
			break;
		case CAN_MODULE_CMD_SOFTPWM_CONFIG:
			maxTimer = (((uint16_t)rxMsg->Data[0])<<8) + rxMsg->Data[1];
			resolution = rxMsg->Data[2];
			Timer_SetTimeout(act_softPWM_TIMER, resolution, TimerTypeFreeRunning, NULL);
			eeprom_write_word_crc(EEDATA16.maxTimer, maxTimer , WITHOUT_CRC);
			eeprom_write_byte_crc(EEDATA.resolution, resolution , WITHOUT_CRC);
			EEDATA_UPDATE_CRC;
			StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
			while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
			break;
		case CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL:
			if (rxMsg->Length > 0)
			{
				act_softPWM_ReportInterval = rxMsg->Data[0];
				eeprom_write_byte_crc(EEDATA.ReportInterval, act_softPWM_ReportInterval , WITH_CRC);
				Timer_SetTimeout(act_softPWM_SEND_TIMER, act_softPWM_ReportInterval*1000 , TimerTypeFreeRunning, 0);
			}

			StdCan_Msg_t txMsg;

			StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
			StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
			txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_SOFTPWM;
			txMsg.Header.ModuleId = act_softPWM_ID;
			txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_REPORT_INTERVAL;
			txMsg.Length = 1;
			txMsg.Data[0] = act_softPWM_ReportInterval;
			while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
			break;
		}
	}
}

void act_softPWM_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_SOFTPWM; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = act_softPWM_ID;
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
