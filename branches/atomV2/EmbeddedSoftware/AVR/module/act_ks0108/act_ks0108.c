
#include "act_ks0108.h"

static unsigned char Splash_left[] PROGMEM = {
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf,0xe7,0x3,0xf1,0xfa,0xab,0x73,0xb5,0x35,0x36,0x6e,0xfe,0xfe,0xfe,0x6e,0xb4,0x35,0x35,0x71,0xab,0xfb,0xf2,0xf0,0x3,0xf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0x7,0xff,//Line 1
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,0xfc,0xe3,0x1f,0xbf,0x7f,0xfe,0xfe,0xff,0xbf,0x77,0x77,0x77,0xbf,0xff,0xfe,0xfe,0xff,0x7f,0x3f,0xdf,0xe3,0xfc,0xfc,0x81,0x3f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,//Line 2
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0x1f,0x87,0xd9,0xdc,0xdf,0x7,0x1,0xc,0x2f,0x2f,0x76,0x76,0x79,0x7d,0xfb,0xfb,0xfb,0xfb,0x7d,0x79,0x76,0x36,0xf,0xc,0x9,0x7,0xf,0xdf,0xdf,0xd8,0xa7,0x8f,0x3f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,//Line 3
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf,0xf0,0xfe,0xff,0xff,0xff,0x7,0x3,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x3,0x7,0xff,0xff,0xff,0xff,0xf0,0xf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,//Line 4
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0x83,0xfc,0xff,0xff,0xff,0xff,0x9f,0xe0,0xf8,0xf0,0xf0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xf0,0xf0,0xf8,0xe0,0x1f,0xff,0xff,0xff,0xff,0xfc,0x83,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,//Line 5
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,0xff,0xff,0x1f,0xe1,0xfe,0xff,0xff,0x7f,0x1f,0x7,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x7,0x1f,0x7f,0xff,0xff,0xfe,0xe1,0x1f,0xff,0xff,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0x3,0xb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xb,0x3,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfb,0xfc,0xff,//Line 6
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7,0xf8,0xff,0xff,0xf,0xf0,0xff,0xff,0xf,0x13,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x13,0xf,0xff,0xff,0xf0,0xf,0xff,0xff,0xf8,0x7,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,//Line 7
0xff,0xbf,0x5f,0x5f,0x6f,0x77,0x79,0x7e,0x9f,0x7f,0x87,0xf8,0xff,0xff,0xff,0x80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xf8,0xf8,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x80,0xff,0xff,0xff,0xf8,0x87,0x7f,0x9f,0x7e,0x79,0x77,0x6f,0x5f,0x5f,0xbf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f,0x0,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x0,0x7f,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff//Line 7
};
#define SOLID	0
#define TRANSPARENT 1
uint8_t color=GLCD_COLOR_BLACK;
uint8_t Transparent=SOLID;

void numtoascii( int16_t num, char **str );
void signedtoascii(int16_t num, uint8_t decimalplace, char *string, uint8_t numberofdecimals);
void unsignedtoascii(uint16_t num, uint8_t decimalplace, char *string, uint8_t numberofdecimals);

void act_ks0108_Init(void)
{
	///TODO: Initialize hardware etc here
	// Backlight
	TCCR0A |= (1<<COM0A1)|(1<<WGM01)|(1<<WGM00);
	TCCR0B |= (1<<CS00);
	OCR0A = act_ks0108_INITIAL_BACKLIGHT;
	DDRD |= (1<<PD6);

	glcdPowerOn();

	uint16_t ixa = 0;
	uint8_t jxa = 0;
	for (jxa = 0; jxa< 8; jxa++){
		glcdSetXY(0,jxa*8);
		for (ixa = 0; ixa < 128; ixa++){
			glcdWriteData((uint8_t)pgm_read_byte((uint16_t)&Splash_left+ixa+jxa*128), GLCD_COLOR_CLEAR);
		}
	}
	//glcdSetXY(0,0);
	//glcdPutStrTransparent("=_-[]_-=",GLCD_COLOR_SET);
	//glcdSetXY(0,8);
	//glcdPutStrTransparent("--------",GLCD_COLOR_SET);
	//glcdSetXY(0,16);
	//glcdPutStr("-_=[]=-_",GLCD_COLOR_SET);
	//glcdDrawRect(32, 32, 40, 16, GLCD_COLOR_SET);
	//glcdDrawLine(10, 10, 100, 60, GLCD_COLOR_SET);
	//glcdFillRect(90, 10, 20, 20, GLCD_COLOR_SET);
	//glcdInvertRect(28, 10, 20, 20);
	//glcdInvert();
	//glcdDrawRoundRect(85, 5, 30, 30, 5, GLCD_COLOR_SET);
	//glcdDrawCircle(64, 32, 10, 1);
	glcdSetXY(87,8);
	glcdPutStrTransparent("Home-",GLCD_COLOR_SET);
	glcdSetXY(80,16);
	glcdPutStrTransparent("Automa-",GLCD_COLOR_SET);
	glcdSetXY(91,24);
	glcdPutStrTransparent("ion",GLCD_COLOR_SET);
	//glcdDrawLine(10, 62, 50, 5, GLCD_COLOR_CLEAR);
	
}

