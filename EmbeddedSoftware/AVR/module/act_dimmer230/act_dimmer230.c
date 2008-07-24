
#include "act_dimmer230.h"
uint8_t state = ACT_DIMMMER230_STATE_IDLE;

ISR (TIMER1_COMPA_vect) 
{

}
ISR (act_dimmer230_ZC_PCINT_vect) 
{
	if (state==ACT_DIMMMER230_STATE_IDLE && (act_dimmer230_ZC_PIN&(1<<act_dimmer230_ZC_BIT))) 
	{
		
		//enable timer and timer interrupt
		TCNT1 = 0;				//reset timer1 count register
		// here timer oc1a register should be set to the correct time (zero-cross time + dimmer value)
		TIFR1=(1<<OCF1A);		//clear interrupt flag before enabling interrupt
		TIMSK1|=(1<<OCIE1A);

		state = ACT_DIMMMER230_STATE_TIMER_ON;
	}
}

void act_dimmer230_Init(void)
{
	///TODO: Initialize hardware etc here

	// set dimmer channel1 to output 0
	act_dimmer230_CHAN1_PORT &= ~_BV(act_dimmer230_CHAN1_BIT);
	act_dimmer230_CHAN1_DDR |= _BV(act_dimmer230_CHAN1_BIT);
	// set zero cross detection to input, no pullup
	act_dimmer230_ZC_PORT &= ~_BV(act_dimmer230_ZC_BIT);
	act_dimmer230_ZC_DDR &= ~_BV(act_dimmer230_ZC_BIT);
	
	
	TIMSK1=0;							//disable timer interrupt
	TCCR1A=0;
	TCCR1B=(1<<CS11);					//enable timer, set to prescaler 8, must be changed if clk is changed
	
	//wait for high (ac leaving zero cross)
	while (!(act_dimmer230_ZC_PIN&(1<<act_dimmer230_ZC_BIT))) { }
	//wait for low (ac approaching zero cross)
	while (act_dimmer230_ZC_PIN&(1<<act_dimmer230_ZC_BIT)) { }
	uint16_t timeAtFall = TCNT1;	//Timer_GetTicks();
	//wait for high (ac leaving zero cross)
	while (!(act_dimmer230_ZC_PIN&(1<<act_dimmer230_ZC_BIT))) { }
	uint16_t timeAtRise = TCNT1;	//Timer_GetTicks();

	//setup interrupt on zerocross, pcint
	PCMSK0=(1<<act_dimmer230_ZC_PCINT_BIT);
	PCIFR=(1<<act_dimmer230_ZC_PCIF);	//clear any pending interrupt before enabling interrupts
	PCICR=(1<<act_dimmer230_ZC_PCIE);	//enable interrupt for PCINT
}

void act_dimmer230_Process(void)
{
	///TODO: Stuff that needs doing is done here
}

void act_dimmer230_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_CLASS_MODULE_DEF && ///TODO: Change this to the actual class type
		StdCan_Ret_direction(rxMsg->Header) == DIR_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_TYPE_MODULE_def_default && ///TODO: Change this to the actual module type
		rxMsg->Header.ModuleId == act_dimmer230_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case 0:
		///TODO: Do something dummy
		break;
		}
	}
}

void act_dimmer230_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_CLASS_MODULE_DEF); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIR_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_TYPE_MODULE_def_default; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = act_dimmer230_ID;
	txMsg.Header.Command = CAN_CMD_MODULE_NMT_LIST;
	txMsg.Length = 6;

	txMsg.Data[0] = NODE_HW_ID_BYTE0;
	txMsg.Data[1] = NODE_HW_ID_BYTE1;
	txMsg.Data[2] = NODE_HW_ID_BYTE2;
	txMsg.Data[3] = NODE_HW_ID_BYTE3;
	
	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;
	
	StdCan_Put(&txMsg);
}
