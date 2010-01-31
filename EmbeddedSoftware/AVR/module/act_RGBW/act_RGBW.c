
#include "act_RGBW.h"

#define RED 0
#define GREEN 1
#define BLUE 2
#define WHITE 3
#define ALGO_FREQ		5
#define ADC_W	5
#define ADC_R	4
#define ADC_G	2
#define ADC_B	0

#define OCR_W	OCR1B
#define OCR_R	OCR1A
#define OCR_G	OCR0A
#define OCR_B	OCR0B

#define MAX_CURRENT_W	250UL
#define MAX_ADC_W		MAX_CURRENT_W*4UL*1024UL/5000UL

#define MAX_CURRENT_R	50UL
#define MAX_ADC_R		MAX_CURRENT_R*4UL*3UL*1024UL/5000UL

#define MAX_CURRENT_G	50UL
#define MAX_ADC_G		MAX_CURRENT_G*4UL*3UL*1024UL/5000UL

#define MAX_CURRENT_B	50UL
#define MAX_ADC_B		MAX_CURRENT_B*4UL*3UL*1024UL/5000UL
uint16_t pwmValue[4];
uint16_t adcValue[4];
uint8_t sendInfo[4] = {0,0,0,0};

int8_t fadeSpeed[4] = {0,0,0,0};
uint8_t fadeSpeedFrac[4] = {0,0,0,0};
uint16_t fadeTarget[4] = {0,0,0,0};
uint8_t fadeSpeedCnt[4] = {0,0,0,0};

uint16_t demoEndValue[4] = {0,0,0,0};
uint16_t demoHighValue[4] = {0,0,0,0};
uint8_t demoState[4] = {ACT_RGBW_DEMO_STATE_NOT_RUNNING, ACT_RGBW_DEMO_STATE_NOT_RUNNING, ACT_RGBW_DEMO_STATE_NOT_RUNNING, 					ACT_RGBW_DEMO_STATE_NOT_RUNNING};

uint8_t channel_to_send = 1;
void calculatePWM(uint8_t timer);
#ifdef act_RGBW_USEEEPROM
#include "act_RGBW_eeprom.h"
struct eeprom_act_RGBW EEMEM eeprom_act_RGBW = 
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

