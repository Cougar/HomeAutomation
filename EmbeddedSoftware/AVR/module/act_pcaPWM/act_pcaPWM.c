
#include "act_pcaPWM.h"
#include "drivers/io/PCA9634.h"

static uint16_t setValue[8]={0,0,0,0,0,0,0,0};
static uint16_t isValue[8];

int8_t fadeSpeed[8] = {0,0,0,0,0,0,0,0};
uint8_t fadeSpeedFrac[8] = {0,0,0,0,0,0,0,0};
uint16_t fadeTarget[8] = {0,0,0,0,0,0,0,0};
uint8_t fadeSpeedCnt[8] = {0,0,0,0,0,0,0,0};

uint16_t demoEndValue[8] = {0,0,0,0,0,0,0,0};
uint16_t demoHighValue[8] = {0,0,0,0,0,0,0,0};
uint8_t demoState[8] = {ACT_PCAPWM_DEMO_STATE_NOT_RUNNING, ACT_PCAPWM_DEMO_STATE_NOT_RUNNING, 
						ACT_PCAPWM_DEMO_STATE_NOT_RUNNING, ACT_PCAPWM_DEMO_STATE_NOT_RUNNING,
						ACT_PCAPWM_DEMO_STATE_NOT_RUNNING, ACT_PCAPWM_DEMO_STATE_NOT_RUNNING,
						ACT_PCAPWM_DEMO_STATE_NOT_RUNNING, ACT_PCAPWM_DEMO_STATE_NOT_RUNNING};

#if act_pcaPWM_USEEEPROM==1
#include "act_pcaPWM_eeprom.h"
struct eeprom_act_pcaPWM EEMEM eeprom_act_pcaPWM =
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file.
		0x0000,	// ch 1
		0x0000,	// ch 2
		0x0000,	// ch 3
		0x0000,	// ch 4
		0x0000,	// ch 5
		0x0000,	// ch 6
		0x0000,	// ch 7
		0x0000	// ch 8
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
};
#endif

#if act_pcaPWM_ENABLE_FADE == 1
void act_pcaPWM_timer_callback(uint8_t timer)
{
	uint8_t channel = 0;
	for (channel = 0; channel < 8; channel++) {
		/* Check demo states */
		if (setValue[channel] == fadeTarget[channel]) {
			switch (demoState[channel])
			{
			case ACT_PCAPWM_DEMO_STATE_NOT_RUNNING:
			break;
			case ACT_PCAPWM_DEMO_STATE_DECREASE:
				demoState[channel] = ACT_PCAPWM_DEMO_STATE_INCREASE;
				fadeTarget[channel] = demoHighValue[channel];
				fadeSpeed[channel] = -fadeSpeed[channel];
			break;
			case ACT_PCAPWM_DEMO_STATE_INCREASE:
				demoState[channel] = ACT_PCAPWM_DEMO_STATE_GOBACK;
				fadeTarget[channel] = demoEndValue[channel];
				fadeSpeed[channel] = -fadeSpeed[channel];
			break;
			case ACT_PCAPWM_DEMO_STATE_GOBACK:
				demoState[channel] = ACT_PCAPWM_DEMO_STATE_NOT_RUNNING;
			break;
			}
		}
		
		/* Change the dimmerValue according to the current fading */
		fadeSpeedCnt[channel]++;
		if (fadeSpeedCnt[channel] == fadeSpeedFrac[channel] && setValue[channel] != (fadeTarget[channel])) 
		{
			fadeSpeedCnt[channel] = 0;
			uint16_t tempDimVal = setValue[channel];
			setValue[channel] += (fadeSpeed[channel]*39);
			if ((fadeSpeed[channel] > 0 && (setValue[channel] < tempDimVal || setValue[channel] >= (fadeTarget[channel]))) || 
				(fadeSpeed[channel] < 0 && (setValue[channel] > tempDimVal || setValue[channel] <= (fadeTarget[channel])))) 
			{
				setValue[channel] = (fadeTarget[channel]);
			}
			if (setValue[channel] > 10000) {
				setValue[channel] = 10000;
			}
			/* if targetvalue was reached then send the netinfo packet */
			if (setValue[channel] == fadeTarget[channel])
			{
				//sendInfo[channel] = 1;
			}
		}
	}

}
#endif

