
#include "act_hwPWM.h"

uint16_t pwmValue[4];
uint8_t sendInfo[4] = {0,0,0,0};

int8_t fadeSpeed[4] = {0,0,0,0};
uint8_t fadeSpeedFrac[4] = {0,0,0,0};
uint16_t fadeTarget[4] = {0,0,0,0};
uint8_t fadeSpeedCnt[4] = {0,0,0,0};

uint16_t demoEndValue[4] = {0,0,0,0};
uint16_t demoHighValue[4] = {0,0,0,0};
uint8_t demoState[4] = {ACT_HWPWM_DEMO_STATE_NOT_RUNNING, ACT_HWPWM_DEMO_STATE_NOT_RUNNING, ACT_HWPWM_DEMO_STATE_NOT_RUNNING, 					ACT_HWPWM_DEMO_STATE_NOT_RUNNING};

#ifdef act_hwPWM_USEEEPROM
#include "act_hwPWM_eeprom.h"
struct eeprom_act_hwPWM EEMEM eeprom_act_hwPWM = 
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file. 
		0x0000,	// ch 1
		0x0000,	// ch 2
		0x0000,	// ch 3
		0x0000	// ch 4
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 
#endif

#if act_hwPWM_ENABLE_FADE == 1
void act_hwPWM_timer_callback(uint8_t timer)
{
	uint8_t channel = 0;
	for (channel = 0; channel < 4; channel++) {
		/* Check demo states */
		if (pwmValue[channel] == fadeTarget[channel]) {
			switch (demoState[channel])
			{
			case ACT_HWPWM_DEMO_STATE_NOT_RUNNING:
			break;
			case ACT_HWPWM_DEMO_STATE_DECREASE:
				demoState[channel] = ACT_HWPWM_DEMO_STATE_INCREASE;
				fadeTarget[channel] = demoHighValue[channel];
				fadeSpeed[channel] = -fadeSpeed[channel];
			break;
			case ACT_HWPWM_DEMO_STATE_INCREASE:
				demoState[channel] = ACT_HWPWM_DEMO_STATE_GOBACK;
				fadeTarget[channel] = demoEndValue[channel];
				fadeSpeed[channel] = -fadeSpeed[channel];
			break;
			case ACT_HWPWM_DEMO_STATE_GOBACK:
				demoState[channel] = ACT_HWPWM_DEMO_STATE_NOT_RUNNING;
			break;
			}
		}
		
		/* Change the dimmerValue according to the current fading */
		fadeSpeedCnt[channel]++;
		if (fadeSpeedCnt[channel] == fadeSpeedFrac[channel] && pwmValue[channel] != (fadeTarget[channel])) 
		{
			fadeSpeedCnt[channel] = 0;
			uint16_t tempDimVal = pwmValue[channel];
			pwmValue[channel] += (fadeSpeed[channel]*39);
			if ((fadeSpeed[channel] > 0 && (pwmValue[channel] < tempDimVal || pwmValue[channel] >= (fadeTarget[channel]))) || 
				(fadeSpeed[channel] < 0 && (pwmValue[channel] > tempDimVal || pwmValue[channel] <= (fadeTarget[channel])))) 
			{
				pwmValue[channel] = (fadeTarget[channel]);
			}
			if (pwmValue[channel] > 10000) {
				pwmValue[channel] = 10000;
			}
			/* if targetvalue was reached then send the netinfo packet */
			if (pwmValue[channel] == fadeTarget[channel])
			{
				sendInfo[channel] = 1;
			}
		}
	}

}
#endif


