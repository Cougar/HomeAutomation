
#include "sns_flower.h"

#if sns_flower_USEEEPROM==1
#include "sns_flower_eeprom.h"

struct eeprom_sns_flower EEMEM eeprom_sns_flower = 
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file. 
		0xAB,	// x
		0x1234	// y
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 
#endif

void sns_flower_Init(void)
{
#if sns_flower_USEEEPROM==1
	if (EEDATA_OK)
	{
	  ///TODO: Use stored data to set initial values for the module
	  blablaX = eeprom_read_byte(EEDATA.x);
	  blablaY = eeprom_read_word(EEDATA.y);
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_byte_crc(EEDATA.x, 0xAB, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA.y, 0x1234, WITHOUT_CRC);
	  EEDATA_UPDATE_CRC;
	}
#endif  

	// to use PCINt lib, call this function: (the callback function look as a timer callback function)
	// Pcint_SetCallbackPin(sns_flower_PCINT, EXP_C , &sns_flower_pcint_callback);

	ADC_Init();
	Timer_SetTimeout(sns_flower_WAIT_TIMER, sns_flower_WAIT_PERIOD_S*1000u , TimerTypeFreeRunning, 0);
}



void sns_flower_Process(void)
{
	static uint8_t measureState=0;
	static uint16_t results[6];
	static uint16_t resultsInv[6];
	uint16_t temp;
	static uint8_t flowerChannelToSend = 0;
  
	
	if (Timer_Expired(sns_flower_WAIT_TIMER)) {
		
		gpio_set_out(sns_flower_highSide_PIN);
		gpio_set_out(sns_flower_lowSide_PIN);
		gpio_set_pin(sns_flower_highSide_PIN);
		gpio_clr_pin(sns_flower_lowSide_PIN);
		measureState=0;
		Timer_SetTimeout(sns_flower_MEASUREMENT_TIMER, sns_flower_MEASUREMENT_PERIOD_MS , TimerTypeOneShot, 0);
	}
	
	if (Timer_Expired(sns_flower_MEASUREMENT_TIMER)) {
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_VOLTAGECURRENT;
		txMsg.Header.ModuleId = sns_flower_ID;
		txMsg.Length = 3;
		switch (measureState) {
			case 0:
#ifdef sns_flower0AD
				results[0] = ADC_Get(sns_flower0AD);
#endif
#ifdef sns_flower1AD
				results[1] = ADC_Get(sns_flower1AD);
#endif
#ifdef sns_flower2AD
				results[2] = ADC_Get(sns_flower2AD);
#endif
#ifdef sns_flower3AD
				results[3] = ADC_Get(sns_flower3AD);
#endif
#ifdef sns_flower4AD
				results[4] = ADC_Get(sns_flower4AD);
#endif
#ifdef sns_flower5AD
				results[5] = ADC_Get(sns_flower5AD);
#endif	
				gpio_clr_pin(sns_flower_highSide_PIN);
				gpio_set_pin(sns_flower_lowSide_PIN);
				measureState=1;
				Timer_SetTimeout(sns_flower_MEASUREMENT_TIMER, sns_flower_MEASUREMENT_PERIOD_MS , TimerTypeOneShot, 0);
				break;
			case 1:
#ifdef sns_flower0AD
				resultsInv[0] = 1023-ADC_Get(sns_flower0AD);
#endif
#ifdef sns_flower1AD
				resultsInv[1] = 1023-ADC_Get(sns_flower1AD);
#endif
#ifdef sns_flower2AD
				resultsInv[2] = 1023-ADC_Get(sns_flower2AD);
#endif
#ifdef sns_flower3AD
				resultsInv[3] = 1023-ADC_Get(sns_flower3AD);
#endif
#ifdef sns_flower4AD
				resultsInv[4] = 1023-ADC_Get(sns_flower4AD);
#endif
#ifdef sns_flower5AD
				resultsInv[5] = 1023-ADC_Get(sns_flower5AD);
#endif	
				gpio_clr_pin(sns_flower_highSide_PIN);
				gpio_clr_pin(sns_flower_lowSide_PIN);
				gpio_set_in(sns_flower_highSide_PIN);
				gpio_set_in(sns_flower_lowSide_PIN);
				measureState=2;
				flowerChannelToSend=0;
				Timer_SetTimeout(sns_flower_MEASUREMENT_TIMER, sns_flower_MEASUREMENT_PERIOD_MS , TimerTypeOneShot, 0);
				break;
			case 2:	
				  switch(flowerChannelToSend) {
					  case 0:
#ifdef sns_flower0AD
						txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_PERCENT;
						txMsg.Data[0] = 0;
						temp = (uint16_t)((float)(results[0]+resultsInv[0])*4.888);
						txMsg.Data[1] = (temp>>8)&0xff;
						txMsg.Data[2] = (temp)&0xff;

						while (StdCan_Put(&txMsg) != StdCan_Ret_OK) {}
						flowerChannelToSend = 1;
# if  !(defined(sns_flower1AD) || defined(sns_flower2AD) || defined(sns_flower3AD) || defined(sns_flower4AD) || defined(sns_flower5AD))
						flowerChannelToSend = 0;
						measureState=0;
# endif
						break;
#endif
					case 1:
#ifdef sns_flower1AD
						txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_PERCENT;
						txMsg.Data[0] = 1;
						temp = (uint16_t)((float)(results[1]+resultsInv[1])*4.888);
						txMsg.Data[1] = (temp>>8)&0xff;
						txMsg.Data[2] = (temp)&0xff;

						while (StdCan_Put(&txMsg) != StdCan_Ret_OK) {}
						flowerChannelToSend = 2;
# if  !(defined(sns_flower2AD) || defined(sns_flower3AD) || defined(sns_flower4AD) || defined(sns_flower5AD))
						flowerChannelToSend = 0;
						measureState=0;
# endif
						break;
#endif
					case 2:
#ifdef sns_flower2AD
						txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_PERCENT;
						txMsg.Data[0] = 2;
						temp = (uint16_t)((float)(results[2]+resultsInv[2])*4.888);
						txMsg.Data[1] = (temp>>8)&0xff;
						txMsg.Data[2] = (temp)&0xff;

						while (StdCan_Put(&txMsg) != StdCan_Ret_OK) {}
						flowerChannelToSend = 3;
#if  !(defined(sns_flower3AD) || defined(sns_flower4AD) || defined(sns_flower5AD))
						flowerChannelToSend = 0;
						measureState=0;
#endif
						break;
#endif
					case 3:
#ifdef sns_flower3AD
						txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_PERCENT;
						txMsg.Data[0] = 3;
						temp = (uint16_t)((float)(results[3]+resultsInv[3])*4.888);
						txMsg.Data[1] = (temp>>8)&0xff;
						txMsg.Data[2] = (temp)&0xff;

						while (StdCan_Put(&txMsg) != StdCan_Ret_OK) {}
						flowerChannelToSend = 4;
#if  !( defined(sns_flower4AD) || defined(sns_flower5AD))
						flowerChannelToSend = 0;
						measureState=0;
#endif
						break;
#endif
					case 4:
#ifdef sns_flower4AD
						txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_PERCENT;
						txMsg.Data[0] = 4;
						temp = (uint16_t)((float)(results[4]+resultsInv[4])*4.888);
						txMsg.Data[1] = (temp>>8)&0xff;
						txMsg.Data[2] = (temp)&0xff;

						while (StdCan_Put(&txMsg) != StdCan_Ret_OK) {}
						flowerChannelToSend = 5;
#if  !(defined(sns_flower5AD))
						flowerChannelToSend = 0;
						measureState=0;
#endif
						break;
#endif
					case 5:
#ifdef sns_flower5AD
						txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_PERCENT;
						txMsg.Data[0] = 5;
						temp = (uint16_t)((float)(results[5]+resultsInv[5])*4.888);
						txMsg.Data[1] = (temp>>8)&0xff;
						txMsg.Data[2] = (temp)&0xff;

						while (StdCan_Put(&txMsg) != StdCan_Ret_OK) {}
						flowerChannelToSend = 0;
						measureState=0;
						break;
#endif
					default:
						measureState=0;
						break;
				}
				if (measureState != 0) {
					Timer_SetTimeout(sns_flower_MEASUREMENT_TIMER, sns_flower_CAN_MSG_PAUS_MS , TimerTypeOneShot, 0);
				}
				break;
		}
  
	}
}

void sns_flower_HandleMessage(StdCan_Msg_t *rxMsg)
{
/*	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS && 
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_FLOWER && 
		rxMsg->Header.ModuleId == sns_flower_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}*/
}

void sns_flower_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_FLOWER; 
	txMsg.Header.ModuleId = sns_flower_ID;
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
