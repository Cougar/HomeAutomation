
#include "act_output.h"

uint8_t chnValue[act_output_NUM_SUPPORTED];

#if act_output_USEEEPROM==1
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
#ifdef	act_output_CH8
		0x00,
#endif
#ifdef	act_output_CH9
		0x00,
#endif
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 
#endif

void act_output_Init(void)
{
#if act_output_USEEEPROM==1
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
#ifdef	act_output_CH8
		chnValue[index] = eeprom_read_byte(EEDATA.ch8);
		index++;
#endif	  
#ifdef	act_output_CH9
		chnValue[index] = eeprom_read_byte(EEDATA.ch9);
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
#ifdef	act_output_CH8
		eeprom_write_byte_crc(EEDATA.ch8, 0x00, WITHOUT_CRC);
#endif	  
#ifdef	act_output_CH9
		eeprom_write_byte_crc(EEDATA.ch9, 0x00, WITHOUT_CRC);
#endif	  
		EEDATA_UPDATE_CRC;
	}
#endif  
	///Initialize hardware
	uint8_t index = 0;

#if act_output_CH0PCA95xxIO==1 |act_output_CH1PCA95xxIO==1 | act_output_CH2PCA95xxIO==1 | act_output_CH3PCA95xxIO==1 | act_output_CH4PCA95xxIO==1 | act_output_CH5PCA95xxIO==1 | act_output_CH6PCA95xxIO==1 | act_output_CH7PCA95xxIO==1
	Pca95xx_Init(0);
#endif 
	
#ifdef	act_output_CH0
#if act_output_CH0PCA95xxIO==0
	gpio_set_statement(chnValue[index],act_output_CH0);
	gpio_set_out(act_output_CH0);
#else
	Pca95xx_set_statement(chnValue[index],act_output_CH0);
	Pca95xx_set_out(act_output_CH0);
#endif
	index++;
#endif
#ifdef	act_output_CH1
#if act_output_CH1PCA95xxIO==0
	gpio_set_statement(chnValue[index],act_output_CH1);
	gpio_set_out(act_output_CH1);
#else
	Pca95xx_set_statement(chnValue[index],act_output_CH1);
	Pca95xx_set_out(act_output_CH1);
#endif
	index++;
#endif
#ifdef	act_output_CH2
#if act_output_CH2PCA95xxIO==0
	gpio_set_statement(chnValue[index],act_output_CH2);
	gpio_set_out(act_output_CH2);
#else
	Pca95xx_set_statement(chnValue[index],act_output_CH2);
	Pca95xx_set_out(act_output_CH2);
#endif
	index++;
#endif
#ifdef	act_output_CH3
#if act_output_CH3PCA95xxIO==0
	gpio_set_statement(chnValue[index],act_output_CH3);
	gpio_set_out(act_output_CH3);
#else
	Pca95xx_set_statement(chnValue[index],act_output_CH3);
	Pca95xx_set_out(act_output_CH3);
#endif
	index++;
#endif
#ifdef	act_output_CH4
#if act_output_CH4PCA95xxIO==0
	gpio_set_statement(chnValue[index],act_output_CH4);
	gpio_set_out(act_output_CH4);
#else
	Pca95xx_set_statement(chnValue[index],act_output_CH4);
	Pca95xx_set_out(act_output_CH4);
#endif
	index++;
#endif
#ifdef	act_output_CH5
#if act_output_CH5PCA95xxIO==0
	gpio_set_statement(chnValue[index],act_output_CH5);
	gpio_set_out(act_output_CH5);
#else
	Pca95xx_set_statement(chnValue[index],act_output_CH5);
	Pca95xx_set_out(act_output_CH5);
#endif
	index++;
#endif
#ifdef	act_output_CH6
#if act_output_CH6PCA95xxIO==0
	gpio_set_statement(chnValue[index],act_output_CH6);
	gpio_set_out(act_output_CH6);
#else
	Pca95xx_set_statement(chnValue[index],act_output_CH6);
	Pca95xx_set_out(act_output_CH6);
#endif
	index++;
#endif
#ifdef	act_output_CH7
#if act_output_CH7PCA95xxIO==0
	gpio_set_statement(chnValue[index],act_output_CH7);
	gpio_set_out(act_output_CH7);
#else
	Pca95xx_set_statement(chnValue[index],act_output_CH7);
	Pca95xx_set_out(act_output_CH7);
#endif
	index++;
#endif
#ifdef	act_output_CH8
#if act_output_CH8PCA95xxIO==0
	gpio_set_statement(chnValue[index],act_output_CH8);
	gpio_set_out(act_output_CH8);
#else
	Pca95xx_set_statement(chnValue[index],act_output_CH8);
	Pca95xx_set_out(act_output_CH8);
#endif
	index++;
#endif
#ifdef	act_output_CH9
#if act_output_CH9PCA95xxIO==0
	gpio_set_statement(chnValue[index],act_output_CH9);
	gpio_set_out(act_output_CH9);
