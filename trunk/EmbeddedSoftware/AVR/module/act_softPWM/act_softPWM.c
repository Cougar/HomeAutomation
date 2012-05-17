
#include "act_softPWM.h"

#if (act_softPWM_PIN_0_INVERT == 0)
	#define set_pin_0_below_trigger	gpio_set_pin(act_softPWM_PIN_0)
	#define set_pin_0_above_trigger	gpio_clr_pin(act_softPWM_PIN_0)
#else
	#define set_pin_0_below_trigger	gpio_clr_pin(act_softPWM_PIN_0)
	#define set_pin_0_above_trigger	gpio_set_pin(act_softPWM_PIN_0)
#endif

#if (act_softPWM_PIN_1_INVERT == 0)
	#define set_pin_1_below_trigger	gpio_set_pin(act_softPWM_PIN_1)
	#define set_pin_1_above_trigger	gpio_clr_pin(act_softPWM_PIN_1)
#else
	#define set_pin_1_below_trigger	gpio_clr_pin(act_softPWM_PIN_1)
	#define set_pin_1_above_trigger	gpio_set_pin(act_softPWM_PIN_1)
#endif

#if (act_softPWM_PIN_2_INVERT == 0)
	#define set_pin_2_below_trigger	gpio_set_pin(act_softPWM_PIN_2)
	#define set_pin_2_above_trigger	gpio_clr_pin(act_softPWM_PIN_2)
#else
	#define set_pin_2_below_trigger	gpio_clr_pin(act_softPWM_PIN_2)
	#define set_pin_2_above_trigger	gpio_set_pin(act_softPWM_PIN_2)
#endif

#if (act_softPWM_PIN_3_INVERT == 0)
	#define set_pin_3_below_trigger	gpio_set_pin(act_softPWM_PIN_3)
	#define set_pin_3_above_trigger	gpio_clr_pin(act_softPWM_PIN_3)
#else
	#define set_pin_3_below_trigger	gpio_clr_pin(act_softPWM_PIN_3)
	#define set_pin_3_above_trigger	gpio_set_pin(act_softPWM_PIN_3)
#endif

#if (act_softPWM_PIN_4_INVERT == 0)
	#define set_pin_4_below_trigger	gpio_set_pin(act_softPWM_PIN_4)
	#define set_pin_4_above_trigger	gpio_clr_pin(act_softPWM_PIN_4)
#else
	#define set_pin_4_below_trigger	gpio_clr_pin(act_softPWM_PIN_4)
	#define set_pin_4_above_trigger	gpio_set_pin(act_softPWM_PIN_4)
#endif

#if (act_softPWM_PIN_5_INVERT == 0)
	#define set_pin_5_below_trigger	gpio_set_pin(act_softPWM_PIN_5)
	#define set_pin_5_above_trigger	gpio_clr_pin(act_softPWM_PIN_5)
#else
	#define set_pin_5_below_trigger	gpio_clr_pin(act_softPWM_PIN_5)
	#define set_pin_5_above_trigger	gpio_set_pin(act_softPWM_PIN_5)
#endif

#if (act_softPWM_PIN_6_INVERT == 0)
	#define set_pin_6_below_trigger	gpio_set_pin(act_softPWM_PIN_6)
	#define set_pin_6_above_trigger	gpio_clr_pin(act_softPWM_PIN_6)
#else
	#define set_pin_6_below_trigger	gpio_clr_pin(act_softPWM_PIN_6)
	#define set_pin_6_above_trigger	gpio_set_pin(act_softPWM_PIN_6)
#endif

#if (act_softPWM_PIN_7_INVERT == 0)
	#define set_pin_7_below_trigger	gpio_set_pin(act_softPWM_PIN_7)
	#define set_pin_7_above_trigger	gpio_clr_pin(act_softPWM_PIN_7)
#else
	#define set_pin_7_below_trigger	gpio_clr_pin(act_softPWM_PIN_7)
	#define set_pin_7_above_trigger	gpio_set_pin(act_softPWM_PIN_7)
#endif

static uint16_t currentTimer = 0;
static uint16_t maxTimer = 10;
static uint8_t resolution = 1;
static uint8_t act_softPWM_ReportInterval =0;
static uint8_t currentSendChannelId = 0;

#if act_softPWM_USEEEPROM==1
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

static uint16_t pwmValue[NUMBEROFCHANNELS];
static uint16_t pwmValueCAN[NUMBEROFCHANNELS];

#if act_softPWM_ACTIVATE_AUTOOFF != 0
static uint8_t offCounter[NUMBEROFCHANNELS];
#endif

