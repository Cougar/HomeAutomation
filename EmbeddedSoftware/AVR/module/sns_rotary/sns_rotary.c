
#include "sns_rotary.h"
uint8_t rotaryEncoder_Position = 0;
uint8_t rotaryEncoder_Position_old = 0;
uint8_t rotaryEncoder_Button_Position = 0;
uint8_t rotaryEncoder_Button_Position_old = 0;
uint8_t FlagBlockTransmission = 0;


void sns_rotary_pcint_callback(uint8_t id, uint8_t status) 
{
	uint8_t rot_data = 0;
	static uint8_t rot_lastdir = 0, rot_laststate = 0;

	//Take care of button push
	if (gpio_get_state(ROTARY_BTN) != rotaryEncoder_Button_Position){ //The buttonstate has changed!
		rotaryEncoder_Button_Position = gpio_get_state(ROTARY_BTN);
	}
	//Take care of rotary encoder movement
	if(gpio_get_state(ROTARY_CH1)){
		rot_data |= 0x01;
	}
	if(gpio_get_state(ROTARY_CH2)){
		rot_data |= 0x02;
	}

	if( rot_data==0 || rot_data==3 ){ // Are both signals high or low?
		if( rot_data==0 && rot_laststate!=rot_data ){ // Are both signals low? In that case we are finished with one turn and should print out the direction it went.
			if( rot_lastdir&0x01 ){
			#if ROTARY_CHx_INVERT_DIRECTION==1
				rotaryEncoder_Position--;	// Moving clockwise
			#else
				rotaryEncoder_Position++;	// Moving counter clockwise
			#endif
			}else{
			#if ROTARY_CHx_INVERT_DIRECTION==1
				rotaryEncoder_Position++;	// Moving counter clockwise
			#else
				rotaryEncoder_Position--;	// Moving clockwise
			#endif
			}
		}
		rot_laststate = rot_data;
	} else { // No, only one of the signals are high. We can use this to find out what direction we are moving.
		rot_lastdir = rot_data;
	}
} 

void BlockTransmission_callback(uint8_t timer)
{
	FlagBlockTransmission = 0;
}

#if ROTARY_SECOND_ENABLE != 0
uint8_t rotaryEncoder_second_Position = 0;
uint8_t rotaryEncoder_second_Position_old = 0;
uint8_t rotaryEncoder_second_Button_Position = 0;
uint8_t rotaryEncoder_second_Button_Position_old = 0;
uint8_t FlagBlockTransmission_second = 0;

void sns_rotary_pcint_second_callback(uint8_t id, uint8_t status) 
{
	uint8_t rot_data = 0;
	static uint8_t rot_lastdir = 0, rot_laststate = 0;

	//Take care of button push
	if (gpio_get_state(ROTARY_BTN_SECOND) != rotaryEncoder_second_Button_Position){ //The buttonstate has changed!
		rotaryEncoder_second_Button_Position = gpio_get_state(ROTARY_BTN_SECOND);
	}
	//Take care of rotary encoder movement
	if(gpio_get_state(ROTARY_CH1_SECOND)){
		rot_data |= 0x01;
	}
	if(gpio_get_state(ROTARY_CH2_SECOND)){
		rot_data |= 0x02;
	}

	if( rot_data==0 || rot_data==3 ){ // Are both signals high or low?
		if( rot_data==0 && rot_laststate!=rot_data ){ // Are both signals low? In that case we are finished with one turn and should print out the direction it went.
			if( rot_lastdir&0x01 ){
			#if ROTARY_CHx_INVERT_DIRECTION_SECOND==1
				rotaryEncoder_second_Position--;	// Moving clockwise
			#else
				rotaryEncoder_second_Position++;	// Moving counter clockwise
			#endif
			}else{
			#if ROTARY_CHx_INVERT_DIRECTION_SECOND==1
				rotaryEncoder_second_Position++;	// Moving counter clockwise
			#else
				rotaryEncoder_second_Position--;	// Moving clockwise
			#endif
			}
		}
		rot_laststate = rot_data;
	} else { // No, only one of the signals are high. We can use this to find out what direction we are moving.
		rot_lastdir = rot_data;
	}
} 

