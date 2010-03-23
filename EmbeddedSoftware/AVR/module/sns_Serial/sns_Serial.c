
#include "sns_Serial.h"

#ifdef sns_Serial_USEEEPROM
#include "sns_Serial_eeprom.h"
struct eeprom_sns_Serial EEMEM eeprom_sns_Serial =
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file.
		0xAB,	// x
		0x1234	// y
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
};
#endif

// types
typedef enum
{
	SNS_SERIAL_PHYFORMAT_RS232 = 0,
	SNS_SERIAL_PHYFORMAT_RS485 = 1,
	SNS_SERIAL_PHYFORMAT_LOOPBACK = 2
} snsSerialPhyFormat_t;

// internal variables
static uint16_t baudRate = 9600;
static snsSerialPhyFormat_t format = SNS_SERIAL_PHYFORMAT_RS232;

void sns_Serial_Init(void)
{
#ifdef sns_Serial_USEEEPROM
	if (EEDATA_OK)
	{
	  ///TODO: Use stored data to set initial values for the module
	  blablaX = eeprom_read_byte(EEDATA.x);
	  blablaY = eeprom_read_word(EEDATA16.y);
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_byte_crc(EEDATA.x, 0xAB, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.y, 0x1234, WITHOUT_CRC);
	  EEDATA_UPDATE_CRC;
	}
#endif
	// default baudrate
	uart_init(UART_BAUD_SELECT(baudRate, F_CPU));
	// configure control pins
	gpio_set_out(sns_Serial_RXEN);
	gpio_set_out(sns_Serial_TXEN);
	gpio_set_out(sns_Serial_ON);
	gpio_set_out(sns_Serial_485_232);
	gpio_set_out(sns_Serial_TERM_EN);
	gpio_set_out(sns_Serial_485_CONNECT);
	//default to loopback mode until config command received
	gpio_clr_pin(sns_Serial_ON);
	gpio_clr_pin(sns_Serial_485_232);
	gpio_clr_pin(sns_Serial_485_CONNECT);	// disable RS485_CONNECT
	gpio_clr_pin(sns_Serial_TERM_EN);		// disable termination

	// to use PCINt lib, call this function: (the callback function look as a timer callback function)
	// Pcint_SetCallbackPin(sns_Serial_PCINT, EXP_C , &sns_Serial_pcint_callback);

}

void sns_Serial_Process(void)
{
	// prepare a new message, in case new data is available
	StdCan_Msg_t msg;
	msg.Length = 0; // no data so far
	StdCan_Set_class(msg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(msg.Header, DIRECTIONFLAG_TO_OWNER);
	msg.Header.ModuleType = CAN_MODULE_TYPE_SNS_SERIAL;
	msg.Header.ModuleId = sns_Serial_ID;
	msg.Header.Command = CAN_MODULE_CMD_SERIAL_SERIALDATA;
	
	unsigned char status;
	
	do
	{
		// ask uart driver for more data
		unsigned int data = uart_getc();
		// character is contained in LSB
		unsigned char c = (unsigned char)(data & 0x00FF);
		// status is contained in MSB
		status = (unsigned char)((data & 0xFF00) >> 8);
		
		// status == 0 means we just received a new char
		if (status == 0)
		{
			// insert it into the message
			msg.Data[(uint8_t)msg.Length] = c;
			msg.Length++;
		}
	// keep going until max data length reached, or until uart RXBUF is empty
	} while (status == 0 && msg.Length < 8);
	
	// did we fill any data into the message?
	if (msg.Length > 0)
	{
		// transmit this chunk of data (max 8 chars)
		while(StdCan_Put(&msg) != StdCan_Ret_OK);
	}
}

void sns_Serial_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_SERIAL &&
		rxMsg->Header.ModuleId == sns_Serial_ID)
	{
		switch (rxMsg->Header.Command)
		{
			case CAN_MODULE_CMD_SERIAL_SERIALDATA:
				for (uint8_t i=0; i<rxMsg->Length; i++)
				{
					uart_putc(rxMsg->Data[i]);
				}
				break;
			case CAN_MODULE_CMD_SERIAL_SERIALCONFIG:
				baudRate = ((uint16_t)rxMsg->Data[0] << 0) | ((uint16_t)rxMsg->Data[1] << 8);
				format = (snsSerialPhyFormat_t)rxMsg->Data[2];
				uart_init(UART_BAUD_SELECT(baudRate, F_CPU));
				if (format == CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_RS232)
				{
					gpio_clr_pin(sns_Serial_485_CONNECT);	// disable RS485_CONNECT
					gpio_clr_pin(sns_Serial_TERM_EN);		// disable termination
					gpio_clr_pin(sns_Serial_485_232);		// RS232 mode
					gpio_set_pin(sns_Serial_RXEN);			// enable RX
					gpio_set_pin(sns_Serial_TXEN);			// enable TX
					gpio_set_pin(sns_Serial_ON);			// enable charge pump
				}
				else if (format == CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_RS485FULLDUPLEX)
				{
					gpio_clr_pin(sns_Serial_485_CONNECT);	// disable RS485_CONNECT
					gpio_clr_pin(sns_Serial_TERM_EN);		// disable termination
					gpio_set_pin(sns_Serial_485_232);		// RS485 mode
					gpio_set_pin(sns_Serial_RXEN);			// enable RX
					gpio_set_pin(sns_Serial_TXEN);			// enable TX
					gpio_set_pin(sns_Serial_ON);			// enable charge pump
				}
				else if (format == CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_RS485HALFDUPLEX)
				{
					gpio_set_pin(sns_Serial_485_CONNECT);	// enable RS485_CONNECT
					gpio_clr_pin(sns_Serial_TERM_EN);		// disable termination
					gpio_set_pin(sns_Serial_485_232);		// RS485 mode
					gpio_set_pin(sns_Serial_RXEN);			// enable RX
					gpio_clr_pin(sns_Serial_TXEN);			// disable TX
					gpio_set_pin(sns_Serial_ON);			// enable charge pump
				}
				else if (format == CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_RS485HALFDUPLEXWITHTERMINATION)
				{
					gpio_set_pin(sns_Serial_485_CONNECT);	// enable RS485_CONNECT
					gpio_set_pin(sns_Serial_TERM_EN);		// enable termination
					gpio_set_pin(sns_Serial_485_232);		// RS485 mode
					gpio_set_pin(sns_Serial_RXEN);			// enable RX
					gpio_clr_pin(sns_Serial_TXEN);			// disable TX
					gpio_set_pin(sns_Serial_ON);			// enable charge pump
				}
				else if (format == CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_LOOPBACK)
				{
					gpio_clr_pin(sns_Serial_485_CONNECT);	// disable RS485_CONNECT
					gpio_clr_pin(sns_Serial_TERM_EN);		// disable termination
					gpio_set_pin(sns_Serial_485_232);		// RS232 mode
					gpio_set_pin(sns_Serial_RXEN);			// enable RX
					gpio_set_pin(sns_Serial_TXEN);			// enable TX
					gpio_clr_pin(sns_Serial_ON);			// disable charge pump (i.e. enable loopback)
				}
				else
				{
					//invalid format
				}
				break;
		}
	}
}

void sns_Serial_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_SERIAL;
	txMsg.Header.ModuleId = sns_Serial_ID;
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