void act_pcaPWM_Init(void)
{
#if act_pcaPWM_USEEEPROM==1
	if (EEDATA_OK)
	{
	  setValue[0] = eeprom_read_word(EEDATA16.ch1);
	  setValue[1] = eeprom_read_word(EEDATA16.ch2);
	  setValue[2] = eeprom_read_word(EEDATA16.ch3);
	  setValue[3] = eeprom_read_word(EEDATA16.ch4);
	  setValue[4] = eeprom_read_word(EEDATA16.ch5);
	  setValue[5] = eeprom_read_word(EEDATA16.ch6);
	  setValue[6] = eeprom_read_word(EEDATA16.ch7);
	  setValue[7] = eeprom_read_word(EEDATA16.ch8);
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_word_crc(EEDATA16.ch1, 0x0000, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.ch2, 0x0000, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.ch3, 0x0000, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.ch4, 0x0000, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.ch5, 0x0000, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.ch6, 0x0000, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.ch7, 0x0000, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.ch8, 0x0000, WITHOUT_CRC);
	  EEDATA_UPDATE_CRC;
	}
#endif

	pca9634_init(act_pcaPWM_I2C_ADDRESS);
	for (uint8_t i=0; i<8; i++)
	{
		pca9634_setOpMode(i, PCA9634_OP_IND);
		/* Set "current" value to something impossible to force an update at start */
		isValue[i] = 10001;
	}
	
#ifdef act_pcaPWM_OUTPUT_ENABLE_IO
	gpio_set_out(act_pcaPWM_OUTPUT_ENABLE_IO);
	gpio_clr_pin(act_pcaPWM_OUTPUT_ENABLE_IO);
#endif
#ifdef act_pcaPWM_LED_OVERRIDE_IO
	gpio_set_out(act_pcaPWM_LED_OVERRIDE_IO);
	gpio_clr_pin(act_pcaPWM_LED_OVERRIDE_IO);
#endif

#if act_pcaPWM_ENABLE_FADE == 1
	Timer_SetTimeout(act_pcaPWM_FADE_TIMER, 10, TimerTypeFreeRunning, &act_pcaPWM_timer_callback);
#endif
}

void act_pcaPWM_Process(void)
{

	for (uint8_t i=0; i < 8; i++)
	{
		if (setValue[i] != isValue[i])
		{
			isValue[i] = setValue[i];
			pca9634_setDuty(i, isValue[i]*act_pcaPWM_FACT);
#if act_pcaPWM_USEEEPROM==1
			Timer_SetTimeout(act_pcaPWM_STORE_VALUE_TIMEOUT, act_pcaPWM_STORE_VALUE_TIMEOUT_TIME*1000, TimerTypeOneShot, 0);
#endif
		}
	}

#if act_pcaPWM_USEEEPROM==1
	if (Timer_Expired(act_pcaPWM_STORE_VALUE_TIMEOUT))
	{
		if (isValue[0] != eeprom_read_word(EEDATA16.ch1))
		{
			eeprom_write_word_crc(EEDATA16.ch1, isValue[0], WITH_CRC);
		}
		if (isValue[1] != eeprom_read_word(EEDATA16.ch2))
		{
			eeprom_write_word_crc(EEDATA16.ch2, isValue[1], WITH_CRC);
		}
		if (isValue[2] != eeprom_read_word(EEDATA16.ch3))
		{
			eeprom_write_word_crc(EEDATA16.ch3, isValue[2], WITH_CRC);
		}
		if (isValue[3] != eeprom_read_word(EEDATA16.ch4))
		{
			eeprom_write_word_crc(EEDATA16.ch4, isValue[3], WITH_CRC);
		}
		if (isValue[4] != eeprom_read_word(EEDATA16.ch5))
		{
			eeprom_write_word_crc(EEDATA16.ch5, isValue[4], WITH_CRC);
		}
		if (isValue[5] != eeprom_read_word(EEDATA16.ch6))
		{
			eeprom_write_word_crc(EEDATA16.ch6, isValue[5], WITH_CRC);
		}
		if (isValue[6] != eeprom_read_word(EEDATA16.ch7))
		{
			eeprom_write_word_crc(EEDATA16.ch7, isValue[6], WITH_CRC);
		}
		if (isValue[7] != eeprom_read_word(EEDATA16.ch8))
		{
			eeprom_write_word_crc(EEDATA16.ch8, isValue[7], WITH_CRC);
		}
	}
#endif
}