void act_hwPWM_Init(void)
{
#ifdef act_hwPWM_USEEEPROM
	if (EEDATA_OK)
	{
	  ///TODO: Use stored data to set initial values for the module
	  pwmValue[0] = eeprom_read_word(EEDATA16.ch1);
	  pwmValue[1] = eeprom_read_word(EEDATA16.ch2);
	  pwmValue[2] = eeprom_read_word(EEDATA16.ch3);
	  pwmValue[3] = eeprom_read_word(EEDATA16.ch4);
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_word_crc(EEDATA16.ch1, 0x0000, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.ch2, 0x0000, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.ch3, 0x0000, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.ch4, 0x0000, WITHOUT_CRC);
	  EEDATA_UPDATE_CRC;
	}
#endif
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR0A = 0;
	TCCR0B = 0;

	/* set up pwm values */
	cli();
	#if act_hwPWM_CH1_COM>0
	OCR_1=(uint16_t)(pwmValue[0]*act_hwPWM_CH1_FACT)>>8;
	#endif
	#if act_hwPWM_CH2_COM>0
	OCR_2=(uint16_t)(pwmValue[1]*act_hwPWM_CH2_FACT)>>8;
	#endif
	#if act_hwPWM_CH3_COM>0
	OCR_3=(uint16_t)(pwmValue[2]*act_hwPWM_CH3_FACT)>>8;
	#endif
	#if act_hwPWM_CH4_COM>0
	OCR_4=(uint16_t)(pwmValue[3]*act_hwPWM_CH4_FACT)>>8;
	#endif
	sei();

	/* set up waveform generation mode for timer 1 */
	#if act_hwPWM_CH1_COM>0
	TCCR1A=((act_hwPWM_CH1_WGM&0x03)<<WGM00);
	TCCR1B=(((act_hwPWM_CH1_WGM>>2)&0x03)<<WGM12);
	#elif act_hwPWM_CH2_COM>0
	TCCR1A=((act_hwPWM_CH2_WGM&0x03)<<WGM00);
	TCCR1B=(((act_hwPWM_CH2_WGM>>2)&0x03)<<WGM12);
	#endif

	/* enable outputs for timer 1 */
	#if act_hwPWM_CH1_COM>0
	gpio_set_out(EXP_B);
	#endif
	#if act_hwPWM_CH2_COM>0
	gpio_set_out(EXP_C);
	#endif

	/* set up counter mode for timer 1 */
	#if act_hwPWM_CH1_COM>0 || act_hwPWM_CH2_COM>0
	if (pwmValue[0]>0)
	{
		TCCR1A|=(act_hwPWM_CH1_COM<<COM1B0);
	}
	if (pwmValue[1]>0)
	{
		TCCR1A|=(act_hwPWM_CH2_COM<<COM1A0);
	}
	ICR1 = 0xffff;
	#endif

	/* enable timer 1 */
	#if act_hwPWM_CH1_COM>0
	TCCR1B|=(act_hwPWM_CH1_CS<<CS10);
	#elif act_hwPWM_CH2_COM>0
	TCCR1B|=(act_hwPWM_CH2_CS<<CS10);
	#endif


	/* set up waveform generation mode for timer 0 */
	#if act_hwPWM_CH3_COM>0
	TCCR0A=((act_hwPWM_CH3_WGM&0x03)<<WGM00);
	TCCR0B=(((act_hwPWM_CH3_WGM>>2)&0x01)<<WGM02);
	#elif act_hwPWM_CH4_COM>0
	TCCR0A=((act_hwPWM_CH4_WGM&0x03)<<WGM00);
	TCCR0B=(((act_hwPWM_CH4_WGM>>2)&0x01)<<WGM02);
	#endif

	/* enable outputs for timer 0 */
	#if act_hwPWM_CH3_COM>0
	gpio_set_out(EXP_F);
	#endif
	#if act_hwPWM_CH4_COM>0
	gpio_set_out(EXP_G);
	#endif

	/* set up counter mode for timer 0 */
	#if act_hwPWM_CH3_COM>0 || act_hwPWM_CH4_COM>0
	if (pwmValue[2]>0)
	{
		TCCR0A|=(act_hwPWM_CH3_COM<<COM0A0);
	}
	if (pwmValue[3]>0)
	{
		TCCR0A|=(act_hwPWM_CH4_COM<<COM0B0);
	}
	#endif

	/* enable timer 0 */
	#if act_hwPWM_CH3_COM>0
	TCCR0B|=(act_hwPWM_CH3_CS<<CS00);
	#elif act_hwPWM_CH4_COM>0
	TCCR0B|=(act_hwPWM_CH4_CS<<CS00);
	#endif
	//printf("1A %x, 1B %x, 0A %x, 0B %x\n", TCCR1A, TCCR1B, TCCR0A, TCCR0B);
	
#if act_hwPWM_ENABLE_FADE == 1
	Timer_SetTimeout(act_hwPWM_FADE_TIMER, 10, TimerTypeFreeRunning, &act_hwPWM_timer_callback);
#endif
	
	/* Setup timeout for sending the status packet */
	Timer_SetTimeout(act_hwPWM_SEND_STATUS_TIMEOUT, act_hwPWM_SEND_STATUS_INTERVAL_S*1000, TimerTypeFreeRunning, 0);
	//printf("Hello world!\n");
}
uint8_t channel_to_send = 1;
void act_hwPWM_Process(void)
{
	if (Timer_Expired(act_hwPWM_SEND_STATUS_TIMEOUT))
	{
		if (channel_to_send >4) {
		  #if act_hwPWM_CH1_COM==0
		  channel_to_send = 2;
		      #if act_hwPWM_CH2_COM==0
			  channel_to_send = 3;
			  #if act_hwPWM_CH3_COM==0
			      channel_to_send = 4;
			  #else
				  channel_to_send=3;
			    #endif	
		      #else
			      channel_to_send=2;
			#endif		
		 	  
		  #else
		  	channel_to_send=1;
		  #endif		
		
		}
		sendInfo[channel_to_send-1] = 1;
		channel_to_send++;
#if act_hwPWM_CH1_COM==0
		  if (channel_to_send == 1)
		    channel_to_send++;
#endif		
#if act_hwPWM_CH2_COM==0
		  if (channel_to_send == 2)
		    channel_to_send++;
#endif	
#if act_hwPWM_CH3_COM==0
		  if (channel_to_send == 3)
		    channel_to_send++;
#endif	
#if act_hwPWM_CH4_COM==0
		  if (channel_to_send == 4)
		    channel_to_send++;
#endif	
		
		
	}
  	/* Send netinfo packet (if pwmvalue has changed, and periodically) */
	uint8_t index;
	for(index =0;index < 4; index++){
		if (pwmValue[index] > 10000) {
			pwmValue[index] = 10000;
		}
		if (sendInfo[index])
		{
			sendInfo[index] = 0;
			StdCan_Msg_t txMsg;
			StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
			StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
			txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_HWPWM;
			txMsg.Header.ModuleId = act_hwPWM_ID;
			txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_PWM;
			txMsg.Length = 3;
			txMsg.Data[0] = index;
			txMsg.Data[1] = (0xff&(pwmValue[index]>>8));
			txMsg.Data[2] = (0xff&(pwmValue[index]));
			StdCan_Put(&txMsg);
		}
	}
	
	if (Timer_Expired(act_hwPWM_STORE_VALUE_TIMEOUT))
	{
		if (pwmValue[0] != eeprom_read_word(EEDATA16.ch1))
		{
			eeprom_write_word_crc(EEDATA16.ch1, pwmValue[0], WITH_CRC);
		}
		if (pwmValue[1] != eeprom_read_word(EEDATA16.ch2))
		{
			eeprom_write_word_crc(EEDATA16.ch2, pwmValue[1], WITH_CRC);
		}
		if (pwmValue[2] != eeprom_read_word(EEDATA16.ch3))
		{
			eeprom_write_word_crc(EEDATA16.ch3, pwmValue[2], WITH_CRC);
		}
		if (pwmValue[3] != eeprom_read_word(EEDATA16.ch4))
		{
			eeprom_write_word_crc(EEDATA16.ch4, pwmValue[3], WITH_CRC);
		}
	}
					
#if act_hwPWM_CH1_COM>0
	if (OCR_1 != (uint16_t)(pwmValue[0]*act_hwPWM_CH1_FACT)>>8) {
		cli();	
		OCR_1=(uint16_t)(pwmValue[0]*act_hwPWM_CH1_FACT)>>8;
		if (OCR_1==0)
		{
			TCCR1A&=~((1<<COM1B0)|(1<<COM1B1));
		}
		else
		{
			TCCR1A|=(act_hwPWM_CH1_COM<<COM1B0);
		}
		sei();
		Timer_SetTimeout(act_hwPWM_STORE_VALUE_TIMEOUT, act_hwPWM_STORE_VALUE_TIMEOUT_TIME*1000, TimerTypeOneShot, 0);
	}
#endif
#if act_hwPWM_CH2_COM>0
	if (OCR_2 != (uint16_t)((uint32_t)pwmValue[1]*act_hwPWM_CH2_FACT)>>8) {
		cli();	
		OCR_2=(uint16_t)(((uint32_t)pwmValue[1]*act_hwPWM_CH2_FACT)>>8);
		if (OCR_2==0)
		{
			TCCR1A&=~((1<<COM1A0)|(1<<COM1A1));
		}
		else
		{
			TCCR1A|=(act_hwPWM_CH2_COM<<COM1A0);
		}
		sei();
		Timer_SetTimeout(act_hwPWM_STORE_VALUE_TIMEOUT, act_hwPWM_STORE_VALUE_TIMEOUT_TIME*1000, TimerTypeOneShot, 0);
	}
#endif
#if act_hwPWM_CH3_COM>0
	if (OCR_3 != (uint16_t)(pwmValue[2]*act_hwPWM_CH3_FACT)>>8) {
		cli();	
		OCR_3=(uint16_t)(pwmValue[2]*act_hwPWM_CH3_FACT)>>8;
		if (OCR_3==0)
		{
			TCCR0A &= ~((1<<COM0A0)|(1<<COM0A1));
		}
		else
		{
			TCCR0A|=(act_hwPWM_CH3_COM<<COM0A0);
		}
		sei();
		Timer_SetTimeout(act_hwPWM_STORE_VALUE_TIMEOUT, act_hwPWM_STORE_VALUE_TIMEOUT_TIME*1000, TimerTypeOneShot, 0);
	}
#endif
#if act_hwPWM_CH4_COM>0
	if (OCR_4 != (uint16_t)(pwmValue[3]*act_hwPWM_CH4_FACT)>>8) {
		cli();	
		OCR_4=(uint16_t)(pwmValue[3]*act_hwPWM_CH4_FACT)>>8;
		if (OCR_4==0)
		{
			TCCR0A &= ~((1<<COM0B0)|(1<<COM0B1));
		}
		else
		{
			TCCR0A|=(act_hwPWM_CH4_COM<<COM0B0);
		}
		sei();
		Timer_SetTimeout(act_hwPWM_STORE_VALUE_TIMEOUT, act_hwPWM_STORE_VALUE_TIMEOUT_TIME*1000, TimerTypeOneShot, 0);
	}
#endif
}

