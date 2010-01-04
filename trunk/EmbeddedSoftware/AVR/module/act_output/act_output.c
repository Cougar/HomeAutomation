
#include "act_output.h"
uint8_t chnValue[8];
#ifdef act_output_USEEEPROM
#include "act_output_eeprom.h"
struct eeprom_act_output EEMEM eeprom_act_output = 
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file. 
#ifdef	act_output_CH0
		0x00,
#endif
#ifdef	act_output_CH1
		0x00,
#endif
#ifdef	act_output_CH2
		0x00,
#endif
#ifdef	act_output_CH3
		0x00,
#endif
#ifdef	act_output_CH4
		0x00,
#endif
#ifdef	act_output_CH5
		0x00,
#endif
#ifdef	act_output_CH6
		0x00,
#endif
#ifdef	act_output_CH7
		0x00,
#endif
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 
#endif

void act_output_Init(void)
{
#ifdef act_output_USEEEPROM
	if (EEDATA_OK)
	{
	  ///TODO: Use stored data to set initial values for the module
	  uint8_t index = 0;
#ifdef	act_output_CH0
		chnValue[index] = eeprom_read_byte(EEDATA.ch0);
		index++;
#endif
#ifdef	act_output_CH1
		chnValue[index] = eeprom_read_byte(EEDATA.ch1);
		index++;
#endif
#ifdef	act_output_CH2
		chnValue[index] = eeprom_read_byte(EEDATA.ch2);
		index++;
#endif
#ifdef	act_output_CH3
		chnValue[index] = eeprom_read_byte(EEDATA.ch3);
		index++;
#endif
#ifdef	act_output_CH4
		chnValue[index] = eeprom_read_byte(EEDATA.ch4);
		index++;
#endif
#ifdef	act_output_CH5
		chnValue[index] = eeprom_read_byte(EEDATA.ch5);
		index++;
#endif
#ifdef	act_output_CH6
		chnValue[index] = eeprom_read_byte(EEDATA.ch6);
		index++;
#endif
#ifdef	act_output_CH7
		chnValue[index] = eeprom_read_byte(EEDATA.ch7);
		index++;
#endif	  
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
#ifdef	act_output_CH0
		eeprom_write_byte_crc(EEDATA.ch0, 0x00, WITHOUT_CRC);
#endif
#ifdef	act_output_CH1
		eeprom_write_byte_crc(EEDATA.ch1, 0x00, WITHOUT_CRC);
#endif
#ifdef	act_output_CH2
		eeprom_write_byte_crc(EEDATA.ch2, 0x00, WITHOUT_CRC);
#endif
#ifdef	act_output_CH3
		eeprom_write_byte_crc(EEDATA.ch3, 0x00, WITHOUT_CRC);
#endif
#ifdef	act_output_CH4
		eeprom_write_byte_crc(EEDATA.ch4, 0x00, WITHOUT_CRC);
#endif
#ifdef	act_output_CH5
		eeprom_write_byte_crc(EEDATA.ch5, 0x00, WITHOUT_CRC);
#endif
#ifdef	act_output_CH6
		eeprom_write_byte_crc(EEDATA.ch6, 0x00, WITHOUT_CRC);
#endif
#ifdef	act_output_CH7
		eeprom_write_byte_crc(EEDATA.ch7, 0x00, WITHOUT_CRC);
#endif	  
		EEDATA_UPDATE_CRC;
	}
#endif  
	///Initialize hardware
	uint8_t index = 0;
#ifdef	act_output_CH0		
		gpio_set_statement(chnValue[index],act_output_CH0);
		gpio_set_out(act_output_CH0);
		index++;
#endif
#ifdef	act_output_CH1
		gpio_set_statement(chnValue[index],act_output_CH1);
		gpio_set_out(act_output_CH1);
		index++;
#endif
#ifdef	act_output_CH2
		gpio_set_statement(chnValue[index],act_output_CH2);
		gpio_set_out(act_output_CH2);
		index++;
#endif
#ifdef	act_output_CH3
		gpio_set_statement(chnValue[index],act_output_CH3);
		gpio_set_out(act_output_CH3);
		index++;
#endif
#ifdef	act_output_CH4
		gpio_set_statement(chnValue[index],act_output_CH4);
		gpio_set_out(act_output_CH4);
		index++;
#endif
#ifdef	act_output_CH5
		gpio_set_statement(chnValue[index],act_output_CH5);
		gpio_set_out(act_output_CH5);
		index++;
#endif
#ifdef	act_output_CH6
		gpio_set_statement(chnValue[index],act_output_CH6);
		gpio_set_out(act_output_CH6);
		index++;
#endif
#ifdef	act_output_CH7
		gpio_set_statement(chnValue[index],act_output_CH7);
		gpio_set_out(act_output_CH7);
		index++;
#endif
	// to use PCINt lib, call this function: (the callback function look as a timer callback function)
	// Pcint_SetCallbackPin(act_output_PCINT, EXP_C , &act_output_pcint_callback);

}

