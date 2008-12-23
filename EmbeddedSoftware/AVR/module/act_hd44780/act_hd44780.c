
#include "act_hd44780.h"

uint8_t Command = 0;

void act_hd44780_Init(void)
{
	///FIXME: What is this, this needs to be more clear by far....

	// Contrast
	TCCR0A |= (1<<COM0B1)|(1<<WGM01)|(1<<WGM00);
	TCCR0B |= (1<<CS00);
	OCR0B = 0x10;
	DDRD |= (1<<DDD5);


	// Backlight
	TCCR1A |= (1<<COM1A1)|(1<<WGM11);
	TCCR1B |= (1<<WGM13)|(1<<WGM12)|(1<<CS10);
	ICR1 = 0xFF; // Make it 8 bits
	OCR1A = act_hd44780_INITIAL_BACKLIGHT;
	DDRB |= (1<<DDB1);


	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	lcd_puts("HomeAutomation\n");
	lcd_puts("HD44780-module\n");
}

void act_hd44780_Process(void)
{
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

		StdCan_Put(&txMsg);
		break;

		case CAN_MODULE_CMD_HD44789_LCD_BACKLIGHT:
		if (rxMsg->Length > 0)
			OCR1AL = rxMsg->Data[0];

		StdCan_Msg_t txMsg;

		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_HD44789;
		txMsg.Header.ModuleId = act_hd44780_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_HD44789_LCD_BACKLIGHT;
		txMsg.Length = 1;

		txMsg.Data[0] = OCR1AL;

		StdCan_Put(&txMsg);
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

	txMsg.Data[0] = NODE_HW_ID_BYTE0;
	txMsg.Data[1] = NODE_HW_ID_BYTE1;
	txMsg.Data[2] = NODE_HW_ID_BYTE2;
	txMsg.Data[3] = NODE_HW_ID_BYTE3;

	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;

	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}
