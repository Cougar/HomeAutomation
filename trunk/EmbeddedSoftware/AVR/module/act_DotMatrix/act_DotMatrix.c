
#include "act_DotMatrix.h"

#ifdef act_DotMatrix_USEEEPROM
#include "act_DotMatrix_eeprom.h"
struct eeprom_act_DotMatrix EEMEM eeprom_act_DotMatrix =
{
	{
		/*TODO: Define initialization values on the EEPROM variables here, 
		this will generate a *.eep file that can be used to store this values to the node, 
		can in future be done with a EEPROM module and the make-scrips. 
		Write the values in the exact same order as the struct is defined in the *.h file.
		*/
		0xAB,		// x
		0x1234		// y
		0x12345678	// z
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
};
#endif

uint8_t	act_DotMatrix_framebuf[8][4];
uint8_t act_DotMatrix_row_counter=0;
uint16_t act_DotMatrix_brightness=0;

/* Write a byte on SPI */
void act_DotMatrix_SPIWrite(uint8_t data) {
	uint8_t dummy = 0;
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	// write data
	dummy = UDR0;
	UDR0 = data;
	waitspi();
}

/* Timer callback run periodically to manage rows.
   For each time callback is run, the next row is enabled and the last disabled
 */
void act_DotMatrix_timer_callback(uint8_t timer)
{
	/* Increase row counter */
	if (act_DotMatrix_row_counter++ == 8) {act_DotMatrix_row_counter = 0;}

	/* Disable all rows */
	gpio_clr_pin(act_DotMatrix_ROW_IO1);
	gpio_clr_pin(act_DotMatrix_ROW_IO2);
	gpio_clr_pin(act_DotMatrix_ROW_IO3);
	gpio_clr_pin(act_DotMatrix_ROW_IO4);
	gpio_clr_pin(act_DotMatrix_ROW_IO5);
	gpio_clr_pin(act_DotMatrix_ROW_IO6);
	gpio_clr_pin(act_DotMatrix_ROW_IO7);
	gpio_clr_pin(act_DotMatrix_ROW_IO8);

	/* Write one column of frame buffer to shift registers */
	act_DotMatrix_SPIWrite(act_DotMatrix_framebuf[7-act_DotMatrix_row_counter][3]);
	act_DotMatrix_SPIWrite(act_DotMatrix_framebuf[7-act_DotMatrix_row_counter][2]);
	act_DotMatrix_SPIWrite(act_DotMatrix_framebuf[7-act_DotMatrix_row_counter][1]);
	act_DotMatrix_SPIWrite(act_DotMatrix_framebuf[7-act_DotMatrix_row_counter][0]);
	/* Add more here to support 8-module panels */

	/* Toggle shift register latch */
	gpio_clr_pin(act_DotMatrix_LATCHCLOCK_IO);
	gpio_set_pin(act_DotMatrix_LATCHCLOCK_IO);

	/* Enable one row */
	switch (act_DotMatrix_row_counter)
	{
		case 0:
			gpio_set_pin(act_DotMatrix_ROW_IO1);
			break;
		case 1:
			gpio_set_pin(act_DotMatrix_ROW_IO2);
			break;
		case 2:
			gpio_set_pin(act_DotMatrix_ROW_IO3);
			break;
		case 3:
			gpio_set_pin(act_DotMatrix_ROW_IO4);
			break;
		case 4:
			gpio_set_pin(act_DotMatrix_ROW_IO5);
			break;
		case 5:
			gpio_set_pin(act_DotMatrix_ROW_IO6);
			break;
		case 6:
			gpio_set_pin(act_DotMatrix_ROW_IO7);
			break;
		case 7:
			gpio_set_pin(act_DotMatrix_ROW_IO8);
			break;
			
		default:
			break;
	}
}

void act_DotMatrix_Init(void)
{
#ifdef act_DotMatrix_USEEEPROM
	if (EEDATA_OK)
	{
	  ///TODO: Use stored data to set initial values for the module
	  blablaX = eeprom_read_byte(EEDATA.x);
	  blablaY = eeprom_read_word(EEDATA16.y);
	  blablaZ = eeprom_read_dword(EEDATA32.y);
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_byte_crc(EEDATA.x, 0xAB, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.y, 0x1234, WITHOUT_CRC);
	  eeprom_write_dword_crc(EEDATA32.y, 0x12345678, WITHOUT_CRC);
	  EEDATA_UPDATE_CRC;
	}
#endif

	/* Set up row driver IO as low output */
	gpio_clr_pin(act_DotMatrix_ROW_IO1);
	gpio_set_out(act_DotMatrix_ROW_IO1);
	gpio_clr_pin(act_DotMatrix_ROW_IO2);
	gpio_set_out(act_DotMatrix_ROW_IO2);
	gpio_clr_pin(act_DotMatrix_ROW_IO3);
	gpio_set_out(act_DotMatrix_ROW_IO3);
	gpio_clr_pin(act_DotMatrix_ROW_IO4);
	gpio_set_out(act_DotMatrix_ROW_IO4);
	gpio_clr_pin(act_DotMatrix_ROW_IO5);
	gpio_set_out(act_DotMatrix_ROW_IO5);
	gpio_clr_pin(act_DotMatrix_ROW_IO6);
	gpio_set_out(act_DotMatrix_ROW_IO6);
	gpio_clr_pin(act_DotMatrix_ROW_IO7);
	gpio_set_out(act_DotMatrix_ROW_IO7);
	gpio_clr_pin(act_DotMatrix_ROW_IO8);
	gpio_set_out(act_DotMatrix_ROW_IO8);

	/* Set up PWM controlling brightness */
	TCCR0A |= (1<<COM0B1)|(1<<WGM01)|(1<<WGM00);
	TCCR0B |= (1<<CS00);
	OCR0B = 0xff-act_DotMatrix_INITIAL_BRIGHTNESS;
	DDRD |= (1<<PD5);

	/* Set up output latch clock */
	gpio_clr_pin(act_DotMatrix_LATCHCLOCK_IO);
	gpio_set_out(act_DotMatrix_LATCHCLOCK_IO);

	/* Initialize USART in SPI-mode */
	UBRR0 = 0;
	USART_SPI_XCK_DDR |= (1<<USART_SPI_XCK); // xck (sck) output
	UCSR0C = (1<<UMSEL01)|(1<<UMSEL00)|(0<<UCPHA0)|(0<<UCPOL0);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UBRR0 = 0;

	/* Start timer for row management */
	Timer_SetTimeout(act_DotMatrix_ROW_TIMER, act_DotMatrix_ROW_TIME, TimerTypeFreeRunning, &act_DotMatrix_timer_callback);
	
	/* Clear buffer memory */
	for (uint8_t i=0; i<8; i++)
	{
		for (uint8_t j=0; j<4; j++)
		{
			act_DotMatrix_framebuf[i][j] = act_DotMatrix_INITIAL_ROW;
		}
	}
}

void act_DotMatrix_Process(void)
{
}

void act_DotMatrix_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_DOTMATRIX &&
		rxMsg->Header.ModuleId == act_DotMatrix_ID)
	{
		switch (rxMsg->Header.Command)
		{
		/* If brightness should be adjusted */
		case CAN_MODULE_CMD_PHYSICAL_PWM:
			if (rxMsg->Length == 3) {
				act_DotMatrix_brightness = (rxMsg->Data[1]<<8)+(rxMsg->Data[2]);

				if (OCR0B != 0xff-((uint16_t)(act_DotMatrix_brightness*act_DotMatrix_PWM_FACT)>>8)) {
					cli();	
					OCR0B=0xff-((uint16_t)(act_DotMatrix_brightness*act_DotMatrix_PWM_FACT)>>8);
					sei();
				}
			}
			break;
		/* If data to first module */
		case CAN_MODULE_CMD_DOTMATRIX_IMAGEDATAMODULE1:
			for (uint8_t i=0; i<8; i++)
			{
				act_DotMatrix_framebuf[i][0] = rxMsg->Data[i];
			}
			break;
		/* If data to second module */
		case CAN_MODULE_CMD_DOTMATRIX_IMAGEDATAMODULE2:
			for (uint8_t i=0; i<8; i++)
			{
				act_DotMatrix_framebuf[i][1] = rxMsg->Data[i];
			}
			break;
		/* If data to third module */
		case CAN_MODULE_CMD_DOTMATRIX_IMAGEDATAMODULE3:
			for (uint8_t i=0; i<8; i++)
			{
				act_DotMatrix_framebuf[i][2] = rxMsg->Data[i];
			}
			break;
		/* If data to fourth module */
		case CAN_MODULE_CMD_DOTMATRIX_IMAGEDATAMODULE4:
			for (uint8_t i=0; i<8; i++)
			{
				act_DotMatrix_framebuf[i][3] = rxMsg->Data[i];
			}
			break;
		}
	}
}

void act_DotMatrix_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_DOTMATRIX;
	txMsg.Header.ModuleId = act_DotMatrix_ID;
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