void act_softPWM_Init(void)
{
#if act_softPWM_USEEEPROM==1
	//if (EEDATA_OK)
	//{
	 // ;
	//} else
	//{	//The CRC of the EEPROM is not correct, store default values and update CRC
		eeprom_write_word_crc(EEDATA16.maxTimer, 10000 , WITHOUT_CRC);
		eeprom_write_byte_crc(EEDATA.resolution, 1 , WITHOUT_CRC);
		eeprom_write_byte_crc(EEDATA.ReportInterval, 0x05 , WITHOUT_CRC);
		EEDATA_UPDATE_CRC;
	//}
	act_softPWM_ReportInterval = eeprom_read_byte(EEDATA.ReportInterval);
	maxTimer = eeprom_read_word(EEDATA16.maxTimer);
	resolution = eeprom_read_byte(EEDATA.resolution);
	Timer_SetTimeout(act_softPWM_SEND_TIMER, act_softPWM_ReportInterval*1000 , TimerTypeFreeRunning, 0);
#endif  
	///TODO: Initialize hardware etc here
uint8_t i;
for (i=0; i < NUMBEROFCHANNELS; i++) {
	pwmValue[i] = 0;
	pwmValueCAN[i] = 0;
}
#ifdef act_softPWM_PIN_0
	gpio_set_out(act_softPWM_PIN_0);
	set_pin_0_below_trigger;
#endif
#ifdef act_softPWM_PIN_1
	gpio_set_out(act_softPWM_PIN_1);
	set_pin_1_below_trigger;
#endif
#ifdef act_softPWM_PIN_2
	gpio_set_out(act_softPWM_PIN_2);
	set_pin_2_below_trigger;
#endif
#ifdef act_softPWM_PIN_3
	gpio_set_out(act_softPWM_PIN_3);
	set_pin_3_below_trigger;
#endif
#ifdef act_softPWM_PIN_4
	gpio_set_out(act_softPWM_PIN_4);
	set_pin_4_below_trigger;
#endif
#ifdef act_softPWM_PIN_5
	gpio_set_out(act_softPWM_PIN_5);
	set_pin_5_below_trigger;
#endif
#ifdef act_softPWM_PIN_6
	gpio_set_out(act_softPWM_PIN_6);
	set_pin_6_below_trigger;
#endif
#ifdef act_softPWM_PIN_7
	gpio_set_out(act_softPWM_PIN_7);
	set_pin_7_below_trigger;
#endif
	
	Timer_SetTimeout(act_softPWM_TIMER, resolution, TimerTypeFreeRunning, NULL);
}

