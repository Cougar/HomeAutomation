
#include "act_hwPWM.h"
#include <avr/pgmspace.h>

uint16_t pwmValue[4];
uint8_t sendInfo[4] = {0,0,0,0};

int8_t hw_fadeSpeed[4] = {0,0,0,0};
uint8_t hw_fadeSpeedFrac[4] = {0,0,0,0};
uint16_t hw_fadeTarget[4] = {0,0,0,0};
uint8_t hw_fadeSpeedCnt[4] = {0,0,0,0};

uint16_t hw_demoEndValue[4] = {0,0,0,0};
uint16_t hw_demoHighValue[4] = {0,0,0,0};
uint8_t hw_demoState[4] = {ACT_HWPWM_DEMO_STATE_NOT_RUNNING, ACT_HWPWM_DEMO_STATE_NOT_RUNNING, ACT_HWPWM_DEMO_STATE_NOT_RUNNING, ACT_HWPWM_DEMO_STATE_NOT_RUNNING};

static const uint16_t lookuptable[257] PROGMEM = 
{ 
  0	,  4	,  8	,  12	,  17	,  21	,  25	,  30	,  34	,  39	,  //0
43	,  48	,  52	,  57	,  62	,  67	,  72	,  77	,  82	,  87	,  //10
93	,  98	,  104	,  109	, 115	,  120	,  126	,  132	,  138	,  144	,  //20
150	,  156	,  162	,  169	,  175	,  182	,  188	,  195	,  202	,  209	,  //30
216	,  223	,  230	,  237	,  245	,  252	,  260	,  267	,  275	,  283	,  //40
291	,  299	,  308	,  316	,  325	,  333	,  342	,  351	,  360	,  369	,  //50
378	,  388	,  397	,  407	,  417	,  427	,  437	,  447	,  457	,  468	,  //60
478	,  489	,  500	,  511	,  523	,  534	,  546	,  557	,  569	,  581	,  //70
594	,  606	,  619	,  632	,  645	,  658	,  671	,  685	,  698	,  712	,  //80
726	,  741	,  755	,  770	,  785	,  800	,  815	,  831	,  847	,  863	,  //90
879	,  896	,  912	,  929	,  946	,  964	,  982	,  999	,  1018	,  1036	,  //100
1055	,  1074	,  1093	,  1113	,  1132	,  1152	,  1173	,  1193	,  1214	,  1236	,  //110
1257	,  1279	,  1301	,  1324	,  1346	,  1369	,  1393	,  1417	,  1441	,  1465	,  //120
1490	,  1515	,  1540	,  1566	,  1593	,  1619	,  1646	,  1673	,  1701	,  1729	,  //130
1758	,  1787	,  1816	,  1846	,  1876	,  1907	,  1938	,  1969	,  2001	,  2033	,  //140
2066	,  2099	,  2133	,  2167	,  2202	,  2237	,  2273	,  2309	,  2346	,  2383	,  //150
2421	,  2459	,  2498	,  2538	,  2577	,  2618	,  2659	,  2701	,  2743	,  2786	,  //160
2829	,  2873	,  2918	,  2963	,  3009	,  3056	,  3103	,  3151	,  3200	,  3249	,  //170
3299	,  3350	,  3402	,  3454	,  3507	,  3560	,  3615	,  3670	,  3726	,  3783	,  //180
3840	,  3899	,  3958	,  4018	,  4079	,  4141	,  4203	,  4267	,  4331	,  4397	,  //190
4463	,  4530	,  4598	,  4667	,  4737	,  4808	,  4881	,  4954	,  5028	,  5103	,  //200
5179	,  5257	,  5335	,  5415	,  5495	,  5577	,  5660	,  5744	,  5830	,  5916	,  //210
6004	,  6093	,  6183	,  6275	,  6368	,  6462	,  6557	,  6654	,  6752	,  6852	,  //220
6953	,  7055	,  7159	,  7265	,  7371	,  7480	,  7590	,  7701	,  7814	,  7929	,  //230
8045	,  8163	,  8283	,  8404	,  8527	,  8652	,  8778	,  8906	,  9036	,  9168	,  //240
9302	,  9438	,  9575	,  9715	,  9856	,  10000,  10000			   //250
};