void act_ks0108_Process(void)
{
	///TODO: Stuff that needs doing is done here
}

void act_ks0108_HandleMessage(StdCan_Msg_t *rxMsg)
{
	StdCan_Msg_t txMsg;
	uint8_t n = 0;

	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_KS0108 &&
		rxMsg->Header.ModuleId == act_ks0108_ID)
	{
	switch (rxMsg->Header.Command)
	{
		case CAN_MODULE_CMD_KS0108_LCD_CLEAR:
		  if (rxMsg->Length == 1) {
			glcdSetColor((0x80&rxMsg->Data[0])>>7);
		  }
		  glcdClear();
		  rxMsg->Data[0] = glcdGetColor()<<7;
		  StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
		  rxMsg->Length = 1;
		  while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
		  break;
		case CAN_MODULE_CMD_KS0108_LCD_INVERT:
		  if (rxMsg->Length == 1) {
			if ((0x80&rxMsg->Data[0])>>7 != glcdGetColor())
				glcdInvert();
		  } else
			glcdInvert();
		  rxMsg->Data[0] = glcdGetColor()<<7;
		  StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
		  rxMsg->Length = 1;
		  while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
		  break;
		case CAN_MODULE_CMD_KS0108_LCD_CURSOR:
		  if (rxMsg->Length == 2) {
			glcdSetXY((0x3f&rxMsg->Data[0])*6, rxMsg->Data[1]*8);
			color = ((0x80&rxMsg->Data[2])>>7);
			Transparent = ((0x40&rxMsg->Data[2])>>6);
		  }
  		  rxMsg->Data[0] = glcdGetColor()<<7;
		  rxMsg->Data[0] = Transparent<<6;
		  rxMsg->Data[0] |= (0x3f & (glcdGetX()/6));
		  rxMsg->Data[1] |= ((glcdGetY()/8));
		  StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
		  rxMsg->Length = 2;
		  while (StdCan_Put(rxMsg) != StdCan_Ret_OK);

		  break;

		case CAN_MODULE_CMD_KS0108_LCD_TEXTAT:
		  glcdSetXY((0x3f&rxMsg->Data[0])*6, rxMsg->Data[1]*8);
		  for (n = 2; n < rxMsg->Length; n++)
		  {
			  if (((0x40&rxMsg->Data[0])>>6)== TRANSPARENT)
				glcdWriteCharTransparent((char)rxMsg->Data[n], (0x80&rxMsg->Data[0])>>7);
			  else
				glcdWriteChar((char)rxMsg->Data[n], (0x80&rxMsg->Data[0])>>7);
		  }
		break;

		case CAN_MODULE_CMD_KS0108_LCD_TEXT:
		for (n = 0; n < rxMsg->Length; n++)
		{
			if (Transparent == TRANSPARENT)
				glcdWriteCharTransparent((char)rxMsg->Data[n], color);
			else
				glcdWriteChar((char)rxMsg->Data[n], color);
		}
		break;

		case CAN_MODULE_CMD_KS0108_LCD_BACKLIGHT:
			if (rxMsg->Length > 0) {
				OCR0A = rxMsg->Data[0];
			}
			StdCan_Msg_t txMsg;
			StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
			StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
			txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_KS0108;
			txMsg.Header.ModuleId = act_ks0108_ID;
			txMsg.Header.Command = CAN_MODULE_CMD_KS0108_LCD_BACKLIGHT;
			txMsg.Length = 1;
			txMsg.Data[0] = OCR0A;
			while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
		break;
		
		case CAN_MODULE_CMD_KS0108_LCD_DRAWRECT:
		  if ((0x40&rxMsg->Data[0])>>6)
			glcdFillRect(rxMsg->Data[1], rxMsg->Data[2], rxMsg->Data[3], rxMsg->Data[4], (0x80&rxMsg->Data[0])>>7);
		  else
			if (rxMsg->Data[5] == 0)
				glcdDrawRect(rxMsg->Data[1], rxMsg->Data[2], rxMsg->Data[3], rxMsg->Data[4], (0x80&rxMsg->Data[0])>>7);
			else
				glcdDrawRoundRect(rxMsg->Data[1], rxMsg->Data[2], rxMsg->Data[3], rxMsg->Data[4], rxMsg->Data[5],(0x80&rxMsg->Data[0])>>7);
		  break;
		
		case CAN_MODULE_CMD_KS0108_LCD_DRAWLINE:
		  glcdDrawLine(rxMsg->Data[1], rxMsg->Data[2], rxMsg->Data[3], rxMsg->Data[4], (0x80&rxMsg->Data[0])>>7);
		  break;

		case CAN_MODULE_CMD_KS0108_LCD_DRAWCIRCLE:
		  glcdDrawCircle(rxMsg->Data[1], rxMsg->Data[2], rxMsg->Data[3], (0x80&rxMsg->Data[0])>>7);
		  break;
	
		case CAN_MODULE_CMD_KS0108_LCD_INVERTRECT:
		  glcdInvertRect(rxMsg->Data[0], rxMsg->Data[1], rxMsg->Data[2], rxMsg->Data[3]);
		  break;

		}
	}
