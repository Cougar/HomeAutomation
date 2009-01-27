
#include "act_dimmer230.h"

//#define USEDEMO

int8_t fadeSpeedCh1 = 0;
uint8_t fadeSpeedFracCh1 = 0;
uint8_t fadeTargetCh1 = 0;
uint8_t fadeSpeedCntCh1 = 0;

#ifdef USEDEMO
uint8_t demo = 0;
//uint8_t demo = 2;	//demo one way
//uint8_t demo = 3;	//disable demo 
uint8_t democnt = 0;
#define DEMOSTEPS 2
#define DEMOSTEPS2 1
#endif

//#define MEASURE_PERIOD
#ifdef MEASURE_PERIOD
uint16_t periodTime = 0;
			//measured during init (by taking the time difference between two zero cross)
			//not used, bad idea, the powerStepTable below asumes 10000
#endif
uint16_t xcTimeDiff=0;
			//zero cross sync (time between pos flank of xc-detection and real zero cross)
uint8_t dimmerValueCh1 = 0;
			//store in eeprom later
			//value of 0 is zero output, value of 255 is max output (ACT_DIMMMER230_MIN_DIM, ACT_DIMMMER230_MAX_DIM)
uint8_t state = ACT_DIMMMER230_STATE_IDLE;
const uint16_t powerStepTable[] PROGMEM = {845, 1068, 1225, 1352, 1459, 1553, 1638, 1716, 1788, 1855, 1917, 1976, 2032, 2086, 2138, 2188, 2236, 2282, 2327, 2370, 2412, 2453, 2493, 2532, 2570, 2607, 2643, 2679, 2714, 2748, 2782, 2815, 2848, 2880, 2911, 2942, 2972, 3002, 3032, 3061, 3090, 3119, 3147, 3175, 3203, 3230, 3257, 3284, 3311, 3337, 3363, 3389, 3415, 3440, 3465, 3490, 3515, 3539, 3563, 3587, 3611, 3635, 3659, 3682, 3705, 3728, 3751, 3774, 3797, 3820, 3843, 3865, 3887, 3909, 3931, 3953, 3975, 3997, 4019, 4041, 4062, 4083, 4104, 4125, 4146, 4167, 4188, 4209, 4230, 4251, 4272, 4292, 4313, 4333, 4354, 4374, 4395, 4415, 4435, 4455, 4476, 4496, 4516, 4536, 4556, 4576, 4596, 4615, 4635, 4655, 4675, 4694, 4714, 4734, 4754, 4773, 4793, 4813, 4832, 4852, 4872, 4891, 4911, 4931, 4951, 4970, 4990};

#if act_dimmer230_SYNC>0
uint8_t zeroCrossCnt = 0;
#endif

ISR (TIMER1_COMPA_vect) 
{
	//testa hur lång denna loop behöver vara
	//verkar fungera upp till 249-250 sen blir det för korta pulser
	act_dimmer230_CHAN1_PORT |= _BV(act_dimmer230_CHAN1_BIT);
	uint8_t dummycnt = 40;
	while (dummycnt > 0) 
	{
		dummycnt++;
		asm("nop");
		asm("nop");
	}
	act_dimmer230_CHAN1_PORT &= ~_BV(act_dimmer230_CHAN1_BIT);			
	
	TIMSK1=0;							//disable timer interrupt
	state = ACT_DIMMMER230_STATE_IDLE;
}

