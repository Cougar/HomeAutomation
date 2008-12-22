
#include "act_RGBdriver.h"
#define ALGO_FREQ		5
#define TARGET_CURRENT	250UL
#define TARGET_ADC		TARGET_CURRENT*4UL*1024UL/5000UL

#define TARGET_CURRENT2	50UL
#define TARGET_ADC2		TARGET_CURRENT2*4UL*3UL*1024UL/5000UL

#define TARGET_CURRENT3	50UL
#define TARGET_ADC3		TARGET_CURRENT3*4UL*3UL*1024UL/5000UL

#define TARGET_CURRENT4	50UL
#define TARGET_ADC4		TARGET_CURRENT4*4UL*3UL*1024UL/5000UL

/*

50mA*0.1R = 5mV
5mV*40ggr = 200mV
200mV*1024/5V = 41

50mA*0.1R = 5mV
5mV*40ggr = 200mV
50mA*0.1*40*1024/5 = 41
50mA*4*1024/5 = 41
*/

void calculatePWM(uint8_t timer) {
	uint16_t adcValue;
	adcValue = ADC_Get(5);

	if (adcValue > TARGET_ADC+5)
		OCR1B -=1;
	else if (adcValue < TARGET_ADC)
		OCR1B++;
	else if (adcValue > TARGET_ADC+10)
		OCR1B=0;


	adcValue = ADC_Get(4);

	if (adcValue > TARGET_ADC2+5)
		OCR1A -=1;
	else if (adcValue < TARGET_ADC2)
		OCR1A++;
	else if (adcValue > TARGET_ADC2+10)
		OCR1A=0;


	adcValue = ADC_Get(2);

	if (adcValue > TARGET_ADC3+5)
		OCR0A -=1;
	else if (adcValue < TARGET_ADC3)
		OCR0A++;
	else if (adcValue > TARGET_ADC3+10)
		OCR0A=0;


	adcValue = ADC_Get(0);

	if (adcValue > TARGET_ADC4+5)
		OCR0B -=1;
	else if (adcValue < TARGET_ADC4)
		OCR0B++;
	else if (adcValue > TARGET_ADC4+10)
		OCR0B=0;

}

void sendADCvalue(uint8_t timer) {
	StdCan_Msg_t txMsg;
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_RGBDRIVER; 
	txMsg.Header.ModuleId = act_RGBdriver_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_DEBUG;
	txMsg.Length = 6;

	//txMsg.Data[0] = (uint8_t)(ADC_Get(5)>>2);
	txMsg.Data[0] = (uint8_t)(ADC_Get(5)&0xff);
	txMsg.Data[1] = 0;
	txMsg.Data[2] = OCR1B;
	txMsg.Data[3] = 0;
	
	txMsg.Data[4] = TARGET_CURRENT;
	txMsg.Data[5] = TARGET_ADC;
	
	StdCan_Put(&txMsg);
}

void act_RGBdriver_Init(void)
{
	ADC_Init();
	
	//DDRB &= ~(1<<PB1);
	//DDRB &= ~(1<<PB2);	//never set PB2 as input 
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1A |= (1 << WGM10) | (0 << WGM11);	//8-bit pwm with top=0xff
	TCCR1B |= (1 << WGM12);		//fast pwm
	cli();
	OCR1A = 0;  // set on time (tp)
	OCR1B = 0;  // set on time (tp)
	sei();
	DDRB |= _BV(PB2);			//set port to output
	TCCR1A |= (1<<COM1A1);		//enable output compare
	DDRB |= _BV(PB1);			//set port to output
	TCCR1A |= (1<<COM1B1);		//enable output compare

	//cli();
	//OCR1A = 10;  // set on time (tp)
	//OCR1B = 40;  // set on time (tp)
	//sei();

	TCCR1B |= (1 << CS10);		//enable timer, prescaler=1


	// Contrast
	TCCR0A |= (1<<WGM01)|(1<<WGM00);
	TCCR0B |= (1<<CS00);
	cli();
	OCR0A = 0;  // set on time (tp)
	OCR0B = 0;  // set on time (tp)
	sei();
	DDRD |= (1<<DDD5);
	DDRD |= (1<<DDD6);
	TCCR0A |= (1<<COM0B1)|(1<<COM0A1);

	Timer_SetTimeout(act_RGBdriver_ADC_SEND_TIMER, 2000, TimerTypeFreeRunning, sendADCvalue);
	Timer_SetTimeout(act_RGBdriver_REGULATOR_TIMER, ALGO_FREQ, TimerTypeFreeRunning, calculatePWM);
}

void act_RGBdriver_Process(void)
{
	///TODO: Stuff that needs doing is done here
}

void act_RGBdriver_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT && 
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_RGBDRIVER && 
		rxMsg->Header.ModuleId == act_RGBdriver_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case 0:
		///TODO: Do something dummy
		break;
		}
	}
}

void act_RGBdriver_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_RGBDRIVER;
	txMsg.Header.ModuleId = act_RGBdriver_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_LIST;
	txMsg.Length = 6;

	txMsg.Data[0] = NODE_HW_ID_BYTE0;
	txMsg.Data[1] = NODE_HW_ID_BYTE1;
	txMsg.Data[2] = NODE_HW_ID_BYTE2;
	txMsg.Data[3] = NODE_HW_ID_BYTE3;
	
	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;
	
	StdCan_Put(&txMsg);
}
