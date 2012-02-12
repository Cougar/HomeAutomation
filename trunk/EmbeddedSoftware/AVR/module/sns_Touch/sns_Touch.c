
#include "sns_Touch.h"
point touchBuffer[sns_Touch_BUFFERSIZE];
uint8_t rxbufidx;
uint8_t pushStatus;
point lastValidPoint;
uint8_t timeCnt;


#ifdef sns_Touch_USEEEPROM
#include "sns_Touch_eeprom.h"
struct eeprom_sns_Touch EEMEM eeprom_sns_Touch = 
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file. 
		0xAB,	// x
		0x1234	// y
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 
#endif

void sns_Touch_Init(void)
{
#ifdef sns_Touch_USEEEPROM
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
	///TODO: Initialize hardware etc here

	// to use PCINt lib, call this function: (the callback function look as a timer callback function)
	// Pcint_SetCallbackPin(sns_Touch_PCINT, EXP_C , &sns_Touch_pcint_callback);

	ADC_Init();
	Timer_SetTimeout(sns_Touch_POLL_TIMER, sns_Touch_POLL_PERIOD , TimerTypeFreeRunning, 0);
	
	rxbufidx = 0;
	pushStatus = 0;
	lastValidPoint.x=0;
	lastValidPoint.y=0;
	timeCnt=0;
}