#if act_hwPWM_USEEEPROM==1
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
		if (pwmValue[channel] == hw_fadeTarget[channel]) {
			switch (hw_demoState[channel])
			{
			case ACT_HWPWM_DEMO_STATE_NOT_RUNNING:
			break;
			case ACT_HWPWM_DEMO_STATE_DECREASE:
				hw_demoState[channel] = ACT_HWPWM_DEMO_STATE_INCREASE;
				hw_fadeTarget[channel] = hw_demoHighValue[channel];
				hw_fadeSpeed[channel] = -hw_fadeSpeed[channel];
			break;
			case ACT_HWPWM_DEMO_STATE_INCREASE:
				hw_demoState[channel] = ACT_HWPWM_DEMO_STATE_GOBACK;
				hw_fadeTarget[channel] = hw_demoEndValue[channel];
				hw_fadeSpeed[channel] = -hw_fadeSpeed[channel];
			break;
			case ACT_HWPWM_DEMO_STATE_GOBACK:
				hw_demoState[channel] = ACT_HWPWM_DEMO_STATE_NOT_RUNNING;
			break;
			}
		}
		
		/* Change the dimmerValue according to the current fading */
		hw_fadeSpeedCnt[channel]++;
		if (hw_fadeSpeedCnt[channel] == hw_fadeSpeedFrac[channel] && pwmValue[channel] != (hw_fadeTarget[channel])) 
		{
			hw_fadeSpeedCnt[channel] = 0;
			uint16_t tempDimVal = pwmValue[channel];
			pwmValue[channel] += (hw_fadeSpeed[channel]*39);
			if ((hw_fadeSpeed[channel] > 0 && (pwmValue[channel] < tempDimVal || pwmValue[channel] >= (hw_fadeTarget[channel]))) || 
				(hw_fadeSpeed[channel] < 0 && (pwmValue[channel] > tempDimVal || pwmValue[channel] <= (hw_fadeTarget[channel])))) 
			{
				pwmValue[channel] = (hw_fadeTarget[channel]);
			}
			if (pwmValue[channel] > 10000) {
				pwmValue[channel] = 10000;
			}
			/* if targetvalue was reached then send the netinfo packet */
			if (pwmValue[channel] == hw_fadeTarget[channel])
			{
				sendInfo[channel] = 1;
			}
		}
	}

}
#endif