void BlockTransmission_second_callback(uint8_t timer)
{
	FlagBlockTransmission_second = 0;
}
#endif
void sns_rotary_Init(void)
{
	/*
	 * Initialize rotaryencoders and buttons
	 */
	rotaryEncoder_Position = 0;	// Set initial value to 0
	rotaryEncoder_Position_old = 0;	// Set initial value to 0

	gpio_set_in(ROTARY_CH1);	// Set to input
	gpio_set_pin(ROTARY_CH1);	// Enable pull-up
	gpio_set_in(ROTARY_CH2);	// Set to input
	gpio_set_pin(ROTARY_CH2);	// Enable pull-up
	gpio_set_in(ROTARY_BTN);	// Set to input
	gpio_set_pin(ROTARY_BTN);	// Enable pull-up

	// Enable IO-pin interrupt
	Pcint_SetCallbackPin(sns_rotary_PCINT_CH1, ROTARY_CH1, &sns_rotary_pcint_callback);
	Pcint_SetCallbackPin(sns_rotary_PCINT_CH2, ROTARY_CH2, &sns_rotary_pcint_callback);
	Pcint_SetCallbackPin(sns_rotary_PCINT_BTN, ROTARY_BTN, &sns_rotary_pcint_callback);

#if ROTARY_SECOND_ENABLE != 0
	rotaryEncoder_second_Position = 0;	// Set initial value to 0
	rotaryEncoder_second_Position_old = 0;	// Set initial value to 0

	gpio_set_in(ROTARY_CH1_SECOND);	// Set to input
	gpio_set_pin(ROTARY_CH1_SECOND);	// Enable pull-up
	gpio_set_in(ROTARY_CH2_SECOND);	// Set to input
	gpio_set_pin(ROTARY_CH2_SECOND);	// Enable pull-up
	gpio_set_in(ROTARY_BTN_SECOND);	// Set to input
	gpio_set_pin(ROTARY_BTN_SECOND);	// Enable pull-up

	// Enable IO-pin interrupt
	Pcint_SetCallbackPin(sns_rotary_PCINT_CH1_SECOND, ROTARY_CH1_SECOND, &sns_rotary_pcint_second_callback);
	Pcint_SetCallbackPin(sns_rotary_PCINT_CH2_SECOND, ROTARY_CH2_SECOND, &sns_rotary_pcint_second_callback);
	Pcint_SetCallbackPin(sns_rotary_PCINT_BTN_SECOND, ROTARY_BTN_SECOND, &sns_rotary_pcint_second_callback);
#endif

}