#if act_RGBW_ENABLE_FADE == 1
void act_RGBW_timer_callback(uint8_t timer)
{
	uint8_t channel = 0;
	for (channel = 0; channel < 4; channel++) {
		/* Check demo states */
		if (pwmValue[channel] == fadeTarget[channel]) {
			switch (demoState[channel])
			{
			case ACT_RGBW_DEMO_STATE_NOT_RUNNING:
			break;
			case ACT_RGBW_DEMO_STATE_DECREASE:
				demoState[channel] = ACT_RGBW_DEMO_STATE_INCREASE;
				fadeTarget[channel] = demoHighValue[channel];
				fadeSpeed[channel] = -fadeSpeed[channel];
			break;
			case ACT_RGBW_DEMO_STATE_INCREASE:
				demoState[channel] = ACT_RGBW_DEMO_STATE_GOBACK;
				fadeTarget[channel] = demoEndValue[channel];
				fadeSpeed[channel] = -fadeSpeed[channel];
			break;
			case ACT_RGBW_DEMO_STATE_GOBACK:
				demoState[channel] = ACT_RGBW_DEMO_STATE_NOT_RUNNING;
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
			/* if targetvalue was reached then send the netinfo packet */
			if (pwmValue[channel] == fadeTarget[channel])
			{
				sendInfo[channel] = 1;
			}
		}
	}

}
#endif


void act_RGBW_Init(void)
{
#ifdef act_RGBW_USEEEPROM
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
	ADC_Init();
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR0A = 0;
	TCCR0B = 0;

	/* set up pwm values */
	/*
	cli();
	#if act_RGBW_CH1_COM>0
	OCR_1=(uint16_t)(pwmValue[0]*act_RGBW_CH1_FACT)>>8;
	#endif
	#if act_RGBW_CH2_COM>0
	OCR_2=(uint16_t)(pwmValue[1]*act_RGBW_CH2_FACT)>>8;
	#endif
	#if act_RGBW_CH3_COM>0
	OCR_3=(uint16_t)(pwmValue[2]*act_RGBW_CH3_FACT)>>8;
	#endif
	#if act_RGBW_CH4_COM>0
	OCR_4=(uint16_t)(pwmValue[3]*act_RGBW_CH4_FACT)>>8;
	#endif
	sei();
	*/
	ADC_Init();

	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1A |= (1 << WGM10) | (0 << WGM11);	//8-bit pwm with top=0xff
	TCCR1B |= (1 << WGM12);		//fast pwm
	
	cli();
	OCR1A = 0;  // set on time (tp)
	OCR1B = 0;  // set on time (tp)
	sei();
	TCCR1A |= (1<<COM1A1);		//enable output compare
	TCCR1A |= (1<<COM1B1);		//enable output compare
	TCCR1B |= (1 << CS10);		//enable timer, prescaler=1
	TCCR0A |= (1<<WGM01)|(1<<WGM00);
	TCCR0B |= (1<<CS00);
	cli();
	OCR0A = 0;  // set on time (tp)
	OCR0B = 0;  // set on time (tp)
	sei();
	TCCR0A |= (1<<COM0B1)|(1<<COM0A1);
	
	gpio_clr_pin(EXP_B);
	gpio_clr_pin(EXP_C);
	gpio_clr_pin(EXP_F);
	gpio_clr_pin(EXP_G);
	
	gpio_set_out(EXP_B);
	gpio_set_out(EXP_C);
	gpio_set_out(EXP_F);
	gpio_set_out(EXP_G);
	
	
	Timer_SetTimeout(act_RGBW_FADE_TIMER, 10, TimerTypeFreeRunning, &act_RGBW_timer_callback);
	Timer_SetTimeout(act_RGBW_REGULATOR_TIMER, ALGO_FREQ, TimerTypeFreeRunning, calculatePWM);
	/* Setup timeout for sending the status packet */
	Timer_SetTimeout(act_RGBW_SEND_STATUS_TIMEOUT, act_RGBW_SEND_STATUS_INTERVAL_S*1000, TimerTypeFreeRunning, 0);
}

void calculatePWM(uint8_t timer) {
	uint16_t adcMeasurment;
	
	//Calculate ADC-value
	adcValue[WHITE] = (0x3ff&((MAX_ADC_W*pwmValue[WHITE])/10000));
	if (pwmValue[WHITE] == 0) adcValue[WHITE] = 0;
	adcValue[RED] = (0x3ff&((MAX_ADC_W*pwmValue[RED])/10000));
	if (pwmValue[RED] == 0) adcValue[RED] = 0;
	adcValue[BLUE] = (0x3ff&((MAX_ADC_W*pwmValue[BLUE])/10000));
	if (pwmValue[BLUE] == 0) adcValue[BLUE] = 0;
	adcValue[GREEN] = (0x3ff&((MAX_ADC_W*pwmValue[GREEN])/10000));
	if (pwmValue[GREEN] == 0) adcValue[GREEN] = 0;
	//printf("adc: %d %d %d %d\n",adcValue[WHITE],adcValue[RED],adcValue[BLUE],adcValue[GREEN]);
		
	adcMeasurment = ADC_Get(ADC_W);
	if (adcMeasurment > MAX_ADC_W+50)
		OCR_W=0;
	else if (adcMeasurment > adcValue[WHITE]+16 && OCR_W > 1)
		OCR_W-=2;
	else if (adcMeasurment < adcValue[WHITE]-5 && OCR_W < 255)
		OCR_W++;
	else if (adcMeasurment > adcValue[WHITE]+5 && OCR_W >= 1)
		OCR_W -=1;
//printf("0: %d\n",adcMeasurment);
	adcMeasurment = ADC_Get(ADC_R);
//printf("1: %d\n",adcMeasurment);
	if (adcMeasurment > MAX_ADC_R+50)
		OCR_R=0;
	else if (adcMeasurment > adcValue[RED]+16 && OCR_R > 1)
		OCR_R-=2;
	else if (adcMeasurment < adcValue[RED]-5 && OCR_R < 255)
		OCR_R++;
	else if (adcMeasurment > adcValue[RED]+5 && OCR_R >= 1)
		OCR_R -=1;

	adcMeasurment = ADC_Get(ADC_G);
//printf("1: %d\n",adcMeasurment);
	if (adcMeasurment > MAX_ADC_G+50)
		OCR_G=0;
	else if (adcMeasurment > adcValue[GREEN]+16 && OCR_G > 1)
		OCR_G-=2;
	else if (adcMeasurment < adcValue[GREEN]-5 && OCR_G < 255)
		OCR_G++;
	else if (adcMeasurment > adcValue[GREEN]+5 && OCR_G >= 1)
		OCR_G -=1;

	adcMeasurment = ADC_Get(ADC_B);
//printf("2: %d\n",adcMeasurment);
	if (adcMeasurment > MAX_ADC_B+50)
		OCR_B=0;
	else if (adcMeasurment > adcValue[BLUE]+16 && OCR_B > 1)
		OCR_B-=2;
	else if (adcMeasurment < adcValue[BLUE]-5 && OCR_B < 255)
		OCR_B++;
	else if (adcMeasurment > adcValue[BLUE]+5 && OCR_B >= 1)
		OCR_B -=1;
	

	cli();
	
	if (OCR_W==0)
	{
		TCCR1A&=~((1<<COM1B1));
	}
	else
	{
		TCCR1A |= (1<<COM1B1);		//enable output compare
	}
	if (OCR_R==0)
	{
		TCCR1A&=~((1<<COM1A1));
	}
	else
	{
		TCCR1A |= (1<<COM1A1);		//enable output compare
	}
	if (OCR_G==0)
	{
		TCCR0A &= ~((1<<COM0A0)|(1<<COM0A1));
	}
	else
	{
		TCCR0A|=(1<<COM0A1);
	}
	if (OCR_B==0)
	{
		TCCR0A &= ~((1<<COM0B0)|(1<<COM0B1));
	}
	else
	{
		TCCR0A|=(1<<COM0B1);
	}
	sei();
	
	
}

void act_RGBW_Process(void)
{
	if (Timer_Expired(act_RGBW_SEND_STATUS_TIMEOUT))
	{
		if (channel_to_send >4) {
			channel_to_send=1;
		}
		sendInfo[channel_to_send-1] = 1;
		channel_to_send++;
		
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_RGBDRIVER;
		txMsg.Header.ModuleId = 1;
		txMsg.Header.Command = CAN_MODULE_CMD_RGBDRIVER_DEBUG;
		txMsg.Length = 8;
		//txMsg.Data[0] = (uint8_t)(ADC_Get(5)>>2);
		//txMsg.Data[0] = (uint8_t)((ADC_Get(ADC_W)>>2)&0xff);
		//txMsg.Data[1] = (uint8_t)((ADC_Get(ADC_R)>>2)&0xff);
		//txMsg.Data[2] = (uint8_t)((ADC_Get(ADC_G)>>2)&0xff);
		//txMsg.Data[3] = (uint8_t)((ADC_Get(ADC_B)>>2)&0xff);
		txMsg.Data[0] = (uint8_t)((adcValue[WHITE]>>2)&0xff);
		txMsg.Data[1] = (uint8_t)((adcValue[RED]>>2)&0xff);
		txMsg.Data[2] = (uint8_t)((adcValue[GREEN]>>2)&0xff);
		txMsg.Data[3] = (uint8_t)((adcValue[BLUE]>>2)&0xff);
		txMsg.Data[4] = (uint8_t)((MAX_ADC_W>>2)&0xff);
		txMsg.Data[5] = (uint8_t)((MAX_ADC_R>>2)&0xff);
		txMsg.Data[6] = (uint8_t)((MAX_ADC_G>>2)&0xff);
		txMsg.Data[7] = (uint8_t)((MAX_ADC_B>>2)&0xff);
		//txMsg.Data[4] = (uint8_t)((adcValue[WHITE]>>2)&0xff);
		//txMsg.Data[5] = (uint8_t)((adcValue[RED]>>2)&0xff);
		//txMsg.Data[6] = (uint8_t)((adcValue[GREEN]>>2)&0xff);
		//txMsg.Data[7] = (uint8_t)((adcValue[BLUE]>>2)&0xff);
		
		StdCan_Put(&txMsg);
		
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
			txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_RGBW;
			txMsg.Header.ModuleId = act_RGBW_ID;
			txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_PWM;
			txMsg.Length = 3;
			txMsg.Data[0] = index+1;
			txMsg.Data[1] = (0xff&(pwmValue[index]>>8));
			txMsg.Data[2] = (0xff&(pwmValue[index]));
			StdCan_Put(&txMsg);
		}
	}
	
	if (Timer_Expired(act_RGBW_STORE_VALUE_TIMEOUT))
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
}