void act_hwPWM_Init(void)
{
#if act_hwPWM_USEEEPROM==1
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
		if (pwmValue[index] > ACT_HWPWM_MAX_DIM) {
			pwmValue[index] = ACT_HWPWM_MAX_DIM;
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
			
			//printf("CH1 PWM: %u, LED: %u, ledindex: %u, ocr: %u\n",pwmValue[1],(uint16_t)(pgm_read_word(&lookuptable[(pwmValue[1]/39)])), (pwmValue[1]/39),OCR_2);
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
# if act_hwPWM_CH1_LED>0
#  define CH1_calculation ((pgm_read_word(&lookuptable[(pwmValue[0]/39)])*act_hwPWM_CH1_FACT)>>8)
# else
#  define CH1_calculation ((pwmValue[0]*act_hwPWM_CH1_FACT)>>8)
# endif
	if (OCR_1 != (uint16_t)CH1_calculation) {
		cli();	
		OCR_1=(uint16_t)CH1_calculation;
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
		//printf("CH0 PWM: %u, LED: %u, ORG: %u\n",pwmValue[0],(uint16_t)((lookuptable[(pwmValue[0]/39)-1]*act_hwPWM_CH1_FACT)>>8), (uint16_t)((pwmValue[0]*act_hwPWM_CH1_FACT)>>8));
	}
#endif
#if act_hwPWM_CH2_COM>0
# if act_hwPWM_CH2_LED>0
#  define CH2_calculation ((pgm_read_word(&lookuptable[(pwmValue[1]/39)])*act_hwPWM_CH2_FACT)>>8)
# else
#  define CH2_calculation ((pwmValue[1]*act_hwPWM_CH2_FACT)>>8)
# endif
	if (OCR_2 != (uint16_t)CH2_calculation) {
		cli();	
		OCR_2=(uint16_t)CH2_calculation;
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
		//printf("CH1 PWM: %u, LED: %u, ORG: %u, ocr: %u\n",pwmValue[1],(uint16_t)((lookuptable[(pwmValue[1]/39)-1]*act_hwPWM_CH2_FACT)>>8), (uint16_t)((pwmValue[1]*act_hwPWM_CH2_FACT)>>8),OCR_2);
	}
#endif
#if act_hwPWM_CH3_COM>0
# if act_hwPWM_CH3_LED>0
#  define CH3_calculation ((pgm_read_word(&lookuptable[(pwmValue[2]/39)])*act_hwPWM_CH3_FACT)>>8)
# else
#  define CH3_calculation ((pwmValue[2]*act_hwPWM_CH3_FACT)>>8)
# endif
	if (OCR_3 != (uint16_t)CH3_calculation) {
		cli();	
		OCR_3=(uint16_t)CH3_calculation;
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
		//printf("CH2 PWM: %u, LED: %u, ORG: %u\n",pwmValue[2],(uint16_t)((pgm_read_word(&lookuptable[(pwmValue[2]/39)])*act_hwPWM_CH3_FACT)>>8)
, (uint16_t)((pwmValue[2]*act_hwPWM_CH3_FACT)>>8));
	}
#endif
#if act_hwPWM_CH4_COM>0
# if act_hwPWM_CH4_LED>0
#  define CH4_calculation ((pgm_read_word(&lookuptable[(pwmValue[3]/39)])*act_hwPWM_CH4_FACT)>>8)
# else
#  define CH4_calculation ((pwmValue[3]*act_hwPWM_CH4_FACT)>>8)
# endif
	if (OCR_4 != (uint16_t)CH4_calculation) {
		cli();	
		OCR_4=(uint16_t)CH4_calculation;
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
		//printf("CH3 PWM: %u, LED: %u, ORG: %u\n",pwmValue[3],(uint16_t)((lookuptable[(pwmValue[3]/39)-1]*act_hwPWM_CH4_FACT)>>8), (uint16_t)((pwmValue[3]*act_hwPWM_CH4_FACT)>>8));
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
				
				hw_fadeSpeedCnt[channel] = 0;
				hw_fadeSpeed[channel] = 0;
				if (speed == 0) {
					/* do nothing */
				} else {
					uint16_t diffToMin = pwmValue[channel] - ACT_HWPWM_MIN_DIM;
					uint16_t diffToMax = ACT_HWPWM_MAX_DIM - pwmValue[channel];
				
					hw_demoEndValue[channel] = pwmValue[channel];
					if (diffToMin >= steps && diffToMax >= steps)
					{
						/* not close to min or max */
						hw_fadeTarget[channel] = pwmValue[channel] - steps;
						hw_demoHighValue[channel] = pwmValue[channel] + steps;
					}
					else if (diffToMin >= steps)
					{
						/* close to max */
						hw_fadeTarget[channel] = pwmValue[channel] - steps - steps + diffToMax;
						hw_demoHighValue[channel] = ACT_HWPWM_MAX_DIM;
					}
					else if (diffToMax >= steps)
					{
						/* close to min */
						hw_fadeTarget[channel] = ACT_HWPWM_MIN_DIM;
						hw_demoHighValue[channel] = pwmValue[channel] + steps + steps - diffToMin;
					}
					hw_demoState[channel] = ACT_HWPWM_DEMO_STATE_DECREASE;
				
					if ((speed&0x80) == 0x80) {
						hw_fadeSpeed[channel] = (speed&0x7f)+1;
						hw_fadeSpeedFrac[channel] = 1;
					} else {
						hw_fadeSpeed[channel] = 1;
						hw_fadeSpeedFrac[channel] = 0x80-(speed&0x7f);
					}
					if (hw_fadeTarget[channel] <= pwmValue[channel]) {
						hw_fadeSpeed[channel] = -hw_fadeSpeed[channel];
					}
				}
			}
		break;
		
		case CAN_MODULE_CMD_HWPWM_START_FADE:	/* StartFade(channel, speed, direction) */
			if (rxMsg->Length == 3) {
				
				channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint8_t direction = rxMsg->Data[2];
				hw_demoState[channel] = ACT_HWPWM_DEMO_STATE_NOT_RUNNING;
				hw_fadeSpeedCnt[channel] = 0;
				hw_fadeSpeed[channel] = 0;
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
					hw_fadeTarget[channel] = endValue;
					if (hw_fadeTarget[channel] != pwmValue[channel]) {
						if ((speed&0x80) == 0x80) {
							hw_fadeSpeed[channel] = (speed&0x7f)+1;
							hw_fadeSpeedFrac[channel] = 1;
						} else {
							hw_fadeSpeed[channel] = 1;
							hw_fadeSpeedFrac[channel] = 0x80-(speed&0x7f);
						}
						if (hw_fadeTarget[channel] < pwmValue[channel]) {
							hw_fadeSpeed[channel] = -hw_fadeSpeed[channel];
						}
					}
				}
			}
		break;

		case CAN_MODULE_CMD_HWPWM_STOP_FADE:	/* StopFade(channel) */
			if (rxMsg->Length == 1) {
				channel = rxMsg->Data[0];
				hw_demoState[channel] = ACT_HWPWM_DEMO_STATE_NOT_RUNNING;
				hw_fadeSpeed[channel] = 0;
				sendInfo[channel] = 1;		/* send netinfo with the current dimmervalue*/
			}
		break;

		case CAN_MODULE_CMD_HWPWM_ABS_FADE:	/* AbsFade(channel, speed, endValue) */
			if (rxMsg->Length == 4) {
				channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint16_t endValue = (rxMsg->Data[2]<<8)+(rxMsg->Data[3]);
				hw_demoState[channel] = ACT_HWPWM_DEMO_STATE_NOT_RUNNING;
				hw_fadeSpeedCnt[channel] = 0;
				hw_fadeSpeed[channel] = 0;
				if (speed == 0) {
					pwmValue[channel] = endValue;	/* set dimmer value immediately */
					sendInfo[channel] = 1;		/* send netinfo with the current dimmervalue*/
				} else {
					hw_fadeTarget[channel] = endValue;
					if (hw_fadeTarget[channel] != pwmValue[channel]) {
						if ((speed&0x80) == 0x80) {
							hw_fadeSpeed[channel] = (speed&0x7f)+1;
							hw_fadeSpeedFrac[channel] = 1;
						} else {
							hw_fadeSpeed[channel] = 1;
							hw_fadeSpeedFrac[channel] = 0x80-(speed&0x7f);
						}
						if (hw_fadeTarget[channel] < pwmValue[channel]) {
							hw_fadeSpeed[channel] = -hw_fadeSpeed[channel];
						}
					}
				}
				//printf("abs fade %d %d %d!\n",hw_fadeTarget[channel], endValue ,pwmValue[channel] );
			}
		break;

		case CAN_MODULE_CMD_HWPWM_REL_FADE:	/* RelFade(channel, speed, direction, steps) */
			if (rxMsg->Length == 5) {
				
				channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint8_t direction = rxMsg->Data[2];
				uint16_t steps = (rxMsg->Data[3]<<8)+(rxMsg->Data[4]);
				
				hw_demoState[channel] = ACT_HWPWM_DEMO_STATE_NOT_RUNNING;
				hw_fadeSpeedCnt[channel] = 0;
				hw_fadeSpeed[channel] = 0;
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
					hw_fadeTarget[channel] = tempDimVal2;		/* set the fade target */
					
					if (hw_fadeTarget[channel] != pwmValue[channel]) {
						if ((speed&0x80) == 0x80) {
							hw_fadeSpeed[channel] = (speed&0x7f)+1;
							hw_fadeSpeedFrac[channel] = 1;
						} else {
							hw_fadeSpeed[channel] = 1;
							hw_fadeSpeedFrac[channel] = 0x80-(speed&0x7f);
						}
						if (hw_fadeTarget[channel] < pwmValue[channel]) {
							hw_fadeSpeed[channel] = -hw_fadeSpeed[channel];
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

	uint32_t HwId=BIOS_GetHwId();
	txMsg.Data[0] = HwId&0xff;
	txMsg.Data[1] = (HwId>>8)&0xff;
	txMsg.Data[2] = (HwId>>16)&0xff;
	txMsg.Data[3] = (HwId>>24)&0xff;
	
	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;

	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}