void sns_Touch_Process(void)
{
	
	if (Timer_Expired(sns_Touch_POLL_TIMER)) {
		timeCnt++;
		//StdCan_Msg_t txMsg;
		gpio_set_in(sns_Touch_XPLUS);
		gpio_set_pullup(sns_Touch_XPLUS);	//turn on pullup for x+
		gpio_set_out(sns_Touch_YPLUS);
		gpio_set_pin(sns_Touch_YPLUS);		//turn on 1 on y+
		gpio_set_out(sns_Touch_YMINUS);
		gpio_clr_pin(sns_Touch_YMINUS);		//turn on 0 on y-
#ifdef sns_Touch_SWITCH_XY
			uint8_t adyval = ADC_Get(sns_Touch_TOUCHXAD)>>2;	//read x-
#else
			uint8_t adxval = ADC_Get(sns_Touch_TOUCHXAD)>>2;	//read x-
#endif
		
		gpio_set_in(sns_Touch_YPLUS);
		gpio_clr_pullup(sns_Touch_YPLUS);	//turn off pullup for y+
		gpio_set_in(sns_Touch_YMINUS);
		gpio_clr_pullup(sns_Touch_YMINUS);	//turn off pullup for y-
		gpio_set_in(sns_Touch_XPLUS);
		gpio_clr_pullup(sns_Touch_XPLUS);	//turn off pullup for x+
		
		gpio_set_in(sns_Touch_YPLUS);
		gpio_set_pullup(sns_Touch_YPLUS);	//turn on pullup for y+
		gpio_set_out(sns_Touch_XPLUS);
		gpio_set_pin(sns_Touch_XPLUS);		//turn on 1 on x+
		gpio_set_out(sns_Touch_XMINUS);
		gpio_clr_pin(sns_Touch_XMINUS);		//turn on 0 on x-
#ifdef sns_Touch_SWITCH_XY
		uint8_t adxval = ADC_Get(sns_Touch_TOUCHYAD)>>2;	//read y-
#else
		uint8_t adyval = ADC_Get(sns_Touch_TOUCHYAD)>>2;	//read y-
#endif
		gpio_set_in(sns_Touch_XPLUS);
		gpio_clr_pullup(sns_Touch_XPLUS);	//turn off pullup for x+
		gpio_set_in(sns_Touch_XMINUS);
		gpio_clr_pullup(sns_Touch_XMINUS);	//turn off pullup for x-
		gpio_set_in(sns_Touch_YPLUS);
		gpio_clr_pullup(sns_Touch_YPLUS);	//turn off pullup for y+
		
		

		if (adyval < 0xf0 && adxval < 0xf0 ) 
		{
#ifdef sns_Touch_INVERT_Y
			adyval = 255-adyval;
#endif
#ifdef sns_Touch_INVERT_X
			adxval = 255-adxval;
#endif
			lastValidPoint.x=adxval;
			lastValidPoint.y=adyval;
			pushStatus = 1;
			uint8_t xdiff=0;
			uint8_t ydiff=0;
			if (rxbufidx > 0)
			{
				xdiff = touchBuffer[rxbufidx-1].x - adxval;
				if (touchBuffer[rxbufidx-1].x < adxval)
				{
					xdiff = adxval - touchBuffer[rxbufidx-1].x;
				}
				ydiff = touchBuffer[rxbufidx-1].y - adyval;
				if (touchBuffer[rxbufidx-1].y < adyval)
				{
					ydiff = adyval - touchBuffer[rxbufidx-1].y;
				}
			}
			if (rxbufidx == 0 || (rxbufidx > 0 && xdiff+ydiff > 10))
			{
				touchBuffer[rxbufidx].x = adxval;
				touchBuffer[rxbufidx].y = adyval;
				rxbufidx++;
				if (rxbufidx==sns_Touch_BUFFERSIZE)
				{
					rxbufidx=0;
				}
			}
			if (rxbufidx == 0 || (rxbufidx > 0 && xdiff+ydiff > 10) || timeCnt%sns_Touch_POLL_SEND_RESOLUTION == 0)
			{
				StdCan_Msg_t txMsg;
				StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
				StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
				txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_TOUCH;
				txMsg.Header.ModuleId = sns_Touch_ID;
				txMsg.Header.Command = CAN_MODULE_CMD_TOUCH_RAW;
				txMsg.Length = 3;
				txMsg.Data[0] = CAN_MODULE_ENUM_TOUCH_RAW_STATUS_PRESSED;
				txMsg.Data[1] = adxval;
				txMsg.Data[2] = adyval;
	
				StdCan_Put(&txMsg);
			}
			//printf("Y: %d X: %d\n", adyval, adxval);
		}
		else if (rxbufidx > 3)
		{
			if (pushStatus==1)
			{
				pushStatus = 2;
			}
			//printf("Released %d\n", rxbufidx);
			gesture functionData = parseBuffer(touchBuffer, 0, rxbufidx-1);
			
			StdCan_Msg_t txMsg;
			StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
			StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
			txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_TOUCH;
			txMsg.Header.ModuleId = sns_Touch_ID;
			txMsg.Header.Command = CAN_MODULE_CMD_TOUCH_GESTURE;
			txMsg.Length = 3;
			txMsg.Data[0] = functionData.f1;
			txMsg.Data[1] = functionData.f7;
			txMsg.Data[2] = functionData.f3<<7|functionData.f4<<6|functionData.f5<<5|functionData.f6<<4|functionData.f8<<3|functionData.f9<<2|functionData.f2;
	
			StdCan_Put(&txMsg);
			/*for (uint8_t i = 0; i < rxbufidx; i++)
			{
				printf("%3d ", touchBuffer[i].x);
			}
			printf("\n");
			for (uint8_t i = 0; i < rxbufidx; i++)
			{
				printf("%3d ", touchBuffer[i].y);
			}
			printf("\n");
			*/
			
			rxbufidx = 0;
		}
		else if (pushStatus==1)
		{
			rxbufidx = 0;
			pushStatus = 2;
		}
		
		if (pushStatus == 2)
		{
			pushStatus = 0;
			
			StdCan_Msg_t txMsg;
			StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
			StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
			txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_TOUCH;
			txMsg.Header.ModuleId = sns_Touch_ID;
			txMsg.Header.Command = CAN_MODULE_CMD_TOUCH_RAW;
			txMsg.Length = 3;
			txMsg.Data[0] = CAN_MODULE_ENUM_TOUCH_RAW_STATUS_RELEASED;
			txMsg.Data[1] = lastValidPoint.x;
			txMsg.Data[2] = lastValidPoint.y;

			StdCan_Put(&txMsg);
		}
	}
}


void sns_Touch_HandleMessage(StdCan_Msg_t *rxMsg)
{
	/*if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_xyz && ///TODO: Change this to the actual class type
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_xyz && ///TODO: Change this to the actual module type
		rxMsg->Header.ModuleId == sns_Touch_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}*/
}

void sns_Touch_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_TOUCH;
	txMsg.Header.ModuleId = sns_Touch_ID;
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
