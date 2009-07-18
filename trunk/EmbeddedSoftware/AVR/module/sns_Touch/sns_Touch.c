
#include "sns_Touch.h"
point touchBuffer[sns_Touch_BUFFERSIZE];
uint8_t rxbufidx;
uint8_t pushStatus;

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


/*
To fix:
Input parameter for buffer
Output returnvalues
Break out to a driver file
*/
uint8_t parseBuffer(uint8_t startIndex, uint8_t endIndex)
{
	int16_t f1_k = 0, f3_tmp = 0, f4_tmp = 0, f5_tmp = 0, f9_tmp = 0;
	uint8_t f7_tmp = 0, f7_sign = 0, f1_sign = 0, f3_yref = 0, f1 = 0, f2 = 0, f3 = 0, f4 = 0, f5 = 0, f6 = 0, f7 = 0, f8 = 0, f9 = 0, xMin, xMax, yMin, yMax;
	
	/* Function 6 indicates the sign value of the y coordinates of the last point minus y coordinates of the first point */
	if ((int16_t)touchBuffer[endIndex].y - (int16_t)touchBuffer[startIndex].y > 0)
	{
		f6 = CAN_MODULE_ENUM_TOUCH_GESTURE_F6_P;
	}
	else
	{
		f6 = CAN_MODULE_ENUM_TOUCH_GESTURE_F6_M;
	}
	
	xMin = touchBuffer[startIndex].x;
	xMax = touchBuffer[startIndex].x;
	yMin = touchBuffer[startIndex].y;
	yMax = touchBuffer[startIndex].y;
	f3_yref = (touchBuffer[startIndex].y + touchBuffer[endIndex].y)/2;
	f1_k = ((touchBuffer[endIndex].y-touchBuffer[startIndex].y)<<4)/(touchBuffer[endIndex].x-touchBuffer[startIndex].x);
	f1_sign = CAN_MODULE_ENUM_TOUCH_GESTURE_F2_U;

	for (uint8_t i = startIndex+1; i < endIndex-1; i++)
	{
		/* Function 1 indicates the number of intersection points between an input gesture g(x) and the straight line f(x) 
		which connects the first and last point of the gesture */
		if (((f1_k*(touchBuffer[i].x-touchBuffer[0].x))>>4) + touchBuffer[0].y < touchBuffer[i].y) 
		{
			if (f1_sign != CAN_MODULE_ENUM_TOUCH_GESTURE_F2_P)
			{
				f1++;
			}
			f1_sign = CAN_MODULE_ENUM_TOUCH_GESTURE_F2_P;
		}
		else 
		{
			if (f1_sign != CAN_MODULE_ENUM_TOUCH_GESTURE_F2_M)
			{
				f1++;
			}
			f1_sign = CAN_MODULE_ENUM_TOUCH_GESTURE_F2_M;
		}
		
		/* Function 3 indicates the sign value of area gap between g(x) and f(x) */
		if (touchBuffer[i].x > min(touchBuffer[startIndex].x, touchBuffer[endIndex].x) && touchBuffer[i].x < max(touchBuffer[startIndex].x, touchBuffer[endIndex].x)) 
		{
		    f3_tmp += f3_yref - (touchBuffer[i].y + touchBuffer[i-1].y)/2;
		}
		
		/* find min an max of x and y */
		if (xMin > touchBuffer[i].x)
		{
			xMin = touchBuffer[i].x;
		}
		if (xMax < touchBuffer[i].x)
		{
			xMax = touchBuffer[i].x;
		}
		if (yMin > touchBuffer[i].y)
		{
			yMin = touchBuffer[i].y;
		}
		if (yMax < touchBuffer[i].y)
		{
			yMax = touchBuffer[i].y;
		}
		
		/* Function 4 represents the sign value of the sum of the x coordinates of all the points that constitute strokes 
		minus the x coordinates of the last point */
		f4_tmp += (int16_t)touchBuffer[i].x - (int16_t)touchBuffer[endIndex].x;
		
		/* Function 5 checks the sign value of the sum of the x coordinates of all the points that constitute strokes 
		minus x coordinates of the first point */
		f5_tmp += (int16_t)touchBuffer[i].x - (int16_t)touchBuffer[startIndex].x;
	}
	
	/* Function 7 indicates the number of intersection points between an input gesture g(x) and all horizontal lines yi, 
	where i is between 0 and n, that constitute a gesture*/
	for (uint8_t j = yMin; j < yMax-1; j++)
	{
		f7_tmp = 0xff;
		f7_sign = 0;
		for (uint8_t i = startIndex+1; i < endIndex-1; i++)
		{
			if (j > touchBuffer[i].y) 
			{
				if (f7_sign != '+')
				{
					f7_tmp++;
				}
				f7_sign = '+';
			}
			else {
				if (f7_sign != '-')
				{
					f7_tmp++;
				}
				f7_sign = '-';
			}
		}
		if (f7_tmp > f7)
		{
			f7 = f7_tmp;
		}
	}
	
	f1--;
	/* Function 2 checks if the y coordinates of the points which exist between the intersection point and the last point are 
	greater than the y coordinates of the intersection point */
	f2 = f1_sign;
	if (f1 == 0)
	{
		f2 = CAN_MODULE_ENUM_TOUCH_GESTURE_F2_U;
	}
	
	/* Function 3 indicates the sign value of area gap between g(x) and f(x) */
	f3 = CAN_MODULE_ENUM_TOUCH_GESTURE_F3_P;
	if (f3_tmp > 0)
	{
		f3 = CAN_MODULE_ENUM_TOUCH_GESTURE_F3_M;
	}
		
	/* Function 9 represents the sign value of the multiplication of x coordinates of the first point and the last point 
	minus the mediate value of x coordinates of all the points */
	f9_tmp = ((touchBuffer[startIndex].x - (xMax+xMin)/2)*(touchBuffer[endIndex].x - (xMax+xMin)/2));
	f9 = CAN_MODULE_ENUM_TOUCH_GESTURE_F9_M;
	if (f9_tmp > 0)
	{
		f9 = CAN_MODULE_ENUM_TOUCH_GESTURE_F9_P;
	}
	
	/* Function 5 checks the sign value of the sum of the x coordinates of all the points that constitute strokes 
	minus x coordinates of the first point */
	f5 = CAN_MODULE_ENUM_TOUCH_GESTURE_F5_M;
	if (f5_tmp > 0)
	{
		f5 = CAN_MODULE_ENUM_TOUCH_GESTURE_F5_P;
	}
	
	/* Function 4 represents the sign value of the sum of the x coordinates of all the points that constitute strokes 
	minus the x coordinates of the last point */
	f4 = CAN_MODULE_ENUM_TOUCH_GESTURE_F4_M;
	if (f4_tmp > 0)
	{
		f4 = CAN_MODULE_ENUM_TOUCH_GESTURE_F4_P;
	}
	
	/* Function 8 checks whether or not x coordinates of all the points except for the first point and the last point is between x0 and xn */
	f8 = CAN_MODULE_ENUM_TOUCH_GESTURE_F8_M;
	if (xMin < min(touchBuffer[startIndex].x,touchBuffer[endIndex].x) || xMax > max(touchBuffer[startIndex].x,touchBuffer[endIndex].x))
	{
		f8 = CAN_MODULE_ENUM_TOUCH_GESTURE_F8_P;
	}
		
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
			parseBuffer(0, rxbufidx-1);
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
			txMsg.Data[1] = adxval;
			txMsg.Data[2] = adyval;

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

	txMsg.Data[0] = NODE_HW_ID_BYTE0;
	txMsg.Data[1] = NODE_HW_ID_BYTE1;
	txMsg.Data[2] = NODE_HW_ID_BYTE2;
	txMsg.Data[3] = NODE_HW_ID_BYTE3;
	
	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;
	
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}