void act_output_Process(void)
{
	if (Timer_Expired(act_output_STORE_VALUE_TIMEOUT))
	{
		uint8_t index = 0;
#ifdef	act_output_CH0
		eeprom_write_byte_crc(EEDATA.ch0, chnValue[index], WITHOUT_CRC);
		index++;
#endif
#ifdef	act_output_CH1
		eeprom_write_byte_crc(EEDATA.ch1, chnValue[index], WITHOUT_CRC);
		index++;
#endif
#ifdef	act_output_CH2
		eeprom_write_byte_crc(EEDATA.ch2, chnValue[index], WITHOUT_CRC);
		index++;
#endif
#ifdef	act_output_CH3
		eeprom_write_byte_crc(EEDATA.ch3, chnValue[index], WITHOUT_CRC);
		index++;
#endif
#ifdef	act_output_CH4
		eeprom_write_byte_crc(EEDATA.ch4, chnValue[index], WITHOUT_CRC);
		index++;
#endif
#ifdef	act_output_CH5
		eeprom_write_byte_crc(EEDATA.ch5, chnValue[index], WITHOUT_CRC);
		index++;
#endif
#ifdef	act_output_CH6
		eeprom_write_byte_crc(EEDATA.ch6, chnValue[index], WITHOUT_CRC);
		index++;
#endif
#ifdef	act_output_CH7
		eeprom_write_byte_crc(EEDATA.ch7, chnValue[index], WITHOUT_CRC);
		index++;
#endif	  
		EEDATA_UPDATE_CRC;
	}
}

void act_output_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_OUTPUT && 
		rxMsg->Header.ModuleId == act_output_ID)
	{
		uint8_t index = 0;
		uint8_t value = 0;
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_PHYSICAL_SETPIN:
			
			index = 0;
			if (rxMsg->Length == 2) {
	#ifdef	act_output_CH0
				if (rxMsg->Data[0] == 0) {
					chnValue[index] = rxMsg->Data[1];
					gpio_set_statement(chnValue[index],act_output_CH0);
				}				
				index++;
	#endif
	#ifdef	act_output_CH1
				if (rxMsg->Data[0] == 1) {
					chnValue[index] = rxMsg->Data[1];
					gpio_set_statement(chnValue[index],act_output_CH1);
				}				
				index++;
	#endif
	#ifdef	act_output_CH2
				if (rxMsg->Data[0] == 2) {
					chnValue[index] = rxMsg->Data[1];
					gpio_set_statement(chnValue[index],act_output_CH2);
				}				
				index++;
	#endif
	#ifdef	act_output_CH3
				if (rxMsg->Data[0] == 3) {
					chnValue[index] = rxMsg->Data[1];
					gpio_set_statement(chnValue[index],act_output_CH3);
				}				
				index++;
	#endif
	#ifdef	act_output_CH4
				if (rxMsg->Data[0] == 4) {
					chnValue[index] = rxMsg->Data[1];
					gpio_set_statement(chnValue[index],act_output_CH4);
				}				
				index++;
	#endif
	#ifdef	act_output_CH5
				if (rxMsg->Data[0] == 5) {
					chnValue[index] = rxMsg->Data[1];
					gpio_set_statement(chnValue[index],act_output_CH5);
				}				
				index++;
	#endif
	#ifdef	act_output_CH6
				if (rxMsg->Data[0] == 6) {
					chnValue[index] = rxMsg->Data[1];
					gpio_set_statement(chnValue[index],act_output_CH6);
				}				
				index++;
	#endif
	#ifdef	act_output_CH7
				if (rxMsg->Data[0] == 7) {
					chnValue[index] = rxMsg->Data[1];
					gpio_set_statement(chnValue[index],act_output_CH7);
				}				
				index++;
	#endif	  
				Timer_SetTimeout(act_output_STORE_VALUE_TIMEOUT, act_output_STORE_VALUE_TIMEOUT_TIME_S*1000, TimerTypeOneShot, 0);
				StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
				rxMsg->Length = 2;
				while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
			} else {
				index = 0;
		#ifdef	act_output_CH0
				if (rxMsg->Data[0] == 0) {value = chnValue[index];}
				index++;
		#endif
		#ifdef	act_output_CH1
				if (rxMsg->Data[0] == 1) {value = chnValue[index];}
				index++;
		#endif
		#ifdef	act_output_CH2
				if (rxMsg->Data[0] == 2) {value = chnValue[index];}
				index++;
		#endif
		#ifdef	act_output_CH3
				if (rxMsg->Data[0] == 3) {value = chnValue[index];}
				index++;
		#endif
		#ifdef	act_output_CH4
				if (rxMsg->Data[0] == 4) {value = chnValue[index];}
				index++;
		#endif
		#ifdef	act_output_CH5
				if (rxMsg->Data[0] == 5) {value = chnValue[index];}
				index++;
		#endif
		#ifdef	act_output_CH6
				if (rxMsg->Data[0] == 6) {value = chnValue[index];}
				index++;
		#endif
		#ifdef	act_output_CH7
				if (rxMsg->Data[0] == 7) {value = chnValue[index];}
				index++;
		#endif	  	
				rxMsg->Data[1] = value;
				StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
				rxMsg->Length = 2;
				while (StdCan_Put(rxMsg) != StdCan_Ret_OK);

			}
		break;
		}
	}
}

void act_output_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_OUTPUT;
	txMsg.Header.ModuleId = act_output_ID;
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
