
#include "sns_Touch.h"

#ifdef sns_Touch_USEEEPROM
#include "sns_Touch_eeprom.h"
struct eeprom_sns_Touch EEMEM eeprom_sns_Touch = 
{
	{
		///TODO: Define initialization values on the EEPROM variables here, this will generate a *.eep file that can be used to store this values to the node, can in future be done with a EEPROM module and the make-scrips. Write the values in the exact same order as the struct is defined in the *.h file. 
		0xAB,	// x
		0x1234	// y
	},
	0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
}; 
#endif

void sns_Touch_Init(void)
{
#ifdef sns_Touch_USEEEPROM
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
	// Pcint_SetCallbackPin(sns_Touch_PCINT, EXP_C , &sns_Touch_pcint_callback);

	ADC_Init();
	Timer_SetTimeout(sns_Touch_POLL_TIMER, sns_Touch_SEND_PERIOD , TimerTypeFreeRunning, 0);

}

void sns_Touch_Process(void)
{
	
	if (Timer_Expired(sns_Touch_POLL_TIMER)) {
		//StdCan_Msg_t txMsg;
		gpio_set_in(sns_Touch_XPLUS);
		gpio_set_pullup(sns_Touch_XPLUS);	//turn on pullup for x+
		gpio_set_out(sns_Touch_YPLUS);
		gpio_set_pin(sns_Touch_YPLUS);		//turn on 1 on y+
		gpio_set_out(sns_Touch_YMINUS);
		gpio_clr_pin(sns_Touch_YMINUS);		//turn on 0 on y-
		uint8_t adxval = ADC_Get(sns_Touch_TOUCHXAD)>>2;	//read x-
		gpio_set_in(sns_Touch_YPLUS);
		gpio_clr_pullup(sns_Touch_YPLUS);	//turn off pullup for y+
		gpio_set_in(sns_Touch_YMINUS);
		gpio_clr_pullup(sns_Touch_YMINUS);	//turn off pullup for y-
		gpio_set_in(sns_Touch_XPLUS);
		gpio_clr_pullup(sns_Touch_XPLUS);	//turn off pullup for x+
		
		gpio_set_in(sns_Touch_YPLUS);
		gpio_set_pullup(sns_Touch_YPLUS);	//turn on pullup for y+
		gpio_set_out(sns_Touch_XPLUS);
		gpio_set_pin(sns_Touch_XPLUS);		//turn on 1 on x+
		gpio_set_out(sns_Touch_XMINUS);
		gpio_clr_pin(sns_Touch_XMINUS);		//turn on 0 on x-
		uint8_t adyval = ADC_Get(sns_Touch_TOUCHYAD)>>2;	//read y-
		gpio_set_in(sns_Touch_XPLUS);
		gpio_clr_pullup(sns_Touch_XPLUS);	//turn off pullup for x+
		gpio_set_in(sns_Touch_XMINUS);
		gpio_clr_pullup(sns_Touch_XMINUS);	//turn off pullup for x-
		gpio_set_in(sns_Touch_YPLUS);
		gpio_clr_pullup(sns_Touch_YPLUS);	//turn off pullup for y+
		
		if (adyval < 0xf0 && adxval < 0xf0 ) {
			printf("Y: %d X: %d\n", adyval, adxval);
		}

	}
}

void sns_Touch_HandleMessage(StdCan_Msg_t *rxMsg)
{
	/*if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_xyz && ///TODO: Change this to the actual class type
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_xyz && ///TODO: Change this to the actual module type
		rxMsg->Header.ModuleId == sns_Touch_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}*/
}

void sns_Touch_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_TST_DEBUG;
	txMsg.Header.ModuleId = sns_Touch_ID;
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