#else
	Pca95xx_set_statement(chnValue[index],act_output_CH9);
	Pca95xx_set_out(act_output_CH9);
#endif
	index++;
#endif

}

void act_output_Process(void)
{
#if act_output_USEEEPROM==1
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
#ifdef	act_output_CH8
		eeprom_write_byte_crc(EEDATA.ch8, chnValue[index], WITHOUT_CRC);
		index++;
#endif	  
#ifdef	act_output_CH9
		eeprom_write_byte_crc(EEDATA.ch9, chnValue[index], WITHOUT_CRC);
		index++;
#endif	  
		EEDATA_UPDATE_CRC;
	}
#endif

}

void act_output_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
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
	#if act_output_CH0PCA95xxIO==0
					gpio_set_statement(chnValue[index],act_output_CH0);
	#else
					Pca95xx_set_statement(chnValue[index],act_output_CH0);
	#endif
				}
				index++;
	#endif
	#ifdef	act_output_CH1
				if (rxMsg->Data[0] == 1) {
					chnValue[index] = rxMsg->Data[1];
	#if act_output_CH1PCA95xxIO==0
					gpio_set_statement(chnValue[index],act_output_CH1);
	#else
					Pca95xx_set_statement(chnValue[index],act_output_CH1);
	#endif
				}
				index++;
	#endif
	#ifdef	act_output_CH2
				if (rxMsg->Data[0] == 2) {
					chnValue[index] = rxMsg->Data[1];
	#if act_output_CH2PCA95xxIO==0
					gpio_set_statement(chnValue[index],act_output_CH2);
	#else
					Pca95xx_set_statement(chnValue[index],act_output_CH2);
	#endif
				}
				index++;
	#endif
	#ifdef	act_output_CH3
				if (rxMsg->Data[0] == 3) {
					chnValue[index] = rxMsg->Data[1];
	#if act_output_CH3PCA95xxIO==0
					gpio_set_statement(chnValue[index],act_output_CH3);
	#else
					Pca95xx_set_statement(chnValue[index],act_output_CH3);
	#endif
				}
				index++;
	#endif
	#ifdef	act_output_CH4
				if (rxMsg->Data[0] == 4) {
					chnValue[index] = rxMsg->Data[1];
	#if act_output_CH4PCA95xxIO==0
					gpio_set_statement(chnValue[index],act_output_CH4);
	#else

					Pca95xx_set_statement(chnValue[index],act_output_CH4);
	#endif
				}
				index++;
	#endif
	#ifdef	act_output_CH5
				if (rxMsg->Data[0] == 5) {
					chnValue[index] = rxMsg->Data[1];
	#if act_output_CH5PCA95xxIO==0
					gpio_set_statement(chnValue[index],act_output_CH5);
	#else
					Pca95xx_set_statement(chnValue[index],act_output_CH5);
	#endif
				}
				index++;
	#endif
	#ifdef	act_output_CH6
				if (rxMsg->Data[0] == 6) {
					chnValue[index] = rxMsg->Data[1];
	#if act_output_CH6PCA95xxIO==0
					gpio_set_statement(chnValue[index],act_output_CH6);
	#else
					Pca95xx_set_statement(chnValue[index],act_output_CH6);
	#endif
				}
				index++;
	#endif
	#ifdef	act_output_CH7
				if (rxMsg->Data[0] == 7) {
					chnValue[index] = rxMsg->Data[1];
	#if act_output_CH7PCA95xxIO==0
					gpio_set_statement(chnValue[index],act_output_CH7);
	#else
					Pca95xx_set_statement(chnValue[index],act_output_CH7);
	#endif
				}
				index++;
	#endif	  
	#ifdef	act_output_CH8
				if (rxMsg->Data[0] == 8) {
					chnValue[index] = rxMsg->Data[1];
	#if act_output_CH8PCA95xxIO==0
					gpio_set_statement(chnValue[index],act_output_CH8);
	#else
					Pca95xx_set_statement(chnValue[index],act_output_CH8);
	#endif
				}
				index++;
	#endif	  
	#ifdef	act_output_CH9
				if (rxMsg->Data[0] == 9) {
					chnValue[index] = rxMsg->Data[1];
	#if act_output_CH9PCA95xxIO==0
					gpio_set_statement(chnValue[index],act_output_CH9);
	#else
					Pca95xx_set_statement(chnValue[index],act_output_CH9);
	#endif
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
		#ifdef	act_output_CH8
				if (rxMsg->Data[0] == 8) {value = chnValue[index];}
				index++;
		#endif	  	
		#ifdef	act_output_CH9
				if (rxMsg->Data[0] == 9) {value = chnValue[index];}
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

	uint32_t HwId=BIOS_GetHwId();
	txMsg.Data[0] = HwId&0xff;
	txMsg.Data[1] = (HwId>>8)&0xff;
	txMsg.Data[2] = (HwId>>16)&0xff;
	txMsg.Data[3] = (HwId>>24)&0xff;
	
	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;
	
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}
