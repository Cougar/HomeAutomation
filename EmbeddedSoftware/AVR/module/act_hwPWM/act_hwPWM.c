
#include "act_hwPWM.h"

uint16_t pwmValue[4];

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
TCCR1A|=(act_hwPWM_CH1_COM<<COM1B0)|(act_hwPWM_CH2_COM<<COM1A0);
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

/* enable outputs for timer 1 */
#if act_hwPWM_CH3_COM>0
gpio_set_out(EXP_F);
#endif
#if act_hwPWM_CH4_COM>0
gpio_set_out(EXP_G);
#endif

/* set up counter mode for timer 0 */
#if act_hwPWM_CH3_COM>0 || act_hwPWM_CH4_COM>0
TCCR0A|=(act_hwPWM_CH3_COM<<COM1A0)|(act_hwPWM_CH4_COM<<COM1B0);
#endif

/* enable timer 0 */
#if act_hwPWM_CH3_COM>0
TCCR0B|=(act_hwPWM_CH3_CS<<CS00);
#elif act_hwPWM_CH4_COM>0
TCCR0B|=(act_hwPWM_CH4_CS<<CS00);
#endif


}

void act_hwPWM_Process(void)
{
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
}

void act_hwPWM_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_HWPWM &&
		rxMsg->Header.ModuleId == act_hwPWM_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_PHYSICAL_PWM:
			if (rxMsg->Length == 3) {
				uint8_t channel = rxMsg->Data[0];
				pwmValue[channel-1] = (rxMsg->Data[1]<<8)+(rxMsg->Data[2]);
				cli();
				switch (channel)
				{
#if act_hwPWM_CH1_COM>0
					case 1:
						OCR_1=(uint16_t)(pwmValue[channel-1]*act_hwPWM_CH1_FACT)>>8;
					break;
#endif
#if act_hwPWM_CH2_COM>0
					case 2:
						OCR_2=(uint16_t)(pwmValue[channel-1]*act_hwPWM_CH2_FACT)>>8;
					break;
#endif
#if act_hwPWM_CH3_COM>0
					case 3:
						OCR_3=(uint16_t)(pwmValue[channel-1]*act_hwPWM_CH3_FACT)>>8;
					break;
#endif
#if act_hwPWM_CH4_COM>0
					case 4:
						OCR_4=(uint16_t)(pwmValue[channel-1]*act_hwPWM_CH4_FACT)>>8;
					break;
#endif
				}
				sei();
				Timer_SetTimeout(act_hwPWM_STORE_VALUE_TIMEOUT, act_hwPWM_STORE_VALUE_TIMEOUT_TIME*1000, TimerTypeOneShot, 0);
			}
			
		break;
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