void act_pcaPWM_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_PCAPWM &&
		rxMsg->Header.ModuleId == act_pcaPWM_ID)
	{
		uint8_t channel = 0;
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_PHYSICAL_PWM:
			if (rxMsg->Length == 3) {
				channel = rxMsg->Data[0];
				setValue[channel] = (rxMsg->Data[1]<<8)+(rxMsg->Data[2]);
			}
		break;
#if act_pcaPWM_ENABLE_FADE == 1
		case CAN_MODULE_CMD_PCAPWM_DEMO:		/* Demo(channel, speed, steps) */
			if (rxMsg->Length == 4) {
				channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint16_t steps = (rxMsg->Data[2]<<8)+(rxMsg->Data[3]);
				
				fadeSpeedCnt[channel] = 0;
				fadeSpeed[channel] = 0;
				if (speed == 0) {
					/* do nothing */
				} else {
					uint16_t diffToMin = setValue[channel] - ACT_PCAPWM_MIN_DIM;
					uint16_t diffToMax = ACT_PCAPWM_MAX_DIM - setValue[channel];
				
					demoEndValue[channel] = setValue[channel];
					if (diffToMin >= steps && diffToMax >= steps)
					{
						/* not close to min or max */
						fadeTarget[channel] = setValue[channel] - steps;
						demoHighValue[channel] = setValue[channel] + steps;
					}
					else if (diffToMin >= steps)
					{
						/* close to max */
						fadeTarget[channel] = setValue[channel] - steps - steps + diffToMax;
						demoHighValue[channel] = ACT_PCAPWM_MAX_DIM;
					}
					else if (diffToMax >= steps)
					{
						/* close to min */
						fadeTarget[channel] = ACT_PCAPWM_MIN_DIM;
						demoHighValue[channel] = setValue[channel] + steps + steps - diffToMin;
					}
					demoState[channel] = ACT_PCAPWM_DEMO_STATE_DECREASE;
				
					if ((speed&0x80) == 0x80) {
						fadeSpeed[channel] = (speed&0x7f)+1;
						fadeSpeedFrac[channel] = 1;
					} else {
						fadeSpeed[channel] = 1;
						fadeSpeedFrac[channel] = 0x80-(speed&0x7f);
					}
					if (fadeTarget[channel] <= setValue[channel]) {
						fadeSpeed[channel] = -fadeSpeed[channel];
					}
				}
			}
		break;
		
		case CAN_MODULE_CMD_PCAPWM_START_FADE:	/* StartFade(channel, speed, direction) */
			if (rxMsg->Length == 3) {
				
				channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint8_t direction = rxMsg->Data[2];
				demoState[channel] = ACT_PCAPWM_DEMO_STATE_NOT_RUNNING;
				fadeSpeedCnt[channel] = 0;
				fadeSpeed[channel] = 0;
				uint16_t endValue = 0;
				if (direction == CAN_MODULE_ENUM_PCAPWM_START_FADE_DIRECTION_INCREASE) {
					endValue = ACT_PCAPWM_MAX_DIM;
				} else if (direction == CAN_MODULE_ENUM_PCAPWM_START_FADE_DIRECTION_DECREASE) {
					endValue = ACT_PCAPWM_MIN_DIM;
				}
					
				if (speed == 0) {
					setValue[channel] = endValue;	/* set dimmer value immediately */
					//sendInfo[channel] = 1;		/* send netinfo with the current dimmervalue*/
				} else {
					fadeTarget[channel] = endValue;
					if (fadeTarget[channel] != setValue[channel]) {
						if ((speed&0x80) == 0x80) {
							fadeSpeed[channel] = (speed&0x7f)+1;
							fadeSpeedFrac[channel] = 1;
						} else {
							fadeSpeed[channel] = 1;
							fadeSpeedFrac[channel] = 0x80-(speed&0x7f);
						}
						if (fadeTarget[channel] < setValue[channel]) {
							fadeSpeed[channel] = -fadeSpeed[channel];
						}
					}
				}
			}
		break;
		
		case CAN_MODULE_CMD_PCAPWM_STOP_FADE:	/* StopFade(channel) */
			if (rxMsg->Length == 1) {
				channel = rxMsg->Data[0];
				demoState[channel] = ACT_PCAPWM_DEMO_STATE_NOT_RUNNING;
				fadeSpeed[channel] = 0;
				//sendInfo[channel] = 1;		/* send netinfo with the current dimmervalue*/
			}
		break;

		case CAN_MODULE_CMD_PCAPWM_ABS_FADE:	/* AbsFade(channel, speed, endValue) */
			if (rxMsg->Length == 4) {
				channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint16_t endValue = (rxMsg->Data[2]<<8)+(rxMsg->Data[3]);
				demoState[channel] = ACT_PCAPWM_DEMO_STATE_NOT_RUNNING;
				fadeSpeedCnt[channel] = 0;
				fadeSpeed[channel] = 0;
				if (speed == 0) {
					setValue[channel] = endValue;	/* set dimmer value immediately */
					//sendInfo[channel] = 1;		/* send netinfo with the current dimmervalue*/
				} else {
					fadeTarget[channel] = endValue;
					if (fadeTarget[channel] != setValue[channel]) {
						if ((speed&0x80) == 0x80) {
							fadeSpeed[channel] = (speed&0x7f)+1;
							fadeSpeedFrac[channel] = 1;
						} else {
							fadeSpeed[channel] = 1;
							fadeSpeedFrac[channel] = 0x80-(speed&0x7f);
						}
						if (fadeTarget[channel] < setValue[channel]) {
							fadeSpeed[channel] = -fadeSpeed[channel];
						}
					}
				}
				//printf("abs fade %d %d %d!\n",fadeTarget[channel], endValue ,pwmValue[channel] );
			}
		break;
		
		case CAN_MODULE_CMD_PCAPWM_REL_FADE:	/* RelFade(channel, speed, direction, steps) */
			if (rxMsg->Length == 5) {
				
				channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint8_t direction = rxMsg->Data[2];
				uint16_t steps = (rxMsg->Data[3]<<8)+(rxMsg->Data[4]);
				
				demoState[channel] = ACT_PCAPWM_DEMO_STATE_NOT_RUNNING;
				fadeSpeedCnt[channel] = 0;
				fadeSpeed[channel] = 0;
				uint16_t tempDimVal = setValue[channel];
				uint16_t tempDimVal2 = setValue[channel];
				if (direction == CAN_MODULE_ENUM_PCAPWM_REL_FADE_DIRECTION_INCREASE) {					/* if increase */
					tempDimVal2 += steps;				/* calculate new value */
					if (tempDimVal2 < tempDimVal) {		/* make overflow test */
						tempDimVal2 = ACT_PCAPWM_MAX_DIM;
					}
				} else if (direction == CAN_MODULE_ENUM_PCAPWM_REL_FADE_DIRECTION_DECREASE) {			/* if decrease */
					tempDimVal2 -= steps;
					if (tempDimVal2 > tempDimVal) {
						tempDimVal2 = ACT_PCAPWM_MIN_DIM;
					}
				}
				if (speed == 0) {
					setValue[channel] = tempDimVal2;		/* set dimmer value immediately */
					//sendInfo[channel] = 1;				/* send netinfo with the current dimmervalue*/
				} else {
					fadeTarget[channel] = tempDimVal2;		/* set the fade target */
					
					if (fadeTarget[channel] != setValue[channel]) {
						if ((speed&0x80) == 0x80) {
							fadeSpeed[channel] = (speed&0x7f)+1;
							fadeSpeedFrac[channel] = 1;
						} else {
							fadeSpeed[channel] = 1;
							fadeSpeedFrac[channel] = 0x80-(speed&0x7f);
						}
						if (fadeTarget[channel] < setValue[channel]) {
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

void act_pcaPWM_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_PCAPWM;
	txMsg.Header.ModuleId = act_pcaPWM_ID;
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
