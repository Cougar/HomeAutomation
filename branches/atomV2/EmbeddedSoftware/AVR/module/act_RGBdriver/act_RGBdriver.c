
#include "act_RGBdriver.h"
#define ALGO_FREQ		100
#define ADC_W	5
#define ADC_R	4
#define ADC_G	2
#define ADC_B	0

#define OCR_W	OCR1B
#define OCR_R	OCR1A
#define OCR_G	OCR0A
#define OCR_B	OCR0B

#define TARGET_CURRENT_W	250UL
#define TARGET_ADC_W		TARGET_CURRENT_W*4UL*1024UL/5000UL

#define TARGET_CURRENT_R	50UL
#define TARGET_ADC_R		TARGET_CURRENT_R*4UL*3UL*1024UL/5000UL

#define TARGET_CURRENT_G	50UL
#define TARGET_ADC_G		TARGET_CURRENT_G*4UL*3UL*1024UL/5000UL

#define TARGET_CURRENT_B	50UL
#define TARGET_ADC_B		TARGET_CURRENT_B*4UL*3UL*1024UL/5000UL

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
	adcValue = ADC_Get(ADC_W);

	if (adcValue > TARGET_ADC_W+50)
		OCR_W=0;
	else if (adcValue > TARGET_ADC_W+16)
		OCR_W-=2;
	else if (adcValue < TARGET_ADC_W-5)
		OCR_W++;
	else if (adcValue > TARGET_ADC_W+5)
		OCR_W -=1;


	adcValue = ADC_Get(ADC_R);

	if (adcValue > TARGET_ADC_R+50)
		OCR_R=0;
	else if (adcValue > TARGET_ADC_R+16)
		OCR_R-=2;
	else if (adcValue < TARGET_ADC_R-5)
		OCR_R++;
	else if (adcValue > TARGET_ADC_R+5)
		OCR_R -=1;

	adcValue = ADC_Get(ADC_G);


	if (adcValue > TARGET_ADC_G+50)
		OCR_G=0;
	else if (adcValue > TARGET_ADC_G+16)
		OCR_G-=2;
	else if (adcValue < TARGET_ADC_G-5)
		OCR_G++;
	else if (adcValue > TARGET_ADC_G+5)
		OCR_G -=1;

	adcValue = ADC_Get(ADC_B);

	if (adcValue > TARGET_ADC_B+50)
		OCR_B=0;
	else if (adcValue > TARGET_ADC_B+16)
		OCR_B-=2;
	else if (adcValue < TARGET_ADC_B-5)
		OCR_B++;
	else if (adcValue > TARGET_ADC_B+5)
		OCR_B -=1;
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
	txMsg.Data[0] = (uint8_t)(ADC_Get(ADC_W)&0xff);
	txMsg.Data[1] = 0;
	txMsg.Data[2] = OCR1B;
	txMsg.Data[3] = TARGET_ADC_R;

	txMsg.Data[4] = TARGET_CURRENT_W;
	txMsg.Data[5] = TARGET_ADC_W;

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

	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}
