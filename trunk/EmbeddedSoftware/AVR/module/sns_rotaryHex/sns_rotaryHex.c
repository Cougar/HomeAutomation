
#include "sns_rotaryHex.h"
uint8_t rotaryEncoder_Position = 0;
uint8_t rotaryEncoder_Position_old = 0;
uint8_t rotaryEncoder_Position_sent = 0;
uint8_t rotaryEncoder_Button_Position = 0;
uint8_t rotaryEncoder_Button_Position_old = 0;

uint8_t FlagBlockTransmission = 0;
uint8_t FlagNewMove = 0;

void BlockTransmission_callback(uint8_t timer)
{
	FlagBlockTransmission = 0;
}

void sns_rotaryHex_pcint_callback(uint8_t id, uint8_t status) 
{
	uint8_t rot_data = 0;
	
	//Take care of button push
	//TODO Implement some sort of debounce here
	if (gpio_get_state(sns_rotaryHex_BTN) != rotaryEncoder_Button_Position){ //The buttonstate has changed!
		rotaryEncoder_Button_Position = gpio_get_state(sns_rotaryHex_BTN);
	}
	//Take care of rotary encoder movement
	
	//get position
	rot_data =	(0x01 & gpio_get_state(sns_rotaryHex_DATA0)) +			((0x01 & gpio_get_state(sns_rotaryHex_DATA1))<< 1) +			((0x01 & gpio_get_state(sns_rotaryHex_DATA2))<< 2) +			((0x01 & gpio_get_state(sns_rotaryHex_DATA3))<< 3);
	
	if (rot_data != rotaryEncoder_Position) {
		//rotaryEncoder_Position = rot_data;
		FlagNewMove = 1;
	}
	
	//some simple debounce function
	if (FlagBlockTransmission != 1) {
		FlagBlockTransmission=1;
		Timer_SetTimeout(sns_rotaryHex_TIMER, 100, TimerTypeOneShot, &BlockTransmission_callback);
	}
} 

void sns_rotaryHex_Init(void)
{
	///TODO: Initialize hardware etc here
	
	/*
	 * Initialize rotaryencoder
	 */
	
	gpio_set_in(sns_rotaryHex_DATA0);	// Set to input
	gpio_set_in(sns_rotaryHex_DATA1);	// Set to input
	gpio_set_in(sns_rotaryHex_DATA2);	// Set to input
	gpio_set_in(sns_rotaryHex_DATA3);	// Set to input
	gpio_set_in(sns_rotaryHex_BTN);		// Set to input
	
	// Enable IO-pin interrupt
	Pcint_SetCallbackPin(sns_rotaryHex_PCINT_CH0, sns_rotaryHex_DATA0, &sns_rotaryHex_pcint_callback);
	Pcint_SetCallbackPin(sns_rotaryHex_PCINT_CH1, sns_rotaryHex_DATA1, &sns_rotaryHex_pcint_callback);
	Pcint_SetCallbackPin(sns_rotaryHex_PCINT_CH2, sns_rotaryHex_DATA2, &sns_rotaryHex_pcint_callback);
	Pcint_SetCallbackPin(sns_rotaryHex_PCINT_CH3, sns_rotaryHex_DATA3, &sns_rotaryHex_pcint_callback);
	Pcint_SetCallbackPin(sns_rotaryHex_PCINT_CH4, sns_rotaryHex_BTN, &sns_rotaryHex_pcint_callback);
	// to use PCINt lib, call this function: (the callback function look as a timer callback function)
	// Pcint_SetCallbackPin(sns_rotaryHex_PCINT, EXP_C , &sns_rotaryHex_pcint_callback);
	FlagNewMove = 0;

}

void sns_rotaryHex_Process(void)
{
	if (FlagNewMove && !FlagBlockTransmission)
	{
		FlagNewMove = 0;
		
		rotaryEncoder_Position =	(0x01 & gpio_get_state(sns_rotaryHex_DATA0)) +
			((0x01 & gpio_get_state(sns_rotaryHex_DATA1))<< 1) +
			((0x01 & gpio_get_state(sns_rotaryHex_DATA2))<< 2) +
			((0x01 & gpio_get_state(sns_rotaryHex_DATA3))<< 3);
		if (rotaryEncoder_Position != rotaryEncoder_Position_old) {
		  uint8_t rot_data = 0;
		  FlagBlockTransmission=1;
		  Timer_SetTimeout(sns_rotaryHex_TIMER, sns_rotaryHex_SEND_DELAY, TimerTypeOneShot, &BlockTransmission_callback);
		  StdCan_Msg_t txMsg;
		  StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		  StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		  txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_ROTARYHEX;
		  txMsg.Header.ModuleId = sns_rotaryHex_ID;
		  txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_ROTARY_SWITCH;
		  txMsg.Length = 4;
		  txMsg.Data[0] = 0x01;
		  int8_t temp_int = 0;
		  temp_int = rotaryEncoder_Position - rotaryEncoder_Position_old;
		  if (temp_int < 0) { //minskar
		    if (temp_int < -7) { //minska för mkt, ökar...
		      temp_int = rotaryEncoder_Position_old - rotaryEncoder_Position;
		      
		      txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_ROTARY_SWITCH_DIRECTION_CLOCKWISE;	//Clockwice
		    } else { //minskar
			txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_ROTARY_SWITCH_DIRECTION_COUNTERCLOCKWISE;	//Counter Clockwice
		    }
		  } else { //ökar
		      if (temp_int > 7) { //ökar för mkt, minskar...
			temp_int = rotaryEncoder_Position_old - rotaryEncoder_Position;
			txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_ROTARY_SWITCH_DIRECTION_COUNTERCLOCKWISE;	//Counter Clockwice
		      } else {	//ökar
			txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_ROTARY_SWITCH_DIRECTION_CLOCKWISE;	//Clockwice
		      }
		  }
 		    if (temp_int >= 8) {
		      temp_int = 1;
		    }
if (temp_int <= -8) {
		      temp_int = -1;
		    }
		    rotaryEncoder_Position_sent += temp_int;
		  if (txMsg.Data[1] == CAN_MODULE_ENUM_PHYSICAL_ROTARY_SWITCH_DIRECTION_COUNTERCLOCKWISE) {
		    temp_int *= -1;
		  }
		  
		  txMsg.Data[2] = temp_int;
		  txMsg.Data[3] = rotaryEncoder_Position;
		  while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
		  rotaryEncoder_Position_old = rotaryEncoder_Position;
		}
	}
	
	
	if (rotaryEncoder_Button_Position != rotaryEncoder_Button_Position_old)
	{
		rotaryEncoder_Button_Position_old = rotaryEncoder_Button_Position;
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_ROTARYHEX;
		txMsg.Header.ModuleId = sns_rotaryHex_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_BUTTON;
		txMsg.Length = 2;
		txMsg.Data[0] = 0x01;
#if sns_rotaryHex_BTN_INVERT_OUTPUT==1
		if (rotaryEncoder_Button_Position_old)
			txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_BUTTON_STATUS_RELEASED;
		else
			txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_BUTTON_STATUS_PRESSED;
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
#else
		if (rotaryEncoder_Button_Position_old)
			txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_BUTTON_STATUS_PRESSED;
		else
			txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_BUTTON_STATUS_RELEASED;
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
#endif
	}
}

void sns_rotaryHex_HandleMessage(StdCan_Msg_t *rxMsg)
{
	
}

void sns_rotaryHex_List(uint8_t ModuleSequenceNumber)
{
StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_ROTARYHEX; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = sns_rotaryHex_ID;
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
