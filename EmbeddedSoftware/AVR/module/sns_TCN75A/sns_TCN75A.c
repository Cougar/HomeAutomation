
#include "sns_TCN75A.h"

unsigned char messageBuf[5];

#ifdef sns_TCN75A_USEEEPROM
#include "sns_TCN75A_eeprom.h"
struct eeprom_sns_TCN75A EEMEM eeprom_sns_TCN75A = 
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file. 
		0xAB,	// x
		0x1234	// y
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 
#endif

void sns_TCN75A_Init(void)
{
#ifdef sns_TCN75A_USEEEPROM
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
	// Pcint_SetCallbackPin(sns_TCN75A_PCINT, EXP_C , &sns_TCN75A_pcint_callback);

	TWI_Master_Initialise();
	Timer_SetTimeout(sns_TCN75A_POLL_TIMER, sns_TCN75A_POLL_PERIOD*1000, TimerTypeFreeRunning, 0);

	messageBuf[0] = (sns_TCN75A_I2C_ADDRESS<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	messageBuf[1] = TCN75A_REG_CONFIG;
	messageBuf[2] = (TCN75A_CNF_RESOLUTION_12<<TCN75A_CNF_RESOLUTION_BIT);
	/* write config register, 12bit resolution */
	TWI_Start_Read_Write( messageBuf, 3 );
	while ( TWI_Transceiver_Busy() );

	messageBuf[0] = (sns_TCN75A_I2C_ADDRESS<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	messageBuf[1] = TCN75A_REG_TEMPERATURE;
	/* choose temperature register */
	TWI_Start_Read_Write( messageBuf, 2 );
	
	while ( TWI_Transceiver_Busy() );
}

void sns_TCN75A_Process(void)
{
	if (Timer_Expired(sns_TCN75A_POLL_TIMER)) {
		/* Read temperature from I2C-device */
		messageBuf[0] = (sns_TCN75A_I2C_ADDRESS<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);
		/* initiate a read of temperature */
		TWI_Start_Read_Write( messageBuf, 3 );
		
		/* wait for data and then fetch data to messageBuf */
		if (TWI_Read_Data_From_Buffer(messageBuf, 3) == TRUE)
		{
			StdCan_Msg_t txMsg;
			StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
			StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
			txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_TCN75A;
			txMsg.Header.ModuleId = sns_TCN75A_ID;
			txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_TEMPERATURE_CELSIUS;
			txMsg.Length = 3;
			/* set up sensor id byte */
			txMsg.Data[0] = 0;
			/* set up sign bit */
			txMsg.Data[1] = messageBuf[1]&0x80;
			/* set up msb byte */
			txMsg.Data[1] |= ((messageBuf[1]&0x7f) >>2 );
			/* set up lsb byte */
			txMsg.Data[2] = ((messageBuf[1]&0x03) <<6 );
			txMsg.Data[2] |= (messageBuf[2] >>2);

			StdCan_Put(&txMsg);
		}
	}
}

void sns_TCN75A_HandleMessage(StdCan_Msg_t *rxMsg)
{
	/*if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_xyz && ///TODO: Change this to the actual class type
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_xyz && ///TODO: Change this to the actual module type
		rxMsg->Header.ModuleId == sns_TCN75A_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}*/
}

void sns_TCN75A_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_TCN75A;
	txMsg.Header.ModuleId = sns_TCN75A_ID;
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