void act_hwPWM_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_HWPWM &&
		rxMsg->Header.ModuleId == act_hwPWM_ID)
	{
		uint8_t channel = 0;
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_PHYSICAL_PWM:
			if (rxMsg->Length == 3) {
				channel = rxMsg->Data[0];
				pwmValue[channel] = (rxMsg->Data[1]<<8)+(rxMsg->Data[2]);
			}
			
		break;
#if act_hwPWM_ENABLE_FADE == 1
		case CAN_MODULE_CMD_HWPWM_DEMO:		/* Demo(channel, speed, steps) */
			if (rxMsg->Length == 4) {
				channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint16_t steps = (rxMsg->Data[2]<<8)+(rxMsg->Data[3]);
				
				fadeSpeedCnt[channel] = 0;
				fadeSpeed[channel] = 0;
				if (speed == 0) {
					/* do nothing */
				} else {
					uint16_t diffToMin = pwmValue[channel] - ACT_HWPWM_MIN_DIM;
					uint16_t diffToMax = ACT_HWPWM_MAX_DIM - pwmValue[channel];
				
					demoEndValue[channel] = pwmValue[channel];
					if (diffToMin >= steps && diffToMax >= steps)
					{
						/* not close to min or max */
						fadeTarget[channel] = pwmValue[channel] - steps;
						demoHighValue[channel] = pwmValue[channel] + steps;
					}
					else if (diffToMin >= steps)
					{
						/* close to max */
						fadeTarget[channel] = pwmValue[channel] - steps - steps + diffToMax;
						demoHighValue[channel] = ACT_HWPWM_MAX_DIM;
					}
					else if (diffToMax >= steps)
					{
						/* close to min */
						fadeTarget[channel] = ACT_HWPWM_MIN_DIM;
						demoHighValue[channel] = pwmValue[channel] + steps + steps - diffToMin;
					}
					demoState[channel] = ACT_HWPWM_DEMO_STATE_DECREASE;
				
					if ((speed&0x80) == 0x80) {
						fadeSpeed[channel] = (speed&0x7f)+1;
						fadeSpeedFrac[channel] = 1;
					} else {
						fadeSpeed[channel] = 1;
						fadeSpeedFrac[channel] = 0x80-(speed&0x7f);
					}
					if (fadeTarget[channel] <= pwmValue[channel]) {
						fadeSpeed[channel] = -fadeSpeed[channel];
					}
				}
			}
		break;
		
		case CAN_MODULE_CMD_HWPWM_START_FADE:	/* StartFade(channel, speed, direction) */
			if (rxMsg->Length == 3) {
				
				channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint8_t direction = rxMsg->Data[2];
				demoState[channel] = ACT_HWPWM_DEMO_STATE_NOT_RUNNING;
				fadeSpeedCnt[channel] = 0;
				fadeSpeed[channel] = 0;
				uint16_t endValue = 0;
				if (direction == CAN_MODULE_ENUM_HWPWM_START_FADE_DIRECTION_INCREASE) {
					endValue = ACT_HWPWM_MAX_DIM;
				} else if (direction == CAN_MODULE_ENUM_HWPWM_START_FADE_DIRECTION_DECREASE) {
					endValue = ACT_HWPWM_MIN_DIM;
				}
					
				if (speed == 0) {
					pwmValue[channel] = endValue;	/* set dimmer value immediately */
					sendInfo[channel] = 1;		/* send netinfo with the current dimmervalue*/
				} else {
					fadeTarget[channel] = endValue;
					if (fadeTarget[channel] != pwmValue[channel]) {
						if ((speed&0x80) == 0x80) {
							fadeSpeed[channel] = (speed&0x7f)+1;
							fadeSpeedFrac[channel] = 1;
						} else {
							fadeSpeed[channel] = 1;
							fadeSpeedFrac[channel] = 0x80-(speed&0x7f);
						}
						if (fadeTarget[channel] < pwmValue[channel]) {
							fadeSpeed[channel] = -fadeSpeed[channel];
						}
					}
				}
			}
		break;

		case CAN_MODULE_CMD_HWPWM_STOP_FADE:	/* StopFade(channel) */
			if (rxMsg->Length == 1) {
				channel = rxMsg->Data[0];
				demoState[channel] = ACT_HWPWM_DEMO_STATE_NOT_RUNNING;
				fadeSpeed[channel] = 0;
				sendInfo[channel] = 1;		/* send netinfo with the current dimmervalue*/
			}
		break;

		case CAN_MODULE_CMD_HWPWM_ABS_FADE:	/* AbsFade(channel, speed, endValue) */
			if (rxMsg->Length == 4) {
				channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint16_t endValue = (rxMsg->Data[2]<<8)+(rxMsg->Data[3]);
				demoState[channel] = ACT_HWPWM_DEMO_STATE_NOT_RUNNING;
				fadeSpeedCnt[channel] = 0;
				fadeSpeed[channel] = 0;
				if (speed == 0) {
					pwmValue[channel] = endValue;	/* set dimmer value immediately */
					sendInfo[channel] = 1;		/* send netinfo with the current dimmervalue*/
				} else {
					fadeTarget[channel] = endValue;
					if (fadeTarget[channel] != pwmValue[channel]) {
						if ((speed&0x80) == 0x80) {
							fadeSpeed[channel] = (speed&0x7f)+1;
							fadeSpeedFrac[channel] = 1;
						} else {
							fadeSpeed[channel] = 1;
							fadeSpeedFrac[channel] = 0x80-(speed&0x7f);
						}
						if (fadeTarget[channel] < pwmValue[channel]) {
							fadeSpeed[channel] = -fadeSpeed[channel];
						}
					}
				}
				//printf("abs fade %d %d %d!\n",fadeTarget[channel], endValue ,pwmValue[channel] );
			}
		break;

		case CAN_MODULE_CMD_HWPWM_REL_FADE:	/* RelFade(channel, speed, direction, steps) */
			if (rxMsg->Length == 5) {
				
				channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint8_t direction = rxMsg->Data[2];
				uint16_t steps = (rxMsg->Data[3]<<8)+(rxMsg->Data[4]);
				
				demoState[channel] = ACT_HWPWM_DEMO_STATE_NOT_RUNNING;
				fadeSpeedCnt[channel] = 0;
				fadeSpeed[channel] = 0;
				uint16_t tempDimVal = pwmValue[channel];
				uint16_t tempDimVal2 = pwmValue[channel];
				if (direction == CAN_MODULE_ENUM_HWPWM_REL_FADE_DIRECTION_INCREASE) {					/* if increase */
					tempDimVal2 += steps;				/* calculate new value */
					if (tempDimVal2 < tempDimVal) {		/* make overflow test */
						tempDimVal2 = ACT_HWPWM_MAX_DIM;
					}
				} else if (direction == CAN_MODULE_ENUM_HWPWM_REL_FADE_DIRECTION_DECREASE) {			/* if decrease */
					tempDimVal2 -= steps;
					if (tempDimVal2 > tempDimVal) {
						tempDimVal2 = ACT_HWPWM_MIN_DIM;
					}
				}
				if (speed == 0) {
					pwmValue[channel] = tempDimVal2;		/* set dimmer value immediately */
					sendInfo[channel] = 1;				/* send netinfo with the current dimmervalue*/
				} else {
					fadeTarget[channel] = tempDimVal2;		/* set the fade target */
					
					if (fadeTarget[channel] != pwmValue[channel]) {
						if ((speed&0x80) == 0x80) {
							fadeSpeed[channel] = (speed&0x7f)+1;
							fadeSpeedFrac[channel] = 1;
						} else {
							fadeSpeed[channel] = 1;
							fadeSpeedFrac[channel] = 0x80-(speed&0x7f);
						}
						if (fadeTarget[channel] < pwmValue[channel]) {
							fadeSpeed[channel] = -fadeSpeed[channel];
						}
					}
				}
			}
		break;
#endif

		}
	}
}

void act_hwPWM_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_HWPWM;
	txMsg.Header.ModuleId = act_hwPWM_ID;
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
