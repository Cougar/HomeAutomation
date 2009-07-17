
#include "sns_Touch.h"
point touchBuffer[sns_Touch_BUFFERSIZE];
uint8_t rxbufidx;


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
}


/*
F1: Klar
F2: Klar
F3: Klar
F4:	Klar
F5:	Klar
F6:	Klar
F7: Klar
F8:	Klar
F9:	

*/



uint8_t parseBuffer(uint8_t startIndex, uint8_t endIndex)
{
	int16_t f1_k = 0, f1_tmp = 0,f2_tmp = 0,f3_tmp = 0,f4_tmp = 0,f5_tmp = 0,f8_tmp = 0,f9_tmp = 0,int16temp;
	uint8_t f7_tmp = 0, f7_sign = 0, f1_sign = 0 ,f3_yref = 0, f1 = 0,f2 = 0,f3 = 0,f4 = 0,f5 = 0,f6 = 0,f7 = 0,f8 = 0,f9 = 0, xMin, xMax, yMin, yMax;
	
	if ((int16_t)touchBuffer[endIndex].y - (int16_t)touchBuffer[startIndex].y > 0)
		f6 = 1;
	else
		f6 = 0;
	
	xMin = touchBuffer[startIndex].x;
	xMax = touchBuffer[startIndex].x;
	yMin = touchBuffer[startIndex].y;
	yMax = touchBuffer[startIndex].y;
	f3_yref = (touchBuffer[startIndex].y + touchBuffer[endIndex].y)/2;
	f1_k = ((touchBuffer[endIndex].y-touchBuffer[startIndex].y)<<4)/(touchBuffer[endIndex].x-touchBuffer[startIndex].x);
	f1_sign = 2;

	for (uint8_t i = startIndex+1; i < endIndex-1; i++)
	{
		if (((f1_k*(touchBuffer[i].x-touchBuffer[0].x))>>4) + touchBuffer[0].y < touchBuffer[i].y) {
			if (f1_sign != 1)
			    f1++;
			f1_sign = 1;
		}
		else {
			if (f1_sign != 0)
			    f1++;
			f1_sign = 0;
		}
		
		if (touchBuffer[i].x > min(touchBuffer[startIndex].x, touchBuffer[endIndex].x) && touchBuffer[i].x < max(touchBuffer[startIndex].x, touchBuffer[endIndex].x)) 
		{
		    f3_tmp += f3_yref - (touchBuffer[i].y + touchBuffer[i-1].y)/2;
		}
		if (xMin > touchBuffer[i].x)
			xMin = touchBuffer[i].x;
		if (xMax < touchBuffer[i].x)
			xMax = touchBuffer[i].x;
		if (yMin > touchBuffer[i].y)
			yMin = touchBuffer[i].y;
		if (yMax < touchBuffer[i].y)
			yMax = touchBuffer[i].y;
		f4_tmp += (int16_t)touchBuffer[i].x - (int16_t)touchBuffer[endIndex].x;
		f5_tmp += (int16_t)touchBuffer[i].x - (int16_t)touchBuffer[startIndex].x;
	}
	
	for (uint8_t j = yMin; j < yMax-1; j++)
	{
		f7_tmp = 0xff;
		f7_sign = 0;
		for (uint8_t i = startIndex+1; i < endIndex-1; i++)
		{
			if (j > touchBuffer[i].y) {
				if (f7_sign != '+')
				    f7_tmp++;
				f7_sign = '+';
			}
			else {
				if (f7_sign != '-')
				    f7_tmp++;
				f7_sign = '-';
			}
		}
		if (f7_tmp > f7)
			f7 = f7_tmp;
	}
	
	f1--;
	f2 = f1_sign;
	if (f1 == 0)
		f2 = 2;
	
	if (f3_tmp > 0)
		f3 = 0;
	else
		f3 = 1;
		
	//int16temp = (((int16_t)touchBuffer[startIndex].x - (int16_t)(xMax+xMin)/2)*((int16_t)touchBuffer[endIndex].x - (int16_t)(xMax+xMin)/2));
	//int16temp = ((touchBuffer[startIndex].x - (xMax+xMin)/2)*(touchBuffer[endIndex].x - (xMax+xMin)/2));
	int16temp = (touchBuffer[startIndex].x*touchBuffer[endIndex].x - (xMax+xMin)/2);
	if (int16temp > 0)
		f9 = 1;
	else
		f9 = 0;
	if (f5_tmp > 0)
		f5 = 1;
	else
		f5 = 0;
	if (f4_tmp > 0)
		f4 = 1;
	else
		f4 = 0;
	if (xMin < min(touchBuffer[startIndex].x,touchBuffer[endIndex].x) || xMax > max(touchBuffer[startIndex].x,touchBuffer[endIndex].x))
		f8 = 0;
	else
		f8 = 1;
		
	//printf("1%u2%c3%c4%c5%c6%c7%u8%c9%c\n", f1, f2, f3, f4, f5, f6, f7, f8, f9);

	StdCan_Msg_t txMsg;
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_TOUCH;
	txMsg.Header.ModuleId = sns_Touch_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_TOUCH_GESTURE;
	txMsg.Length = 3;
	txMsg.Data[0] = f1;
	txMsg.Data[1] = f7;
	txMsg.Data[2] = f3<<7|f4<<6|f5<<5|f6<<4|f8<<3|f9<<2|f2;
	
	StdCan_Put(&txMsg);
	return 0;
}


void sns_Touch_Process(void)
{
	
	if (Timer_Expired(sns_Touch_POLL_TIMER)) {
		//StdCan_Msg_t txMsg;
		gpio_set_in(sns_Touch_XPLUS);
		gpio_set_pullup(sns_Touch_XPLUS);	//turn on pullup for x+
		gpio_set_out(sns_Touch_YPLUS);
		gpio_set_pin(sns_Touch_YPLUS);		//turn on 1 on y+
		gpio_set_out(sns_Touch_YMINUS);
		gpio_clr_pin(sns_Touch_YMINUS);		//turn on 0 on y-
		uint8_t adxval = ADC_Get(sns_Touch_TOUCHXAD)>>2;	//read x-
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
		uint8_t adyval = ADC_Get(sns_Touch_TOUCHYAD)>>2;	//read y-
		gpio_set_in(sns_Touch_XPLUS);
		gpio_clr_pullup(sns_Touch_XPLUS);	//turn off pullup for x+
		gpio_set_in(sns_Touch_XMINUS);
		gpio_clr_pullup(sns_Touch_XMINUS);	//turn off pullup for x-
		gpio_set_in(sns_Touch_YPLUS);
		gpio_clr_pullup(sns_Touch_YPLUS);	//turn off pullup for y+
		
		if (adyval < 0xf0 && adxval < 0xf0 ) 
		{
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
			//printf("Y: %d X: %d\n", adyval, adxval);
		}
		else if (rxbufidx > 2)
		{
			//printf("Released %d\n", rxbufidx);
			uint8_t temp = parseBuffer(0, rxbufidx-1);
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

	txMsg.Data[0] = NODE_HW_ID_BYTE0;
	txMsg.Data[1] = NODE_HW_ID_BYTE1;
	txMsg.Data[2] = NODE_HW_ID_BYTE2;
	txMsg.Data[3] = NODE_HW_ID_BYTE3;
	
	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;
	
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}
