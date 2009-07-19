
#include "sns_Touch.h"
point touchBuffer[sns_Touch_BUFFERSIZE];
uint8_t rxbufidx;
uint8_t pushStatus;

point lastValidPoint;

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
}


/*
F1:	Klar
F2:	Klar
F3:	Klar
F4:	Klar
F5:	Klar
F6:	Klar
F7:	Klar
F8:	Klar? (kontrollera funktion)
F9:	
*/

/*
Touch gesture parser
Implementation of 'A new gesture recognition algorithm and segmentation 
method of Korean scripts for gesture-allowed ink editor' by Mi Gyung Cho

Call function parseBuffer with a buffer containing x/y coordinates, the buffer start and end.
Returns function results that describe the gesture in 24bits

Written by Linus Lundin, Jonas Andersson and Anders Runeson, 2009

To fix:
Break out to a driver file
*/
gesture parseBuffer(point *buffer, uint8_t startIndex, uint8_t endIndex)
{
	int16_t f1_k = 0, f3_tmp = 0, f4_tmp = 0, f5_tmp = 0, f9_tmp = 0;
	uint8_t f7_tmp = 0, f7_sign = 0, f1_sign = 0, f3_yref = 0, xMin, xMax, yMin, yMax;
	gesture functionData;
	functionData.f1=0;
	functionData.f7=0;
	
	/* Function 6 indicates the sign value of the y coordinates of the last point minus y coordinates of the first point */
	functionData.f6 = CAN_MODULE_ENUM_TOUCH_GESTURE_F6_M;
	if ((int16_t)buffer[endIndex].y - (int16_t)buffer[startIndex].y > 0)
	{
		functionData.f6 = CAN_MODULE_ENUM_TOUCH_GESTURE_F6_P;
	}
	
	xMin = buffer[startIndex].x;
	xMax = buffer[startIndex].x;
	yMin = buffer[startIndex].y;
	yMax = buffer[startIndex].y;
	f3_yref = (buffer[startIndex].y + buffer[endIndex].y)/2;
	f1_k = ((buffer[endIndex].y-buffer[startIndex].y)<<4)/(buffer[endIndex].x-buffer[startIndex].x);
	f1_sign = CAN_MODULE_ENUM_TOUCH_GESTURE_F2_U;

	for (uint8_t i = startIndex+1; i < endIndex-1; i++)
	{
		/* Function 1 indicates the number of intersection points between an input gesture g(x) and the straight line f(x) 
		which connects the first and last point of the gesture */
		if (((f1_k*(buffer[i].x-buffer[0].x))>>4) + buffer[0].y < buffer[i].y) 
		{
			if (f1_sign != CAN_MODULE_ENUM_TOUCH_GESTURE_F2_P)
			{
				functionData.f1++;
			}
			f1_sign = CAN_MODULE_ENUM_TOUCH_GESTURE_F2_P;
		}
		else 
		{
			if (f1_sign != CAN_MODULE_ENUM_TOUCH_GESTURE_F2_M)
			{
				functionData.f1++;
			}
			f1_sign = CAN_MODULE_ENUM_TOUCH_GESTURE_F2_M;
		}
		
		/* Function 3 indicates the sign value of area gap between g(x) and f(x) */
		if (buffer[i].x > min(buffer[startIndex].x, buffer[endIndex].x) && buffer[i].x < max(buffer[startIndex].x, buffer[endIndex].x)) 
		{
		    f3_tmp += f3_yref - (buffer[i].y + buffer[i-1].y)/2;
		}
		
		/* find min an max of x and y */
		if (xMin > buffer[i].x)
		{
			xMin = buffer[i].x;
		}
		if (xMax < buffer[i].x)
		{
			xMax = buffer[i].x;
		}
		if (yMin > buffer[i].y)
		{
			yMin = buffer[i].y;
		}
		if (yMax < buffer[i].y)
		{
			yMax = buffer[i].y;
		}
		
		/* Function 4 represents the sign value of the sum of the x coordinates of all the points that constitute strokes 
		minus the x coordinates of the last point */
		f4_tmp += (int16_t)buffer[i].x - (int16_t)buffer[endIndex].x;
		
		/* Function 5 checks the sign value of the sum of the x coordinates of all the points that constitute strokes 
		minus x coordinates of the first point */
		f5_tmp += (int16_t)buffer[i].x - (int16_t)buffer[startIndex].x;
	}
	
	/* Function 7 indicates the number of intersection points between an input gesture g(x) and all horizontal lines yi, 
	where i is between 0 and n, that constitute a gesture*/
	for (uint8_t j = yMin; j < yMax-1; j++)
	{
		f7_tmp = 0xff;
		f7_sign = 0;
		for (uint8_t i = startIndex+1; i < endIndex-1; i++)
		{
			if (j > buffer[i].y) 
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
		if (f7_tmp > functionData.f7)
		{
			functionData.f7 = f7_tmp;
		}
	}
	
	functionData.f1--;
	/* Function 2 checks if the y coordinates of the points which exist between the intersection point and the last point are 
	greater than the y coordinates of the intersection point */
	functionData.f2 = f1_sign;
	if (functionData.f1 == 0)
	{
		functionData.f2 = CAN_MODULE_ENUM_TOUCH_GESTURE_F2_U;
	}
	
	/* Function 3 indicates the sign value of area gap between g(x) and f(x) */
	functionData.f3 = CAN_MODULE_ENUM_TOUCH_GESTURE_F3_P;
	if (f3_tmp > 0)
	{
		functionData.f3 = CAN_MODULE_ENUM_TOUCH_GESTURE_F3_M;
	}
		
	/* Function 9 represents the sign value of the multiplication of x coordinates of the first point and the last point 
	minus the mediate value of x coordinates of all the points */
	f9_tmp = ((buffer[startIndex].x - (xMax+xMin)/2)*(buffer[endIndex].x - (xMax+xMin)/2));
	functionData.f9 = CAN_MODULE_ENUM_TOUCH_GESTURE_F9_M;
	if (f9_tmp > 0)
	{
		functionData.f9 = CAN_MODULE_ENUM_TOUCH_GESTURE_F9_P;
	}
	
	/* Function 5 checks the sign value of the sum of the x coordinates of all the points that constitute strokes 
	minus x coordinates of the first point */
	functionData.f5 = CAN_MODULE_ENUM_TOUCH_GESTURE_F5_M;
	if (f5_tmp > 0)
	{
		functionData.f5 = CAN_MODULE_ENUM_TOUCH_GESTURE_F5_P;
	}
	
	/* Function 4 represents the sign value of the sum of the x coordinates of all the points that constitute strokes 
	minus the x coordinates of the last point */
	functionData.f4 = CAN_MODULE_ENUM_TOUCH_GESTURE_F4_M;
	if (f4_tmp > 0)
	{
		functionData.f4 = CAN_MODULE_ENUM_TOUCH_GESTURE_F4_P;
	}
	
	/* Function 8 checks whether or not x coordinates of all the points except for the first point and the last point is between x0 and xn */
	functionData.f8 = CAN_MODULE_ENUM_TOUCH_GESTURE_F8_M;
	if (xMin < min(buffer[startIndex].x,buffer[endIndex].x) || xMax > max(buffer[startIndex].x,buffer[endIndex].x))
	{
		functionData.f8 = CAN_MODULE_ENUM_TOUCH_GESTURE_F8_P;
	}
		
	//printf("1%u2%c3%c4%c5%c6%c7%u8%c9%c\n", f1, f2, f3, f4, f5, f6, f7, f8, f9);

	return functionData;
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

	txMsg.Data[0] = NODE_HW_ID_BYTE0;
	txMsg.Data[1] = NODE_HW_ID_BYTE1;
	txMsg.Data[2] = NODE_HW_ID_BYTE2;
	txMsg.Data[3] = NODE_HW_ID_BYTE3;
	
	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;
	
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}
