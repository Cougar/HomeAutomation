
#include "sns_inputAnalog.h"

struct {
	uint8_t				Status;			//Used for digital input, high or low
	uint16_t			PeriodCnt;		//Counter for periodicity
	uint16_t			LastSentAdVal;	//Remember the last sent AD value
} sns_inputAnalog_Sensor[sns_inputAnalog_NUM_SUPPORTED];


#define HIGH		1
#define LOW			2
#define NOCHANGE 	0

#ifdef sns_inputAnalog_USEEEPROM
#include "sns_inputAnalog_eeprom.h"

struct eeprom_sns_inputAnalog EEMEM eeprom_sns_inputAnalog = 
{
	{
		{
		/* Define initialization values on the EEPROM variables here, this will generate a *.eep file 
		that can be used to store this values to the node, can in future be done with a EEPROM module 
		and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file.  */
		1*sns_inputAnalog0Factor/(2^sns_inputAnalog0Scale),	//Config, low level threshold voltage, (1 volt)
		2*sns_inputAnalog0Factor/(2^sns_inputAnalog0Scale),	//Config, high level threshold voltage, (2 volts)
		4800,	//Config, periodicity
		CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_SETTING_PERIODICMEASURE,	//Config, if sensor is of type periodic or digital input
		CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_PULLUP_DISABLE,			//Config, if the pullup should be enabled
		CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_REFERENCE_DISABLE,			//Config, if the reference to GND should be enabled
		},
#if sns_inputAnalog_NUM_SUPPORTED>=2
		{
		1*sns_inputAnalog1Factor/(2^sns_inputAnalog1Scale),	//Config, low level threshold voltage, (1 volt)
		2*sns_inputAnalog1Factor/(2^sns_inputAnalog1Scale),	//Config, high level threshold voltage, (2 volts)
		4900,	
		CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_SETTING_PERIODICMEASURE,	//Config, if sensor is of type periodic or digital input
		CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_PULLUP_DISABLE,			//Config, if the pullup should be enabled
		CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_REFERENCE_DISABLE,			//Config, if the reference to GND should be enabled
		},
#endif
#if sns_inputAnalog_NUM_SUPPORTED>=3
		{
		1*sns_inputAnalog2Factor/(2^sns_inputAnalog2Scale),	//Config, low level threshold voltage
		2*sns_inputAnalog2Factor/(2^sns_inputAnalog2Scale),	//Config, high level threshold voltage, (2 volts)
		5100,
		CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_SETTING_PERIODICMEASURE,	//Config, if sensor is of type periodic or digital input
		CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_PULLUP_DISABLE,			//Config, if the pullup should be enabled
		CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_REFERENCE_DISABLE,			//Config, if the reference to GND should be enabled
		},
#endif
#if sns_inputAnalog_NUM_SUPPORTED>=4
		{
		1*sns_inputAnalog3Factor/(2^sns_inputAnalog3Scale),	//Config, low level threshold voltage
		2*sns_inputAnalog3Factor/(2^sns_inputAnalog3Scale),	//Config, high level threshold voltage, (2 volts)
		5200,
		CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_SETTING_PERIODICMEASURE,	//Config, if sensor is of type periodic or digital input
		CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_PULLUP_DISABLE,			//Config, if the pullup should be enabled
		CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_REFERENCE_DISABLE,			//Config, if the reference to GND should be enabled
		}
#endif
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 
#endif

/* Will set up pullup enable outputs according config */
void setPullups(void)
{
	if (sns_inputAnalog_Config[0].PullupEnable==CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_PULLUP_ENABLE)
	{
#if sns_inputAnalog0PullupPCA95xx==0
		gpio_set_pin(sns_inputAnalog0Pullup);
		gpio_set_out(sns_inputAnalog0Pullup);
#else
		Pca95xx_set_pin(sns_inputAnalog0Pullup);
		Pca95xx_set_out(sns_inputAnalog0Pullup);
#endif
	}
	else
	{
#if sns_inputAnalog0PullupPCA95xx==0
		gpio_clr_pin(sns_inputAnalog0Pullup);
		gpio_set_out(sns_inputAnalog0Pullup);
#else
		Pca95xx_clr_pin(sns_inputAnalog0Pullup);
		Pca95xx_set_out(sns_inputAnalog0Pullup);
#endif
	}
	
#if sns_inputAnalog_NUM_SUPPORTED>=2
	if (sns_inputAnalog_Config[1].PullupEnable==CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_PULLUP_ENABLE)
	{
#if sns_inputAnalog1PullupPCA95xx==0
		gpio_set_pin(sns_inputAnalog1Pullup);
		gpio_set_out(sns_inputAnalog1Pullup);
#else
		Pca95xx_set_pin(sns_inputAnalog1Pullup);
		Pca95xx_set_out(sns_inputAnalog1Pullup);
#endif
	}
	else
	{
#if sns_inputAnalog1PullupPCA95xx==0
		gpio_clr_pin(sns_inputAnalog1Pullup);
		gpio_set_out(sns_inputAnalog1Pullup);
#else
		Pca95xx_clr_pin(sns_inputAnalog1Pullup);
		Pca95xx_set_out(sns_inputAnalog1Pullup);
#endif
	}
#endif

#if sns_inputAnalog_NUM_SUPPORTED>=3
	if (sns_inputAnalog_Config[2].PullupEnable==CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_PULLUP_ENABLE)
	{
#if sns_inputAnalog2PullupPCA95xx==0
		gpio_set_pin(sns_inputAnalog2Pullup);
		gpio_set_out(sns_inputAnalog2Pullup);
#else
		Pca95xx_set_pin(sns_inputAnalog2Pullup);
		Pca95xx_set_out(sns_inputAnalog2Pullup);
#endif
	}
	else
	{
#if sns_inputAnalog2PullupPCA95xx==0
		gpio_clr_pin(sns_inputAnalog2Pullup);
		gpio_set_out(sns_inputAnalog2Pullup);
#else
		Pca95xx_clr_pin(sns_inputAnalog2Pullup);
		Pca95xx_set_out(sns_inputAnalog2Pullup);
#endif
	}
#endif

#if sns_inputAnalog_NUM_SUPPORTED>=4
	if (sns_inputAnalog_Config[3].PullupEnable==CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_PULLUP_ENABLE)
	{
#if sns_inputAnalog3PullupPCA95xx==0
		gpio_set_pin(sns_inputAnalog3Pullup);
		gpio_set_out(sns_inputAnalog3Pullup);
#else
		Pca95xx_set_pin(sns_inputAnalog3Pullup);
		Pca95xx_set_out(sns_inputAnalog3Pullup);
#endif
	}
	else
	{
#if sns_inputAnalog3PullupPCA95xx==0
		gpio_clr_pin(sns_inputAnalog3Pullup);
		gpio_set_out(sns_inputAnalog3Pullup);
#else
		Pca95xx_clr_pin(sns_inputAnalog3Pullup);
		Pca95xx_set_out(sns_inputAnalog3Pullup);
#endif
	}
#endif
}

/* Will set up reference enable outputs according config */
void setReferences(void)
{
	if (sns_inputAnalog_Config[0].RefEnable==CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_REFERENCE_ENABLE)
	{
#if sns_inputAnalog0RefPCA95xx==0
		gpio_set_pin(sns_inputAnalog0Ref);
		gpio_set_out(sns_inputAnalog0Ref);
#else
		Pca95xx_set_pin(sns_inputAnalog0Ref);
		Pca95xx_set_out(sns_inputAnalog0Ref);
#endif
	}
	else
	{
#if sns_inputAnalog0RefPCA95xx==0
		gpio_clr_pin(sns_inputAnalog0Ref);
		gpio_set_out(sns_inputAnalog0Ref);
#else
		Pca95xx_clr_pin(sns_inputAnalog0Ref);
		Pca95xx_set_out(sns_inputAnalog0Ref);
#endif
	}
	
#if sns_inputAnalog_NUM_SUPPORTED>=2
	if (sns_inputAnalog_Config[1].RefEnable==CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_REFERENCE_ENABLE)
	{
#if sns_inputAnalog1RefPCA95xx==0
		gpio_set_pin(sns_inputAnalog1Ref);
		gpio_set_out(sns_inputAnalog1Ref);
#else
		Pca95xx_set_pin(sns_inputAnalog1Ref);
		Pca95xx_set_out(sns_inputAnalog1Ref);
#endif
	}
	else
	{
#if sns_inputAnalog1RefPCA95xx==0
		gpio_clr_pin(sns_inputAnalog1Ref);
		gpio_set_out(sns_inputAnalog1Ref);
#else
		Pca95xx_clr_pin(sns_inputAnalog1Ref);
		Pca95xx_set_out(sns_inputAnalog1Ref);
#endif
	}
#endif

#if sns_inputAnalog_NUM_SUPPORTED>=3
	if (sns_inputAnalog_Config[2].RefEnable==CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_REFERENCE_ENABLE)
	{
#if sns_inputAnalog2RefPCA95xx==0
		gpio_set_pin(sns_inputAnalog2Ref);
		gpio_set_out(sns_inputAnalog2Ref);
#else
		Pca95xx_set_pin(sns_inputAnalog2Ref);
		Pca95xx_set_out(sns_inputAnalog2Ref);
#endif
	}
	else
	{
#if sns_inputAnalog2RefPCA95xx==0
		gpio_clr_pin(sns_inputAnalog2Ref);
		gpio_set_out(sns_inputAnalog2Ref);
#else
		Pca95xx_clr_pin(sns_inputAnalog2Ref);
		Pca95xx_set_out(sns_inputAnalog2Ref);
#endif
	}
#endif

#if sns_inputAnalog_NUM_SUPPORTED>=4
	if (sns_inputAnalog_Config[3].RefEnable==CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_REFERENCE_ENABLE)
	{
#if sns_inputAnalog3RefPCA95xx==0
		gpio_set_pin(sns_inputAnalog3Ref);
		gpio_set_out(sns_inputAnalog3Ref);
#else
		Pca95xx_set_pin(sns_inputAnalog3Ref);
		Pca95xx_set_out(sns_inputAnalog3Ref);
#endif
	}
	else
	{
#if sns_inputAnalog3RefPCA95xx==0
		gpio_clr_pin(sns_inputAnalog3Ref);
		gpio_set_out(sns_inputAnalog3Ref);
#else
		Pca95xx_clr_pin(sns_inputAnalog3Ref);
		Pca95xx_set_out(sns_inputAnalog3Ref);
#endif
	}
#endif	
}

void sns_inputAnalog_Init(void)
{
#ifdef sns_inputAnalog_USEEEPROM
	if (EEDATA_OK)
	{
		/* Use stored data to set initial values for the module */
		for (uint8_t i=0; i<sns_inputAnalog_NUM_SUPPORTED; i++)
		{
			//eeprom_write_block( &sns_inputAnalog_Config[i], &eeprom_sns_inputAnalog, sizeof(sns_inputAnalog_Config)*i );
			eeprom_read_block( &sns_inputAnalog_Config[i], &eeprom_sns_inputAnalog+sizeof(sns_inputAnalog_Config)*i, sizeof(sns_inputAnalog_Config) );
		}
	} 
	else
	{
		/* The CRC of the EEPROM is not correct, store default values and update CRC */
		for (uint8_t i=0; i<sns_inputAnalog_NUM_SUPPORTED; i++)
		{
			sns_inputAnalog_Config[i].LowTh=50;			//Config, low level threshold voltage
			sns_inputAnalog_Config[i].HighTh=100;		//Config, high level threshold voltage
			sns_inputAnalog_Config[i].Periodicity=5000;	//Config, periodicity
			sns_inputAnalog_Config[i].Type=CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_SETTING_PERIODICMEASURE;	//Config, if sensor is of type periodic or digital input
			sns_inputAnalog_Config[i].PullupEnable=CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_PULLUP_DISABLE;		//Config, if the pullup should be enabled
			sns_inputAnalog_Config[i].RefEnable=CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_REFERENCE_DISABLE;		//Config, if the reference to GND should be enabled
			
#if ((__AVR_LIBC_MAJOR__ == 1  && __AVR_LIBC_MINOR__ == 6 && __AVR_LIBC_REVISION__ >= 7)||(__AVR_LIBC_MAJOR__ == 1  && __AVR_LIBC_MINOR__ > 6)||__AVR_LIBC_MAJOR__ > 1)
			eeprom_update_block( &sns_inputAnalog_Config[i], &eeprom_sns_inputAnalog+sizeof(sns_inputAnalog_Config)*i, sizeof(sns_inputAnalog_Config) );
#else
			eeprom_write_block( &sns_inputAnalog_Config[i], &eeprom_sns_inputAnalog+sizeof(sns_inputAnalog_Config)*i, sizeof(sns_inputAnalog_Config) );
			#warning Using old version of AVRlibc, does not support eeprom_update-functions
#endif
		}
		EEDATA_UPDATE_CRC;
	}
#endif  

#if sns_inputAnalog_ENABLE_PCA95xx==1
	Pca95xx_Init(0);
#endif 
	/* Initiate ADC */
	ADC_Init();
	/* Start timer for reading inputs */
	Timer_SetTimeout(sns_inputAnalog_TIMER, sns_inputAnalog_POLL_PERIOD_MS , TimerTypeFreeRunning, 0);
	/* Set pullups according config */
	setPullups();
	/* Set gnd references according config */
	setReferences();
}

void sns_inputAnalog_Process(void)
{
	/* When the timer has overflowed the AD channels shall be read */
	if (Timer_Expired(sns_inputAnalog_TIMER)) 
	{
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_INPUTANALOG;
		txMsg.Header.ModuleId = sns_inputAnalog_ID;

		uint16_t AdValue=0;
		/* For each channel */
		for (uint8_t i=0; i<sns_inputAnalog_NUM_SUPPORTED; i++)
		{
			/* Do reading of AD channel */
			switch (i)
			{
				case 0:
					AdValue = ADC_Get(sns_inputAnalog0AD);
					break;
				case 1:
					AdValue = ADC_Get(sns_inputAnalog1AD);
					break;
				case 2:
					AdValue = ADC_Get(sns_inputAnalog2AD);
					break;
				case 3:
					AdValue = ADC_Get(sns_inputAnalog3AD);
					break;
			}
			
			/* If this channel is configured as periodic transmission of voltage */
			if (sns_inputAnalog_Config[i].Type == CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_SETTING_PERIODICMEASURE)
			{
				/* Count periodicity */
				sns_inputAnalog_Sensor[i].PeriodCnt += sns_inputAnalog_POLL_PERIOD_MS;
				/* If periodicity overflowed or AD value changed more than threshold since last sent value */
				if (sns_inputAnalog_Sensor[i].PeriodCnt >= sns_inputAnalog_Config[i].Periodicity ||
					MAX(AdValue,sns_inputAnalog_Sensor[i].LastSentAdVal)-MIN(AdValue,sns_inputAnalog_Sensor[i].LastSentAdVal) > sns_inputAnalog_Config[i].LowTh)
				//if (sns_inputAnalog_Sensor[i].PeriodCnt >= sns_inputAnalog_Config[i].Periodicity ||
				//	abs(AdValue-sns_inputAnalog_Sensor[i].LastSentAdVal) > sns_inputAnalog_Config[i].LowTh)
				{
					/* Reset periodicity counter */
					sns_inputAnalog_Sensor[i].PeriodCnt = 0;
					/* Store value as last sent */
					sns_inputAnalog_Sensor[i].LastSentAdVal = AdValue;
					
					/* send sensor value on CAN with command CAN_MODULE_CMD_PHYSICAL_VOLTAGE */
					txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_VOLTAGE;
					txMsg.Length = 3;
					/* The channel should be transmitted in byte 0 */
					txMsg.Data[0] = i;
					
					uint8_t analogScale = 10;
					/* Select parameters for AD conversion */
					switch (i)
					{
						case 0:
							analogScale = sns_inputAnalog0Scale;
							AdValue = AdValue * sns_inputAnalog0Factor;
							break;
						case 1:
							analogScale = sns_inputAnalog1Scale;
							AdValue = AdValue * sns_inputAnalog1Factor;
							break;
						case 2:
							analogScale = sns_inputAnalog2Scale;
							AdValue = AdValue * sns_inputAnalog2Factor;
							break;
						case 3:
							analogScale = sns_inputAnalog3Scale;
							AdValue = AdValue * sns_inputAnalog3Factor;
							break;
					}
					txMsg.Data[1] = (AdValue>>(analogScale-6+8))&0xff;
					txMsg.Data[2] = (AdValue>>(analogScale-6))&0xff;
					
					/* Send value on CAN */
					while (StdCan_Put(&txMsg) != StdCan_Ret_OK) {}
				}
			}
			/* If this channel is configured as digital input */
			else if (sns_inputAnalog_Config[i].Type == CAN_MODULE_ENUM_INPUTANALOG_ANALOGCONFIG_SETTING_DIGITALINPUT)
			{
				txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_PINSTATUS;
				txMsg.Length = 2;
				/* The channel should be transmitted in byte 0 */
				txMsg.Data[0] = i;
				/* If status was low but voltage is above high theshold */
				if (sns_inputAnalog_Sensor[i].Status == LOW && AdValue > sns_inputAnalog_Config[i].HighTh)
				{
					/* Consider status to be high */
					sns_inputAnalog_Sensor[i].Status = HIGH;
					txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_PINSTATUS_STATUS_HIGH;
					
					/* send status on CAN with command CAN_MODULE_CMD_PHYSICAL_PINSTATUS */
					while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
				}
				/* If status was high but voltage is below low theshold */
				else if (sns_inputAnalog_Sensor[i].Status == HIGH && AdValue < sns_inputAnalog_Config[i].LowTh)
				{
					/* Consider status to be low */
					sns_inputAnalog_Sensor[i].Status = LOW;
					txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_PINSTATUS_STATUS_LOW;
					
					/* send status on CAN with command CAN_MODULE_CMD_PHYSICAL_PINSTATUS */
					while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
				}
			}
		}
	}
}

void sns_inputAnalog_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS && 
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_INPUTANALOG && 
		rxMsg->Header.ModuleId == sns_inputAnalog_ID)
	{
		uint8_t index=0;
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_INPUTANALOG_ANALOGCONFIG:
			/* Find sensor id from incoming data */
			index=rxMsg->Data[0]&0x0f;
			/* Check if sensor id is in range */
			if (index < sns_inputAnalog_NUM_SUPPORTED)
			{
				/* Save all config values */
				sns_inputAnalog_Config[index].Type=(rxMsg->Data[0]>>4)&0x03;
				sns_inputAnalog_Config[index].PullupEnable=(rxMsg->Data[0]>>6)&0x01;
				sns_inputAnalog_Config[index].RefEnable=(rxMsg->Data[0]>>7)&0x01;
				sns_inputAnalog_Config[index].LowTh=(rxMsg->Data[2])|(rxMsg->Data[1]<<8);
				sns_inputAnalog_Config[index].HighTh=(rxMsg->Data[4])|(rxMsg->Data[3]<<8);
				sns_inputAnalog_Config[index].Periodicity=(rxMsg->Data[6])|(rxMsg->Data[5]<<8);
				
				/* Convert voltage to AD value (0-1023) acording static config parameters */
				switch (index)
				{
					case 0:
						sns_inputAnalog_Config[index].LowTh = sns_inputAnalog_Config[index].LowTh<<(sns_inputAnalog0Scale-8);
						sns_inputAnalog_Config[index].LowTh = sns_inputAnalog_Config[index].LowTh / sns_inputAnalog0Factor;
						sns_inputAnalog_Config[index].HighTh = sns_inputAnalog_Config[index].HighTh<<(sns_inputAnalog0Scale-8);
						sns_inputAnalog_Config[index].HighTh = sns_inputAnalog_Config[index].HighTh / sns_inputAnalog0Factor;
						break;
					case 1:
						sns_inputAnalog_Config[index].LowTh = sns_inputAnalog_Config[index].LowTh<<(sns_inputAnalog1Scale-8);
						sns_inputAnalog_Config[index].LowTh = sns_inputAnalog_Config[index].LowTh / sns_inputAnalog1Factor;
						sns_inputAnalog_Config[index].HighTh = sns_inputAnalog_Config[index].HighTh<<(sns_inputAnalog1Scale-8);
						sns_inputAnalog_Config[index].HighTh = sns_inputAnalog_Config[index].HighTh / sns_inputAnalog1Factor;
						break;
					case 2:
						sns_inputAnalog_Config[index].LowTh = sns_inputAnalog_Config[index].LowTh<<(sns_inputAnalog2Scale-8);
						sns_inputAnalog_Config[index].LowTh = sns_inputAnalog_Config[index].LowTh / sns_inputAnalog2Factor;
						sns_inputAnalog_Config[index].HighTh = sns_inputAnalog_Config[index].HighTh<<(sns_inputAnalog2Scale-8);
						sns_inputAnalog_Config[index].HighTh = sns_inputAnalog_Config[index].HighTh / sns_inputAnalog2Factor;
						break;
					case 3:
						sns_inputAnalog_Config[index].LowTh = sns_inputAnalog_Config[index].LowTh<<(sns_inputAnalog3Scale-8);
						sns_inputAnalog_Config[index].LowTh = sns_inputAnalog_Config[index].LowTh / sns_inputAnalog3Factor;
						sns_inputAnalog_Config[index].HighTh = sns_inputAnalog_Config[index].HighTh<<(sns_inputAnalog3Scale-8);
						sns_inputAnalog_Config[index].HighTh = sns_inputAnalog_Config[index].HighTh / sns_inputAnalog3Factor;
						break;
				}
				
				/* Store config to eeprom */
#if ((__AVR_LIBC_MAJOR__ == 1  && __AVR_LIBC_MINOR__ == 6 && __AVR_LIBC_REVISION__ >= 7)||(__AVR_LIBC_MAJOR__ == 1  && __AVR_LIBC_MINOR__ > 6)||__AVR_LIBC_MAJOR__ > 1)
				eeprom_update_block( &sns_inputAnalog_Config[index], &eeprom_sns_inputAnalog+sizeof(sns_inputAnalog_Config)*index, sizeof(sns_inputAnalog_Config) );
#else
				eeprom_write_block( &sns_inputAnalog_Config[index], &eeprom_sns_inputAnalog+sizeof(sns_inputAnalog_Config)*index, sizeof(sns_inputAnalog_Config) );
				#warning Using old version of AVRlibc, does not support eeprom_update-functions
#endif
			}
			break;
		}
		/* Set pullups according config */
		setPullups();
		/* Set gnd references according config */
		setReferences();
	}
}

void sns_inputAnalog_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_INPUTANALOG; 
	txMsg.Header.ModuleId = sns_inputAnalog_ID;
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