/*
	static uint8_t saer=0;
	static uint8_t saer2=0;
	saer++;
	if (saer>10) {
		saer2++;
		uint8_t ixa = 0;
		uint8_t jxa = 0;
		for (jxa = 0; jxa< 8; jxa++){
			glcdSetXY(0,jxa);
			for (ixa = 0; ixa < 128; ixa++){
				glcdWriteData((uint8_t)pgm_read_byte((uint16_t)&Splash_left+ixa+jxa*128));
			}
		}
		saer=0;
		glcdSetXY(83,1);
		glcdPutStr("Tycker ");
		glcdSetXY(83,2);
		glcdPutStr(" detta ");
		glcdSetXY(83,3);
		glcdPutStr("funkar!");
		glcdSetXY(83,4);
		char buffer[20];	
		//numtoascii((int16_t)saer2 ,*buffer);
		unsignedtoascii((int16_t)saer2,0,buffer,1);
		glcdPutStr(buffer);
	}
*/
}

void act_ks0108_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_KS0108;
	txMsg.Header.ModuleId = act_ks0108_ID;
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

//� la pengi. Skall libbifieras.
	void numtoascii( int16_t num, char **str ) {
		if( num==0 ) return;
		numtoascii( num/10, str );
		**str = '0' + (num%10);
		(*str)++;
	}

	void signedtoascii(int16_t num, uint8_t decimalplace, char *string, uint8_t numberofdecimals){ //decimalplace is number of decimals
		uint8_t i;

		if( num<0 ) {
			*(string++) = '-';
			num = -num;
		}
		numtoascii(num>>decimalplace, &string );
		if(numberofdecimals!=0) *(string++) = '.';
		for(i=0;i<numberofdecimals;i++) {
			num %= 1<<decimalplace;
			num *= 10;
			*(string++) = '0' + (num>>decimalplace);
		}
		*(string++) = 0;
	}

	void unsignedtoascii(uint16_t num, uint8_t decimalplace, char *string, uint8_t numberofdecimals){ //decimalplace is number of decimals
		uint8_t i;
		numtoascii(num>>decimalplace, &string );
		if(numberofdecimals!=0) *(string++) = '.';
		for(i=0;i<numberofdecimals;i++) {
			num %= 1<<decimalplace;
			num *= 10;
			*(string++) = '0' + (num>>decimalplace);
		}
		*(string++) = 0;
	}