void sns_rotary_Process(void)
{
	if (rotaryEncoder_Position != rotaryEncoder_Position_old && !FlagBlockTransmission)
	{
		FlagBlockTransmission=1;
		Timer_SetTimeout(sns_rotary_TIMER, sns_rotary_SEND_DELAY, TimerTypeOneShot, &BlockTransmission_callback);
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_ROTARY;
		txMsg.Header.ModuleId = sns_rotary_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_ROTARY_SWITCH;
		txMsg.Length = 4;
		txMsg.Data[0] = 0x01;
		if ((rotaryEncoder_Position > rotaryEncoder_Position_old || (rotaryEncoder_Position_old==0xff && rotaryEncoder_Position==0x00)) && !(rotaryEncoder_Position_old==0x00 && rotaryEncoder_Position==0xff)) {
			txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_ROTARY_SWITCH_DIRECTION_CLOCKWISE;	//Clockwice
			txMsg.Data[2] = rotaryEncoder_Position-rotaryEncoder_Position_old;
			if (txMsg.Data[2]>127)
			{
				txMsg.Data[2] = 256-txMsg.Data[2];
			}
		} else {
			txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_ROTARY_SWITCH_DIRECTION_COUNTERCLOCKWISE;	//Counter Clockwice
			txMsg.Data[2] = rotaryEncoder_Position_old-rotaryEncoder_Position;
			if (txMsg.Data[2]>127)
			{
				txMsg.Data[2] = 256-txMsg.Data[2];
			}
		}
		txMsg.Data[3] = rotaryEncoder_Position;

		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
		rotaryEncoder_Position_old = rotaryEncoder_Position;
	}
	if (rotaryEncoder_Button_Position != rotaryEncoder_Button_Position_old)
	{
		rotaryEncoder_Button_Position_old = rotaryEncoder_Button_Position;
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_ROTARY;
		txMsg.Header.ModuleId = sns_rotary_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_BUTTON;
		txMsg.Length = 2;
		txMsg.Data[0] = 0x01;
#if ROTARY_BTN_INVERT_OUTPUT==1
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
#if ROTARY_SECOND_ENABLE != 0
	if (rotaryEncoder_second_Position != rotaryEncoder_second_Position_old && !FlagBlockTransmission_second)
	{
		FlagBlockTransmission_second=1;
		Timer_SetTimeout(sns_rotary_TIMER_second, sns_rotary_SEND_DELAY, TimerTypeOneShot, &BlockTransmission_second_callback);
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_ROTARY;
		txMsg.Header.ModuleId = sns_rotary_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_ROTARY_SWITCH;
		txMsg.Length = 4;
		txMsg.Data[0] = 0x02;
		if ((rotaryEncoder_second_Position > rotaryEncoder_second_Position_old || (rotaryEncoder_second_Position_old==0xff && rotaryEncoder_second_Position==0x00)) && !(rotaryEncoder_second_Position_old==0x00 && rotaryEncoder_second_Position==0xff)) {
			txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_ROTARY_SWITCH_DIRECTION_CLOCKWISE;	//Clockwice
			txMsg.Data[2] = rotaryEncoder_second_Position-rotaryEncoder_second_Position_old;
			if (txMsg.Data[2]>127)
			{
				txMsg.Data[2] = 256-txMsg.Data[2];
			}
		} else {
			txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_ROTARY_SWITCH_DIRECTION_COUNTERCLOCKWISE;	//Counter Clockwice
			txMsg.Data[2] = rotaryEncoder_second_Position_old-rotaryEncoder_second_Position;
			if (txMsg.Data[2]>127)
			{
				txMsg.Data[2] = 256-txMsg.Data[2];
			}
		}
		txMsg.Data[3] = rotaryEncoder_second_Position;

		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
		rotaryEncoder_second_Position_old = rotaryEncoder_second_Position;
	}
	if (rotaryEncoder_second_Button_Position != rotaryEncoder_second_Button_Position_old)
	{
		rotaryEncoder_second_Button_Position_old = rotaryEncoder_second_Button_Position;
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_ROTARY;
		txMsg.Header.ModuleId = sns_rotary_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_BUTTON;
		txMsg.Length = 2;
		txMsg.Data[0] = 0x02;
#if ROTARY_BTN_INVERT_OUTPUT_SECOND==1
		if (rotaryEncoder_second_Button_Position_old)
			txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_BUTTON_STATUS_RELEASED;
		else
			txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_BUTTON_STATUS_PRESSED;
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
#else
		if (rotaryEncoder_second_Button_Position_old)
			txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_BUTTON_STATUS_PRESSED;
		else
			txMsg.Data[1] = CAN_MODULE_ENUM_PHYSICAL_BUTTON_STATUS_RELEASED;
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
#endif
	}

#endif

}

void sns_rotary_HandleMessage(StdCan_Msg_t *rxMsg)
{

}

void sns_rotary_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_ROTARY; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = sns_rotary_ID;
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