ISR (act_dimmer230_ZC_PCINT_vect) 
{
	//only execute if zerocross pin is low
	if (!(act_dimmer230_ZC_PIN&(1<<act_dimmer230_ZC_BIT))) 
	{
		uint16_t newTimerVal = xcTimeDiff;

		//here new timer value is calculated 

		if (dimmerValueCh1 > 0 && dimmerValueCh1 <= 127)
		{
			newTimerVal += ACT_DIMMMER230_PERIOD_TIME - pgm_read_word(&powerStepTable[dimmerValueCh1-1]);
		} 
		else if (dimmerValueCh1 < 255)
		{
			newTimerVal += pgm_read_word(&powerStepTable[254-dimmerValueCh1]);
		} 
		else if (dimmerValueCh1 == 255)
		{
			newTimerVal += 100;	//almost max, to make sure we don't fire before zerocross
		}
		
		if (newTimerVal > ACT_DIMMMER230_PERIOD_TIME)
		{
			newTimerVal -= ACT_DIMMMER230_PERIOD_TIME;
		}

		if (dimmerValueCh1 > 0) 
		{
			TCNT1 = 0;				//reset timer1 count register
			OCR1A = newTimerVal;

			//enable timer and timer interrupt
			TIFR1=(1<<OCF1A);		//clear interrupt flag before enabling interrupt
			TIMSK1|=(1<<OCIE1A);

			state = ACT_DIMMMER230_STATE_TIMER_ON;
		}
		
		fadeSpeedCntCh1++;
		if (fadeSpeedCntCh1 == fadeSpeedFracCh1) 
		{
			fadeSpeedCntCh1 = 0;
			uint8_t tempDimVal = dimmerValueCh1;
			dimmerValueCh1 += fadeSpeedCh1;
			if ((fadeSpeedCh1 > 0 && (dimmerValueCh1 < tempDimVal || dimmerValueCh1 >= fadeTargetCh1)) || 
				(fadeSpeedCh1 < 0 && (dimmerValueCh1 > tempDimVal || dimmerValueCh1 <= fadeTargetCh1))) 
			{
				dimmerValueCh1 = fadeTargetCh1;
				fadeSpeedCh1 = 0;
			}
		}
		
		
#if act_dimmer230_SYNC>0
		zeroCrossCnt++;
		if (zeroCrossCnt >= act_dimmer230_SYNC)
		{
			zeroCrossCnt = 0;
			//TODO: send sync-message on can

			StdCan_Msg_t txMsg;
			txMsg.Length = 6;
			txMsg.Data[0] = (newTimerVal>>8)&0xff;
			txMsg.Data[1] = newTimerVal&0xff;
			//txMsg.Data[2] = (periodTime>>8)&0xff;
			//txMsg.Data[3] = periodTime&0xff;
			txMsg.Data[4] = (xcTimeDiff>>8)&0xff;
			txMsg.Data[5] = xcTimeDiff&0xff;
			StdCan_Put(&txMsg);

#ifdef USEDEMO
			if (demo == 0 || demo == 2) {
				democnt++;
				if (democnt==DEMOSTEPS)
				{
					democnt = 0;
					uint8_t tempDim=dimmerValueCh1;
					dimmerValueCh1+=DEMOSTEPS2;
					if (dimmerValueCh1<tempDim && demo == 0) {
						dimmerValueCh1=ACT_DIMMMER230_MAX_DIM;
					}
				}
				if (dimmerValueCh1 == ACT_DIMMMER230_MAX_DIM && demo == 0) {
					demo = 1;
				}
			} else if (demo == 1) {
				democnt++;
				if (democnt==DEMOSTEPS)
				{
					democnt = 0;
					uint8_t tempDim=dimmerValueCh1;
					dimmerValueCh1-=DEMOSTEPS2;
					if (dimmerValueCh1>tempDim) {
						dimmerValueCh1=ACT_DIMMMER230_MIN_DIM;
					}
				}
				if (dimmerValueCh1 == ACT_DIMMMER230_MIN_DIM) {
					demo = 0;
				}
			}
#endif //USEDEMO
		}
#endif		
	}
}

void act_dimmer230_Init(void)
{
	// set dimmer channel1 port to output 0
	act_dimmer230_CHAN1_PORT &= ~_BV(act_dimmer230_CHAN1_BIT);
	act_dimmer230_CHAN1_DDR |= _BV(act_dimmer230_CHAN1_BIT);
	// set zero cross detection port to input, no pullup
	act_dimmer230_ZC_PORT &= ~_BV(act_dimmer230_ZC_BIT);
	act_dimmer230_ZC_DDR &= ~_BV(act_dimmer230_ZC_BIT);
	
	TIMSK1=0;							//disable timer interrupt
	TCCR1A=0;
	
	//wait for high (ac leaving zero cross)
	while (!(act_dimmer230_ZC_PIN&(1<<act_dimmer230_ZC_BIT))) { }
	TCCR1B=(1<<CS11);					//enable timer, set to prescaler 8, must be changed if cpu freq is changed
	//wait for low (ac approaching zero cross)
	while (act_dimmer230_ZC_PIN&(1<<act_dimmer230_ZC_BIT)) { }
	uint16_t timeAtFall = TCNT1;	//Timer_GetTicks();
	//wait for high (ac leaving zero cross)
	while (!(act_dimmer230_ZC_PIN&(1<<act_dimmer230_ZC_BIT))) { }
	uint16_t timeAtRise = TCNT1;	//Timer_GetTicks();

#ifdef MEASURE_PERIOD
	//wait for low (ac approaching zero cross)
	while (act_dimmer230_ZC_PIN&(1<<act_dimmer230_ZC_BIT)) { }
	//wait for high (ac leaving zero cross)
	while (!(act_dimmer230_ZC_PIN&(1<<act_dimmer230_ZC_BIT))) { }
	uint16_t timeAtRise2 = TCNT1;	//Timer_GetTicks();
	if (timeAtRise < timeAtRise2) {
		periodTime = timeAtRise2 - timeAtRise;
	}
#endif

	// warning, watch out for wrap around on timestamps (something is broken if wrapped)
	if (timeAtFall < timeAtRise) {
		//calculate time from falling edge to ac zero cross value
		xcTimeDiff = timeAtRise-timeAtFall;
		xcTimeDiff = xcTimeDiff >> 1; //divide by two
	}

	//setup interrupt on zerocross, pcint
	PCMSK0=(1<<act_dimmer230_ZC_PCINT_BIT);
	PCIFR=(1<<act_dimmer230_ZC_PCIF);	//clear any pending interrupt before enabling interrupts
	PCICR=(1<<act_dimmer230_ZC_PCIE);	//enable interrupt for PCINT


	//printf("Hello world!\n");
}

