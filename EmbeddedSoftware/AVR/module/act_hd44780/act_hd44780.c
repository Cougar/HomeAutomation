
#include "act_hd44780.h"

uint8_t Command = 0;
#if act_hd44780_USE_AUTO_BL == 1
uint8_t autoMode = CAN_MODULE_ENUM_HD44789_LCD_BACKLIGHT_AUTOLIGHT_ON;
#endif
void act_hd44780_Init(void)
{
	///FIXME: What is this, this needs to be more clear by far....

	// Contrast (brightness on an oled display)
	TCCR0A |= (1<<COM0B1)|(1<<WGM01)|(1<<WGM00);
	TCCR0B |= (1<<CS00);
	OCR0B = act_hd44780_INITIAL_CONTRAST;
	DDRD |= (1<<PD5);

#if (act_hd44780_TYPE==0)
	// Backlight
	TCCR0A |= (1<<COM0A1)|(1<<WGM01)|(1<<WGM00);
	TCCR0B |= (1<<CS00);
	OCR0A = act_hd44780_INITIAL_BACKLIGHT;
	DDRD |= (1<<PD6);
#endif

	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	lcd_puts("HomeAutomation\n");
	lcd_puts("HD44780-module\n");
}

void act_hd44780_Process(void)
{
#if act_hd44780_USE_AUTO_BL == 1
  if (autoMode == CAN_MODULE_ENUM_HD44789_LCD_BACKLIGHT_AUTOLIGHT_ON) {
    #if (act_hd44780_TYPE==0)
      if ( OCR0A == 0) {
    #else
      if ( OCR0B == 0) {
    #endif

      } else {
      uint16_t Voltage = (0x3ff & ADC_Get(act_hd44780_LIGHTSENSOR_AD));
    #if (act_hd44780_TYPE==0)
	OCR0A = Voltage/4;
	if (OCR0A == 0) {
	  OCR0A = 1;
	}
    #else
	OCR0B = Voltage/4;
	if (OCR0B == 0) {
	  OCR0B = 1;
	}
    #endif
      }
  }
#endif


#if (act_hd44780_TYPE==0)
	if (OCR0A==0 && (TCCR0A & ((1<<COM0A1)|(1<<WGM01)|(1<<WGM00))) == ((1<<COM0A1)|(1<<WGM01)|(1<<WGM00)))
	{
		TCCR0A &= ~((1<<COM0A1)|(1<<WGM01)|(1<<WGM00));
	}
	if (OCR0A!=0 && (TCCR0A & ((1<<COM0A1)|(1<<WGM01)|(1<<WGM00))) != ((1<<COM0A1)|(1<<WGM01)|(1<<WGM00)))
	{
		TCCR0A |= (1<<COM0A1)|(1<<WGM01)|(1<<WGM00);
	}
#endif
		
		

		
		
}

void act_hd44780_HandleMessage(StdCan_Msg_t *rxMsg)
{
	StdCan_Msg_t txMsg;
	uint8_t n = 0;

	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_HD44789 &&
		rxMsg->Header.ModuleId == act_hd44780_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_HD44789_LCD_CLEAR:
		lcd_clrscr();
		break;

		case CAN_MODULE_CMD_HD44789_LCD_CURSOR:
		lcd_gotoxy(rxMsg->Data[0], rxMsg->Data[1]);
		break;

		case CAN_MODULE_CMD_HD44789_LCD_TEXTAT:
		lcd_gotoxy(rxMsg->Data[0], rxMsg->Data[1]);
		for (n = 2; n < rxMsg->Length; n++)
		{
			lcd_putc((char)rxMsg->Data[n]);
		}
		break;
/*
		case CAN_MODULE_CMD_HD44789_LCD_CLEARROW:
		lcd_gotoxy(0, rxMsg->Data[0]);
		for (n = 0; n < act_hd44780_WIDTH; n++)
		{
			lcd_putc(' ');
		}
		break;
*/
		case CAN_MODULE_CMD_HD44789_LCD_TEXT:
		for (n = 0; n < rxMsg->Length; n++)
		{
			lcd_putc((char)rxMsg->Data[n]);
		}
		break;

		case CAN_MODULE_CMD_HD44789_LCD_SIZE:
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_HD44789;
		txMsg.Header.ModuleId = act_hd44780_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_HD44789_LCD_SIZE;
		txMsg.Length = 2;

		txMsg.Data[0] = act_hd44780_WIDTH;
		txMsg.Data[1] = act_hd44780_HEIGHT;

		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
		break;

		case CAN_MODULE_CMD_HD44789_LCD_BACKLIGHT:
		if (rxMsg->Length > 0) {
#if (act_hd44780_TYPE==0)
			OCR0A = rxMsg->Data[0];
#else
			OCR0B = rxMsg->Data[0];
#endif
#if act_hd44780_USE_AUTO_BL == 1
			if (rxMsg->Length == 2) {
				autoMode = rxMsg->Data[0]; 
			}
#endif
		}

		StdCan_Msg_t txMsg;

		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_HD44789;
		txMsg.Header.ModuleId = act_hd44780_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_HD44789_LCD_BACKLIGHT;
		txMsg.Length = 1;

#if (act_hd44780_TYPE==0)
			txMsg.Data[0] = OCR0A;
#else
			txMsg.Data[0] = OCR0B;
#endif
#if act_hd44780_USE_AUTO_BL == 1
		txMsg.Data[1] = autoMode;
		txMsg.Length = 2;
#endif
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
		break;
		}
	}
}

void act_hd44780_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_HD44789;
	txMsg.Header.ModuleId = act_hd44780_ID;
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