void act_RGBW_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_RGBW &&
		rxMsg->Header.ModuleId == act_RGBW_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_PHYSICAL_PWM:
			if (rxMsg->Length == 3) {
				uint8_t channel = rxMsg->Data[0];
				pwmValue[channel-1] = (rxMsg->Data[1]<<8)+(rxMsg->Data[2]);
			}
			
		break;
#if act_RGBW_ENABLE_FADE == 1
		case CAN_MODULE_CMD_RGBW_DEMO:		/* Demo(channel, speed, steps) */
			if (rxMsg->Length == 4) {
				uint8_t channel = rxMsg->Data[0]-1;
				uint8_t speed = rxMsg->Data[1];
				uint16_t steps = (rxMsg->Data[2]<<8)+(rxMsg->Data[3]);
				
				fadeSpeedCnt[channel] = 0;
				fadeSpeed[channel] = 0;
				if (speed == 0) {
					/* do nothing */
				} else {
					uint16_t diffToMin = pwmValue[channel] - ACT_RGBW_MIN_DIM;
					uint16_t diffToMax = ACT_RGBW_MAX_DIM - pwmValue[channel];
				
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
						demoHighValue[channel] = ACT_RGBW_MAX_DIM;
					}
					else if (diffToMax >= steps)
					{
						/* close to min */
						fadeTarget[channel] = ACT_RGBW_MIN_DIM;
						demoHighValue[channel] = pwmValue[channel] + steps + steps - diffToMin;
					}
					demoState[channel] = ACT_RGBW_DEMO_STATE_DECREASE;
				
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
		
		case CAN_MODULE_CMD_RGBW_START_FADE:	/* StartFade(channel, speed, direction) */
			if (rxMsg->Length == 3) {
				
				uint8_t channel = rxMsg->Data[0]-1;
				uint8_t speed = rxMsg->Data[1];
				uint8_t direction = rxMsg->Data[2];
				demoState[channel] = ACT_RGBW_DEMO_STATE_NOT_RUNNING;
				fadeSpeedCnt[channel] = 0;
				fadeSpeed[channel] = 0;
				uint16_t endValue = 0;
				if (direction == CAN_MODULE_ENUM_RGBW_START_FADE_DIRECTION_INCREASE) {
					endValue = ACT_RGBW_MAX_DIM;
				} else if (direction == CAN_MODULE_ENUM_RGBW_START_FADE_DIRECTION_DECREASE) {
					endValue = ACT_RGBW_MIN_DIM;
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

		case CAN_MODULE_CMD_RGBW_STOP_FADE:	/* StopFade(channel) */
			if (rxMsg->Length == 1) {
				uint8_t channel = rxMsg->Data[0]-1;
				demoState[channel] = ACT_RGBW_DEMO_STATE_NOT_RUNNING;
				fadeSpeed[channel] = 0;
				sendInfo[channel] = 1;		/* send netinfo with the current dimmervalue*/
			}
		break;

		case CAN_MODULE_CMD_RGBW_ABS_FADE:	/* AbsFade(channel, speed, endValue) */
			if (rxMsg->Length == 4) {
				uint8_t channel = rxMsg->Data[0]-1;
				uint8_t speed = rxMsg->Data[1];
				uint16_t endValue = (rxMsg->Data[2]<<8)+(rxMsg->Data[3]);
				demoState[channel] = ACT_RGBW_DEMO_STATE_NOT_RUNNING;
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

		case CAN_MODULE_CMD_RGBW_REL_FADE:	/* RelFade(channel, speed, direction, steps) */
			if (rxMsg->Length == 5) {
				
				uint8_t channel = rxMsg->Data[0]-1;
				uint8_t speed = rxMsg->Data[1];
				uint8_t direction = rxMsg->Data[2];
				uint16_t steps = (rxMsg->Data[3]<<8)+(rxMsg->Data[4]);
				
				demoState[channel] = ACT_RGBW_DEMO_STATE_NOT_RUNNING;
				fadeSpeedCnt[channel] = 0;
				fadeSpeed[channel] = 0;
				uint16_t tempDimVal = pwmValue[channel];
				uint16_t tempDimVal2 = pwmValue[channel];
				if (direction == CAN_MODULE_ENUM_RGBW_REL_FADE_DIRECTION_INCREASE) {					/* if increase */
					tempDimVal2 += steps;				/* calculate new value */
					if (tempDimVal2 < tempDimVal) {		/* make overflow test */
						tempDimVal2 = ACT_RGBW_MAX_DIM;
					}
				} else if (direction == CAN_MODULE_ENUM_RGBW_REL_FADE_DIRECTION_DECREASE) {			/* if decrease */
					tempDimVal2 -= steps;
					if (tempDimVal2 > tempDimVal) {
						tempDimVal2 = ACT_RGBW_MIN_DIM;
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

void act_RGBW_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_RGBW; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = act_RGBW_ID;
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
