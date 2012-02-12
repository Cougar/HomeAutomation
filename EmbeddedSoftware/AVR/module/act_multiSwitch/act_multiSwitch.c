
#include "act_multiSwitch.h"


uint8_t activeSwitch = 0;
#ifdef act_multiSwitch_USEEEPROM
#include "act_multiSwitch_eeprom.h"
struct eeprom_act_multiSwitch EEMEM eeprom_act_multiSwitch =
{
  {
    0x00,		// x
  },
  0	// crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts
};
#endif

void act_multiSwitch_Init(void)
{
  #ifdef act_multiSwitch_USEEEPROM
  if (EEDATA_OK)
  {
    activeSwitch = eeprom_read_byte(EEDATA.activeSwitch);
  } else
  {	//The CRC of the EEPROM is not correct, store default values and update CRC
  eeprom_write_byte_crc(EEDATA.activeSwitch, 0x00, WITHOUT_CRC);
  EEDATA_UPDATE_CRC;
  }
  #endif
  ///TODO: Initialize hardware etc here
  
  // to use PCINt lib, call this function: (the callback function look as a timer callback function)
  // Pcint_SetCallbackPin(act_multiSwitch_PCINT, EXP_C , &act_multiSwitch_pcint_callback);
  #ifdef	act_multiSwitch_Feedback_0
  gpio_set_in(act_multiSwitch_Feedback_0);
  #endif
  #ifdef	act_multiSwitch_Feedback_1
  gpio_set_in(act_multiSwitch_Feedback_1);
  #endif
  #ifdef	act_multiSwitch_Feedback_2
  gpio_set_in(act_multiSwitch_Feedback_2);
  #endif
  #ifdef	act_multiSwitch_Feedback_3
  gpio_set_in(act_multiSwitch_Feedback_3);
  #endif
  #ifdef	act_multiSwitch_Feedback_4
  gpio_set_in(act_multiSwitch_Feedback_4);
  #endif
  #ifdef	act_multiSwitch_Feedback_5
  gpio_set_in(act_multiSwitch_Feedback_5);
  #endif
  #ifdef	act_multiSwitch_Feedback_6
  gpio_set_in(act_multiSwitch_Feedback_6);
  #endif
  #ifdef	act_multiSwitch_Feedback_7
  gpio_set_in(act_multiSwitch_Feedback_7);
  #endif
  #ifdef OUTPUT_ACTIVE_LOW
  gpio_set_pin(act_multiSwitch_switch);
  #endif
  #ifdef OUTPUT_ACTIVE_HIGH
  gpio_clr_pin(act_multiSwitch_switch);
  #endif
  
  gpio_set_out(act_multiSwitch_switch);
}

#define STATE_IDLE	0
#define STATE_PRESSED	1
#define STATE_WAIT	2