void act_softPWM_Process(void)
{
	if (Timer_Expired(act_softPWM_TIMER)) {
		currentTimer++;
		if (currentTimer >= maxTimer) {
//printf("cleard Timer %d\n",pwmValue[0]);
			currentTimer=0;
			#ifdef act_softPWM_PIN_0
				if (pwmValue[0]!=0) {
					set_pin_0_below_trigger;
				}
			#endif
			#ifdef act_softPWM_PIN_1 
				if (pwmValue[1]!=0) {
					set_pin_1_below_trigger;
				}
			#endif
			#ifdef act_softPWM_PIN_2 
				if (pwmValue[2]!=0) {
					set_pin_2_below_trigger;
				}
			#endif
			#ifdef act_softPWM_PIN_3 
				if (pwmValue[3]!=0) {
					set_pin_3_below_trigger;
				}
			#endif
			#ifdef act_softPWM_PIN_4 
				if (pwmValue[4]!=0) {
					set_pin_4_below_trigger;
				}
			#endif
			#ifdef act_softPWM_PIN_5 
				if (pwmValue[5]!=0) {
					set_pin_5_below_trigger;
				}
			#endif
			#ifdef act_softPWM_PIN_6 
				if (pwmValue[6]!=0) {
					set_pin_6_below_trigger;
				}
			#endif
			#ifdef act_softPWM_PIN_7 
				if (pwmValue[7]!=0) {
					set_pin_7_below_trigger;
				}
			#endif
		}
		#ifdef act_softPWM_PIN_0
		if (currentTimer >= pwmValue[0] && gpio_get_state(act_softPWM_PIN_0) != act_softPWM_PIN_0_INVERT) {
			set_pin_0_above_trigger;
		}
		#endif
		#ifdef act_softPWM_PIN_1
		if (currentTimer >= pwmValue[1] && gpio_get_state(act_softPWM_PIN_1) != act_softPWM_PIN_1_INVERT) {
			set_pin_1_above_trigger;
		}
		#endif
		#ifdef act_softPWM_PIN_2
		if (currentTimer >= pwmValue[2] && gpio_get_state(act_softPWM_PIN_2) != act_softPWM_PIN_2_INVERT) {
			set_pin_2_above_trigger;
		}
		#endif
		#ifdef act_softPWM_PIN_3
		if (currentTimer >= pwmValue[3] && gpio_get_state(act_softPWM_PIN_3) != act_softPWM_PIN_3_INVERT) {
			set_pin_3_above_trigger;
		}
		#endif
		#ifdef act_softPWM_PIN_4
		if (currentTimer >= pwmValue[4] && gpio_get_state(act_softPWM_PIN_4) != act_softPWM_PIN_4_INVERT) {
			set_pin_4_above_trigger;
		}
		#endif
		#ifdef act_softPWM_PIN_5
		if (currentTimer >= pwmValue[5] && gpio_get_state(act_softPWM_PIN_5) != act_softPWM_PIN_5_INVERT) {
			set_pin_5_above_trigger;
		}
		#endif
		#ifdef act_softPWM_PIN_6
		if (currentTimer >= pwmValue[6] && gpio_get_state(act_softPWM_PIN_6) != act_softPWM_PIN_6_INVERT) {
			set_pin_6_above_trigger;
		}
		#endif
		#ifdef act_softPWM_PIN_7
		if (currentTimer >= pwmValue[7] && gpio_get_state(act_softPWM_PIN_7) != act_softPWM_PIN_7_INVERT) {
			set_pin_7_above_trigger;
		}
		#endif
	}
	if (Timer_Expired(act_softPWM_SEND_TIMER)) {
//printf("sending...\n");
		while(1)
		{
			if (0
			    #ifdef act_softPWM_PIN_0
			    || currentSendChannelId == 0
			    #endif
			    #ifdef act_softPWM_PIN_1
			    || currentSendChannelId == 1
			    #endif
			    #ifdef act_softPWM_PIN_2
			    || currentSendChannelId == 2
			    #endif
			    #ifdef act_softPWM_PIN_3
			    || currentSendChannelId == 3
			    #endif
			    #ifdef act_softPWM_PIN_4
			    || currentSendChannelId == 4
			    #endif
			    #ifdef act_softPWM_PIN_5
			    || currentSendChannelId == 5
			    #endif
			    #ifdef act_softPWM_PIN_6
			    || currentSendChannelId == 6
			    #endif
			    #ifdef act_softPWM_PIN_7
			    || currentSendChannelId == 7
			    #endif
			) {
				break;
			}
			currentSendChannelId++;
			if (currentSendChannelId >= NUMBEROFCHANNELS)
				currentSendChannelId=0;
		}
//printf(" %d\n",currentSendChannelId);
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_SOFTPWM;
		txMsg.Header.ModuleId = act_softPWM_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_PWM;
		txMsg.Length = 3;
		txMsg.Data[0] = currentSendChannelId;
		txMsg.Data[1] = (pwmValueCAN[currentSendChannelId]>>8)&0xff;
		txMsg.Data[2] = (pwmValueCAN[currentSendChannelId])&0xff;
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
//printf("sent pwm2: %d %d\n",pwmValueCAN[currentSendChannelId],currentSendChannelId); 
#if act_softPWM_ACTIVATE_AUTOOFF != 0
		offCounter[currentSendChannelId]--;
		if (offCounter[currentSendChannelId] == 0) {
			pwmValue[currentSendChannelId] = 0;
			pwmValueCAN[currentSendChannelId] = 0;
			offCounter[currentSendChannelId] = 1;
		}
#endif
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
//printf("Got can\n");
		case CAN_MODULE_CMD_PHYSICAL_PWM:
//printf("Got pwm value\n");
			if (rxMsg->Length == 3)
			{ 
//printf("3\n");
				if (rxMsg->Data[0] < NUMBEROFCHANNELS) {
					pwmValueCAN[rxMsg->Data[0]] = ((uint16_t)((rxMsg->Data[1]<<8) + rxMsg->Data[2]));
					if (pwmValueCAN[rxMsg->Data[0]] >= 10000) {
						pwmValue[rxMsg->Data[0]] = maxTimer+1;
						pwmValueCAN[rxMsg->Data[0]] = 10000;
					} else {
						pwmValue[rxMsg->Data[0]] = ((uint16_t)((((uint32_t)pwmValueCAN[rxMsg->Data[0]])*(maxTimer))/10000));
					}
					rxMsg->Data[1] = (uint8_t)(0x00ff & (pwmValueCAN[rxMsg->Data[0]]>>8));
					rxMsg->Data[2] = (uint8_t)(0x00ff & pwmValueCAN[rxMsg->Data[0]]);
					StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
					rxMsg->Length = 3;
					while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
//printf("sent pwm3: %d %d %d\n",pwmValue[rxMsg->Data[0]],maxTimer,resolution); 
					#if act_softPWM_ACTIVATE_AUTOOFF != 0
					offCounter[rxMsg->Data[0]] = act_softPWM_ACTIVATE_AUTOOFF;
					#endif
				}
			} else if (rxMsg->Length == 1) {
				rxMsg->Data[1] = (uint8_t)(0x00ff & (pwmValueCAN[rxMsg->Data[0]]>>8));
				rxMsg->Data[2] = (uint8_t)(0x00ff & pwmValueCAN[rxMsg->Data[0]]);
				StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
				rxMsg->Length = 3;
				while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
			} else {
				rxMsg->Data[0] = (8);
				rxMsg->Data[1] = (uint8_t)(0x00ff & (pwmValueCAN[0]>>8));
				rxMsg->Data[2] = (uint8_t)(0x00ff & pwmValueCAN[0]);
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

	uint32_t HwId=BIOS_GetHwId();
	txMsg.Data[0] = HwId&0xff;
	txMsg.Data[1] = (HwId>>8)&0xff;
	txMsg.Data[2] = (HwId>>16)&0xff;
	txMsg.Data[3] = (HwId>>24)&0xff;
	
	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;
	
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}
