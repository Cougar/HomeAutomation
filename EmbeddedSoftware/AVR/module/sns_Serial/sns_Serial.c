
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

// internal variables
static uint32_t baudRate = 9600;
static uint16_t format = CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_LOOPBACK;

// prepare a new message, in case new data is available
static StdCan_Msg_t msg;

#define sns_Serial_DEBUG 0

void sns_Serial_Init(void)
{
#ifdef sns_Serial_USEEEPROM
	if (EEDATA_OK) {
#if ((__AVR_LIBC_MAJOR__ == 1  && __AVR_LIBC_MINOR__ == 6 && __AVR_LIBC_REVISION__ >=2)||(__AVR_LIBC_MAJOR__ == 1  && __AVR_LIBC_MINOR__ > 6)||__AVR_LIBC_MAJOR__ > 1)
		baudRate = eeprom_read_dword(EEDATA32.baudRate);
#else
#warning This version of AVR-libc does not have support for eeprom read dword
#endif
		format = eeprom_read_word(EEDATA16.format);
	}
	else {
		//The CRC of the EEPROM is not correct, store default values and update CRC
#if ((__AVR_LIBC_MAJOR__ == 1  && __AVR_LIBC_MINOR__ == 6 && __AVR_LIBC_REVISION__ >=2)||(__AVR_LIBC_MAJOR__ == 1  && __AVR_LIBC_MINOR__ > 6)||__AVR_LIBC_MAJOR__ > 1)
		eeprom_write_dword_crc(EEDATA32.baudRate, 9600, WITHOUT_CRC);
#else
#warning This version of AVR-libc does not have support for eeprom write dword
#endif
		eeprom_write_word_crc(EEDATA16.format, CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_LOOPBACK, WITHOUT_CRC);
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
	gpio_set_pin(sns_Serial_485_232);		// RS232 mode
	gpio_set_pin(sns_Serial_RXEN);			// enable RX
	gpio_set_pin(sns_Serial_TXEN);			// enable TX

	// to use PCINt lib, call this function: (the callback function look as a timer callback function)
	// Pcint_SetCallbackPin(sns_Serial_PCINT, EXP_C , &sns_Serial_pcint_callback);

#if sns_Serial_DEBUG==1
	printf("Serial started!\n");
#endif

	msg.Length = 0; // no data so far
}

//unsigned char sData[8];
//uint8_t dataCnt=0;

void sns_Serial_Process(void)
{
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
#if sns_Serial_DEBUG==1
			printf("RX\n");
#endif
			// insert it into the message
			msg.Data[(uint8_t)msg.Length] = c;
			msg.Length++;
			
			Timer_SetTimeout(sns_Serial_FORCE_SEND_TIMER, sns_Serial_FORCE_SEND_TIME_MS , TimerTypeOneShot, 0);
		}
	// keep going until max data length reached, or until uart RXBUF is empty
	} while (status == 0 && msg.Length < 8);
	
	// check if force send or send-frame full
	if (Timer_Expired(sns_Serial_FORCE_SEND_TIMER) || msg.Length == 8) 
	{
		// transmit this chunk of data (max 8 chars)
		while(StdCan_Put(&msg) != StdCan_Ret_OK);
		msg.Length = 0; // no data so far
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
#if sns_Serial_DEBUG==1
				printf("TX\n");
#endif
				gpio_set_pin(sns_Serial_TXEN);			// enable TX
				for (uint8_t i=0; i<rxMsg->Length; i++)
				{
					uart_putc(rxMsg->Data[i]);
				}
				/* TODO Cannot disable TX yet, the data has only been placed in buffer but not sent */
//				gpio_clr_pin(sns_Serial_TXEN);			// disable TX
				break;
			case CAN_MODULE_CMD_SERIAL_SERIALCONFIG:
				baudRate = 10 * (((uint32_t)rxMsg->Data[1] << 0) | ((uint32_t)rxMsg->Data[0] << 8));
				format = (uint16_t)rxMsg->Data[2];
				uart_init(UART_BAUD_SELECT(baudRate, F_CPU));
				if (format == CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_RS232)
				{
#if sns_Serial_DEBUG==1
					printf("RS232\n");
#endif
					gpio_clr_pin(sns_Serial_485_CONNECT);	// disable RS485_CONNECT
					gpio_clr_pin(sns_Serial_TERM_EN);		// disable termination
					gpio_clr_pin(sns_Serial_485_232);		// RS232 mode
					gpio_set_pin(sns_Serial_RXEN);			// enable RX
					gpio_set_pin(sns_Serial_ON);			// enable charge pump
				}
				else if (format == CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_RS485FULLDUPLEX)
				{
#if sns_Serial_DEBUG==1
					printf("RS485FD\n");
#endif
					gpio_clr_pin(sns_Serial_485_CONNECT);	// disable RS485_CONNECT
					gpio_clr_pin(sns_Serial_TERM_EN);		// disable termination
					gpio_set_pin(sns_Serial_485_232);		// RS485 mode
					gpio_set_pin(sns_Serial_RXEN);			// enable RX
					gpio_set_pin(sns_Serial_TXEN);			// enable TX
					gpio_set_pin(sns_Serial_ON);			// enable charge pump
				}
				else if (format == CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_RS485HALFDUPLEX)
				{
#if sns_Serial_DEBUG==1
					printf("RS485HD\n");
#endif
					gpio_set_pin(sns_Serial_485_CONNECT);	// enable RS485_CONNECT
					gpio_clr_pin(sns_Serial_TERM_EN);		// disable termination
					gpio_set_pin(sns_Serial_485_232);		// RS485 mode
					gpio_set_pin(sns_Serial_RXEN);			// enable RX
				/* TODO shoud the TX really be enabled in half duplex? */
					gpio_set_pin(sns_Serial_TXEN);			// enable TX
					gpio_set_pin(sns_Serial_ON);			// enable charge pump
				}
				else if (format == CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_RS485HALFDUPLEXWITHTERMINATION)
				{
#if sns_Serial_DEBUG==1
					printf("485HDT\n");
#endif
					gpio_set_pin(sns_Serial_485_CONNECT);	// enable RS485_CONNECT
					gpio_set_pin(sns_Serial_TERM_EN);		// enable termination
					gpio_set_pin(sns_Serial_485_232);		// RS485 mode
				/* TODO shoud the TX really be enabled in half duplex? */
					gpio_set_pin(sns_Serial_RXEN);			// enable RX
					gpio_set_pin(sns_Serial_TXEN);			// enable TX
					gpio_set_pin(sns_Serial_ON);			// enable charge pump
				}
				else if (format == CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_LOOPBACK)
				{
#if sns_Serial_DEBUG==1
					printf("LPMODE\n");
#endif
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
#if sns_Serial_DEBUG==1
					printf("ERROR!\n");
#endif
					//break prematurely, to prevent data from being saved in EEPROM
					break;
				}
				
				// update EEPROM data
#ifdef sns_Serial_USEEEPROM
#if ((__AVR_LIBC_MAJOR__ == 1  && __AVR_LIBC_MINOR__ == 6 && __AVR_LIBC_REVISION__ >=2)||(__AVR_LIBC_MAJOR__ == 1  && __AVR_LIBC_MINOR__ > 6)||__AVR_LIBC_MAJOR__ > 1)
				eeprom_write_dword_crc(EEDATA32.baudRate, 9600, WITHOUT_CRC);
#else
#warning This version of AVR-libc does not have support for eeprom read dword
#endif
				eeprom_write_word_crc(EEDATA16.format, CAN_MODULE_ENUM_SERIAL_SERIALCONFIG_PHYSICALFORMAT_LOOPBACK, WITHOUT_CRC);
				EEDATA_UPDATE_CRC;
#endif
				
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