void act_multiSwitch_Process(void)
{
  static uint8_t state = STATE_IDLE;
  uint8_t position = 0;
  // get current position
  #ifdef	act_multiSwitch_Feedback_0
  #ifdef INPUT_ACTIVE_LOW
  if	(!gpio_get_state(act_multiSwitch_Feedback_0)) {  position = 0; }
  #endif
  #ifdef INPUT_ACTIVE_HIGH
  if	(gpio_get_state(act_multiSwitch_Feedback_0)) {  position = 0; }
  #endif
  
  #endif  
  #ifdef	act_multiSwitch_Feedback_1
  #ifdef INPUT_ACTIVE_LOW
  if	(!gpio_get_state(act_multiSwitch_Feedback_1)) { position = 1; }
  #endif
  #ifdef INPUT_ACTIVE_HIGH
  if	(gpio_get_state(act_multiSwitch_Feedback_1)) { position = 1; }
  #endif
  
  
  #endif
  #ifdef	act_multiSwitch_Feedback_2
  #ifdef INPUT_ACTIVE_LOW
  if	(!gpio_get_state(act_multiSwitch_Feedback_2)) { position = 2; }
  #endif
  #ifdef INPUT_ACTIVE_HIGH
  if	(gpio_get_state(act_multiSwitch_Feedback_2)) { position = 2; }
  #endif
  
  #endif
  #ifdef	act_multiSwitch_Feedback_3
  #ifdef INPUT_ACTIVE_LOW
  if	(!gpio_get_state(act_multiSwitch_Feedback_3)) {  position = 3; }
  #endif
  #ifdef INPUT_ACTIVE_HIGH
  if	(gpio_get_state(act_multiSwitch_Feedback_3)) {  position = 3; }
  #endif
  
  
  #endif
  #ifdef	act_multiSwitch_Feedback_4
  #ifdef INPUT_ACTIVE_LOW
  if	(!gpio_get_state(act_multiSwitch_Feedback_4)) { position = 4; }
  #endif
  #ifdef INPUT_ACTIVE_HIGH
  if	(gpio_get_state(act_multiSwitch_Feedback_4)) { position = 4; }
  #endif
  
  
  #endif
  #ifdef	act_multiSwitch_Feedback_5
  #ifdef INPUT_ACTIVE_LOW
  if	(!gpio_get_state(act_multiSwitch_Feedback_5)) { position = 5; }
  #endif
  #ifdef INPUT_ACTIVE_HIGH
  if	(gpio_get_state(act_multiSwitch_Feedback_5)) { position = 5; }
  #endif
  
  
  #endif
  #ifdef	act_multiSwitch_Feedback_6
  #ifdef INPUT_ACTIVE_LOW
  if	(!gpio_get_state(act_multiSwitch_Feedback_6)) {  position = 6; }
  #endif
  #ifdef INPUT_ACTIVE_HIGH
  if	(gpio_get_state(act_multiSwitch_Feedback_6)) {  position = 6; }
  #endif
  
  #endif
  #ifdef	act_multiSwitch_Feedback_7
  #ifdef INPUT_ACTIVE_LOW
  if	(!gpio_get_state(act_multiSwitch_Feedback_7)) { position = 7; }
  #endif
  #ifdef INPUT_ACTIVE_HIGH
  if	(gpio_get_state(act_multiSwitch_Feedback_7)) { position = 7; }
  #endif
  #endif
  
  if (state == STATE_IDLE) {
    if (position != activeSwitch) {
      //printf("1: %d = %d\n",position, activeSwitch);
      #ifdef OUTPUT_ACTIVE_LOW
      gpio_clr_pin(act_multiSwitch_switch);
      #endif
      #ifdef OUTPUT_ACTIVE_HIGH
      gpio_set_pin(act_multiSwitch_switch);
      #endif
      state = STATE_PRESSED;
      Timer_SetTimeout(act_multiSwitch_PRESS_TIMER, act_multiSwitch_PRESS_TIMEOUT_TIME_MS, TimerTypeOneShot, 0);
    }
  }
  
  if (state == STATE_PRESSED) {
    if (Timer_Expired(act_multiSwitch_PRESS_TIMER))
    {
      //printf("2: \n");
      #ifdef OUTPUT_ACTIVE_LOW
      gpio_set_pin(act_multiSwitch_switch);
      #endif
      #ifdef OUTPUT_ACTIVE_HIGH
      gpio_clr_pin(act_multiSwitch_switch);
      #endif  
      state = STATE_WAIT;
      Timer_SetTimeout(act_multiSwitch_PRESS_TIMER, act_multiSwitch_WAIT_TIMEOUT_TIME_MS, TimerTypeOneShot, 0);
    }
  }
  if (state == STATE_WAIT) {
    if (Timer_Expired(act_multiSwitch_PRESS_TIMER))
    {
       //printf("3: \n");
       state = STATE_IDLE;
    }
  }
  
  if (Timer_Expired(act_multiSwitch_STORE_VALUE_TIMER))
  {
    eeprom_write_byte_crc(EEDATA.activeSwitch, activeSwitch, WITH_CRC);
  }
}

void act_multiSwitch_HandleMessage(StdCan_Msg_t *rxMsg)
{
  if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
    StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
    rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_MULTISWITCH && 
    rxMsg->Header.ModuleId == act_multiSwitch_ID)
  {
    switch (rxMsg->Header.Command)
    {
      case CAN_MODULE_CMD_PHYSICAL_SETPIN:
	if (rxMsg->Length == 2) {
	  if (rxMsg->Data[1] == CAN_MODULE_ENUM_PHYSICAL_SETPIN_STATUS_HIGH) {
	    activeSwitch= rxMsg->Data[0];
	    Timer_SetTimeout(act_multiSwitch_STORE_VALUE_TIMER, act_multiSwitch_STORE_VALUE_TIMEOUT_TIME_S*1000, TimerTypeOneShot, 0);
	    StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
	    rxMsg->Length = 2;
	    while (StdCan_Put(rxMsg) != StdCan_Ret_OK);
	  } else {
	    rxMsg->Data[0] = activeSwitch+10;
	    rxMsg->Data[1] = CAN_MODULE_ENUM_PHYSICAL_SETPIN_STATUS_HIGH;
	    StdCan_Set_direction(rxMsg->Header, DIRECTIONFLAG_FROM_OWNER);
	    rxMsg->Length = 2;
	    while (StdCan_Put(rxMsg) != StdCan_Ret_OK); 
	  }
	  
	}
	break;
    }
  }
}
  
  void act_multiSwitch_List(uint8_t ModuleSequenceNumber)
  {
    StdCan_Msg_t txMsg;
    
    StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); 
    StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
    txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_MULTISWITCH; 
    txMsg.Header.ModuleId = act_multiSwitch_ID;
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
  
