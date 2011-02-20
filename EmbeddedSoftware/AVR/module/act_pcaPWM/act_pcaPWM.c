
#include "act_pcaPWM.h"
#include "drivers/io/PCA9634.h"

static uint16_t setValue[8];
static uint16_t isValue[8];

#ifdef act_pcaPWM_USEEEPROM
#include "act_pcaPWM_eeprom.h"
struct eeprom_act_pcaPWM EEMEM eeprom_act_pcaPWM =
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file.
		0x0000,	// ch 1
		0x0000,	// ch 2
		0x0000,	// ch 3
		0x0000,	// ch 4
		0x0000,	// ch 5
		0x0000,	// ch 6
		0x0000,	// ch 7
		0x0000	// ch 8
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
};
#endif

void act_pcaPWM_Init(void)
{
#ifdef act_pcaPWM_USEEEPROM
	if (EEDATA_OK)
	{
	  setValue[0] = eeprom_read_word(EEDATA16.ch1);
	  setValue[1] = eeprom_read_word(EEDATA16.ch2);
	  setValue[2] = eeprom_read_word(EEDATA16.ch3);
	  setValue[3] = eeprom_read_word(EEDATA16.ch4);
	  setValue[4] = eeprom_read_word(EEDATA16.ch5);
	  setValue[5] = eeprom_read_word(EEDATA16.ch6);
	  setValue[6] = eeprom_read_word(EEDATA16.ch7);
	  setValue[7] = eeprom_read_word(EEDATA16.ch8);
	} else
	{	//The CRC of the EEPROM is not correct, store default values and update CRC
	  eeprom_write_word_crc(EEDATA16.ch1, 0x0000, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.ch2, 0x0000, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.ch3, 0x0000, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.ch4, 0x0000, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.ch5, 0x0000, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.ch6, 0x0000, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.ch7, 0x0000, WITHOUT_CRC);
	  eeprom_write_word_crc(EEDATA16.ch8, 0x0000, WITHOUT_CRC);
	  EEDATA_UPDATE_CRC;
	}
#endif

	pca9634_init(act_pcaPWM_I2C_ADDRESS);
	for (uint8_t i=0; i<8; i++)
	{
		pca9634_setOpMode(i, PCA9634_OP_IND);
		/* Set "current" value to something impossible to force an update at start */
		isValue[i] = 10001;
	}
	
#ifdef act_pcaPWM_OUTPUT_ENABLE_IO
	gpio_set_out(act_pcaPWM_OUTPUT_ENABLE_IO);
	gpio_clr_pin(act_pcaPWM_OUTPUT_ENABLE_IO);
#endif
#ifdef act_pcaPWM_LED_OVERRIDE_IO
	gpio_set_out(act_pcaPWM_LED_OVERRIDE_IO);
	gpio_clr_pin(act_pcaPWM_LED_OVERRIDE_IO);
#endif
}

void act_pcaPWM_Process(void)
{

	for (uint8_t i=0; i < 8; i++)
	{
		if (setValue[i] != isValue[i])
		{
			isValue[i] = setValue[i];
			pca9634_setDuty(i, isValue[i]*act_pcaPWM_FACT);
			Timer_SetTimeout(act_pcaPWM_STORE_VALUE_TIMEOUT, act_pcaPWM_STORE_VALUE_TIMEOUT_TIME*1000, TimerTypeOneShot, 0);
		}
	}

	if (Timer_Expired(act_pcaPWM_STORE_VALUE_TIMEOUT))
	{
		if (isValue[0] != eeprom_read_word(EEDATA16.ch1))
		{
			eeprom_write_word_crc(EEDATA16.ch1, isValue[0], WITH_CRC);
		}
		if (isValue[1] != eeprom_read_word(EEDATA16.ch2))
		{
			eeprom_write_word_crc(EEDATA16.ch2, isValue[1], WITH_CRC);
		}
		if (isValue[2] != eeprom_read_word(EEDATA16.ch3))
		{
			eeprom_write_word_crc(EEDATA16.ch3, isValue[2], WITH_CRC);
		}
		if (isValue[3] != eeprom_read_word(EEDATA16.ch4))
		{
			eeprom_write_word_crc(EEDATA16.ch4, isValue[3], WITH_CRC);
		}
		if (isValue[4] != eeprom_read_word(EEDATA16.ch5))
		{
			eeprom_write_word_crc(EEDATA16.ch5, isValue[4], WITH_CRC);
		}
		if (isValue[5] != eeprom_read_word(EEDATA16.ch6))
		{
			eeprom_write_word_crc(EEDATA16.ch6, isValue[5], WITH_CRC);
		}
		if (isValue[6] != eeprom_read_word(EEDATA16.ch7))
		{
			eeprom_write_word_crc(EEDATA16.ch7, isValue[6], WITH_CRC);
		}
		if (isValue[7] != eeprom_read_word(EEDATA16.ch8))
		{
			eeprom_write_word_crc(EEDATA16.ch8, isValue[7], WITH_CRC);
		}
	}

}

void act_pcaPWM_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_PCAPWM &&
		rxMsg->Header.ModuleId == act_pcaPWM_ID)
	{
		uint8_t channel = 0;
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_PHYSICAL_PWM:
			if (rxMsg->Length == 3) {
				channel = rxMsg->Data[0];
				setValue[channel] = (rxMsg->Data[1]<<8)+(rxMsg->Data[2]);
			}
			
		break;
		}
	}
}

void act_pcaPWM_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_PCAPWM;
	txMsg.Header.ModuleId = act_pcaPWM_ID;
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