void act_dimmer230_Process(void)
{
	///TODO: Stuff that needs doing is done here
}

void act_dimmer230_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_DIMMER230 && 
		rxMsg->Header.ModuleId == act_dimmer230_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_DIMMER230_DEMO:		// Demo(channel, speed, steps)
			if (rxMsg->Length == 3) {
				
			}
		break;
		
		case CAN_MODULE_CMD_DIMMER230_START_FADE:	// StartFade(channel, speed, direction), just an alias of ABS_FADE
			if (rxMsg->Length == 3) {
				uint8_t channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint8_t direction = rxMsg->Data[2];
				
				fadeSpeedCntCh1 = 0;
				fadeSpeedCh1 = 0;
				uint8_t endValue = 0;
				if (direction == 1) {
					endValue = ACT_DIMMMER230_MAX_DIM;
				} else if (direction == 0) {
					endValue = ACT_DIMMMER230_MIN_DIM;
				}
					
				if (speed == 0) {
					dimmerValueCh1 = endValue;	//set dimmer value immediately
				} else {
					fadeTargetCh1 = endValue;
					if (fadeTargetCh1 != dimmerValueCh1) {
						if ((speed&0x80) == 0) {
							fadeSpeedCh1 = speed&0x7f;
							fadeSpeedFracCh1 = 1;
						} else {
							fadeSpeedCh1 = 1;
							fadeSpeedFracCh1 = speed&0x7f;
						}
						if (fadeTargetCh1 < dimmerValueCh1) {
							fadeSpeedCh1 = -fadeSpeedCh1;
						}
					}
				}
			}
		break;

		case CAN_MODULE_CMD_DIMMER230_STOP_FADE:	// StopFade(channel)
			if (rxMsg->Length == 1) {
				fadeSpeedCh1 = 0;
			}
		break;

		case CAN_MODULE_CMD_DIMMER230_ABS_FADE:	// AbsFade(channel, speed, endValue)
			if (rxMsg->Length == 3) {
				uint8_t channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint8_t endValue = rxMsg->Data[2];

				fadeSpeedCntCh1 = 0;
				fadeSpeedCh1 = 0;
				if (speed == 0) {
					dimmerValueCh1 = endValue;	//set dimmer value immediately
				} else {
					fadeTargetCh1 = endValue;
					if (fadeTargetCh1 != dimmerValueCh1) {
						if ((speed&0x80) == 0) {
							fadeSpeedCh1 = speed&0x7f;
							fadeSpeedFracCh1 = 1;
						} else {
							fadeSpeedCh1 = 1;
							fadeSpeedFracCh1 = speed&0x7f;
						}
						if (fadeTargetCh1 < dimmerValueCh1) {
							fadeSpeedCh1 = -fadeSpeedCh1;
						}
					}
				}
			}
		break;

		case CAN_MODULE_CMD_DIMMER230_REL_FADE:	// RelFade(channel, speed, direction, steps)
			if (rxMsg->Length == 4) {
				uint8_t channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint8_t direction = rxMsg->Data[2];
				uint8_t steps = rxMsg->Data[3];
				
				fadeSpeedCntCh1 = 0;
				fadeSpeedCh1 = 0;
				uint8_t tempDimVal = dimmerValueCh1;
				uint8_t tempDimVal2 = dimmerValueCh1;
				if (direction == 1) {					//if increase
					tempDimVal2 += steps;				//calculate new value
					if (tempDimVal2 < tempDimVal) {		//make overflow test
						tempDimVal2 = ACT_DIMMMER230_MAX_DIM;
					}
				} else if (direction == 0) {			//if decrease
					tempDimVal2 -= steps;
					if (tempDimVal2 > tempDimVal) {
						tempDimVal2 = ACT_DIMMMER230_MIN_DIM;
					}
				}
				if (speed == 0) {
					dimmerValueCh1 = tempDimVal2;		//set dimmer value immediately
				} else {
					fadeTargetCh1 = tempDimVal2;		//set the fade target
					
					if (fadeTargetCh1 != dimmerValueCh1) {
						if ((speed&0x80) == 0) {
							fadeSpeedCh1 = speed&0x7f;
							fadeSpeedFracCh1 = 1;
						} else {
							fadeSpeedCh1 = 1;
							fadeSpeedFracCh1 = speed&0x7f;
						}
						if (fadeTargetCh1 < dimmerValueCh1) {
							fadeSpeedCh1 = -fadeSpeedCh1;
						}
					}
				}
				
			}
		break;
}
	}
}

void act_dimmer230_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); 
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_DIMMER230;
	txMsg.Header.ModuleId = act_dimmer230_ID;
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
