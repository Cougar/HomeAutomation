
#include "sns_identification.h"
uint8_t sns_identification_pinStatus;
#define HIGH		1
#define LOW			2
#define NOCHANGE 	0
#define HIGH_NOCH	3
#define LOW_NOCH	4

#define IO_HIGH		1
#define IO_LOW		0


void sns_identification_pcint_callback(uint8_t id, uint8_t status) 
{
#ifdef	sns_identification_PIN
#if sns_identification_PCA95xxIO==0
	if (id == sns_identification_PCINT) {
	  if (IO_LOW != status) {
	    sns_identification_pinStatus = HIGH;
	  } else {
	    sns_identification_pinStatus = LOW;
	  }
	}
#endif
#endif
}

#if sns_identification_PCA95xxIO==1
/*void sns_identification_PCA95xx_callback(uint16_t status) 
{
	printf("0x%x\n",status);
}*/
#endif 

void sns_identification_Init(void)
{
	sns_identification_pinStatus = NOCHANGE;

#if sns_identification_PCA95xxIO==1
	Pca95xx_Init(0);
#endif 

#ifdef	sns_identification_PIN
#if sns_identification_PCA95xxIO==0
	#if (sns_identification_pullup == 1)
		gpio_set_pullup(sns_identification_PIN);
	#else
		gpio_clr_pullup(sns_identification_PIN);
	#endif
		gpio_set_in(sns_identification_PIN);
		Pcint_SetCallbackPin(sns_identification_PCINT, sns_identification_PIN, &sns_identification_pcint_callback);
#else
		Pca95xx_set_in(sns_identification_PIN);
#endif
#endif

#if sns_identification_PCA95xxIO==1
	uint16_t status = Pca95xx_GetInputs();
#ifdef	sns_identification_PIN
#if sns_identification_PCA95xxIO==1
	if (((status>>sns_identification_PIN)&0x1) != IO_LOW) {
		sns_identification_pinStatus = HIGH_NOCH;
	} else {
		sns_identification_pinStatus = LOW_NOCH;
	}
#endif
#endif
	/* Currently only polling of ports is implemented */
	//Pca95xx_SetCallback(sns_identification_PCA95XX_CALLBACK_INDEX, &sns_identification_PCA95xx_callback);
#endif 

	Timer_SetTimeout(sns_identification_POLL_TIMER, sns_identification_POLL_TIME_MS, TimerTypeFreeRunning, 0);
}

void sns_identification_Process(void)
{
	if (Timer_Expired(sns_identification_POLL_TIMER))
	{
#if sns_identification_PCA95xxIO==1
		uint16_t status = Pca95xx_GetInputs();
#ifdef	sns_identification_PIN
#if sns_identification_PCA95xxIO==1
		if (sns_identification_pinStatus == LOW_NOCH && ((status>>sns_identification_PIN)&0x1) == IO_HIGH) {
			sns_identification_pinStatus = HIGH;
		} else if (sns_identification_pinStatus == HIGH_NOCH && ((status>>sns_identification_PIN)&0x1) == IO_LOW) {
			sns_identification_pinStatus = LOW;
		}
#endif
#endif
#endif 

		if (sns_identification_pinStatus == LOW) {
			StdCan_Msg_t txMsg;
			StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
			StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
			txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_IDENTIFICATION;
			txMsg.Header.ModuleId = sns_identification_ID;
			txMsg.Header.Command = CAN_MODULE_CMD_IDENTIFICATION_ID;
			txMsg.Length = 4;
			uint32_t HwId=BIOS_GetHwId();
			txMsg.Data[0] = HwId&0xff;
			txMsg.Data[1] = (HwId>>8)&0xff;
			txMsg.Data[2] = (HwId>>16)&0xff;
			txMsg.Data[3] = (HwId>>24)&0xff;
			sns_identification_pinStatus = LOW_NOCH;
			while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
		}
		else if (sns_identification_pinStatus == HIGH)
		{
			sns_identification_pinStatus = HIGH_NOCH;
		}
	}
}

void sns_identification_HandleMessage(StdCan_Msg_t *rxMsg)
{
	
}

void sns_identification_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_IDENTIFICATION;
	txMsg.Header.ModuleId = sns_identification_ID;
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
