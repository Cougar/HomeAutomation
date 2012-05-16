
#include "act_dimmer230.h"
#include "act_dimmer230_eeprom.h"

#ifdef CHN_LOADED
#include "../chn_ChnMaster/chn_ChnMaster.h"
#endif

#ifdef CHN_LOADED
void act_dimmer230_chn_update( uint16_t channel_id, uint16_t value );
#endif

int8_t fadeSpeed = 0;
uint8_t fadeSpeedFrac = 0;
uint8_t fadeTarget = 0;
uint8_t fadeSpeedCnt = 0;

uint8_t demoEndValue = 0;
uint8_t demoHighValue = 0;
uint8_t demoState = ACT_DIMMMER230_DEMO_STATE_NOT_RUNNING;

/*	This bool is set to make process routine send the NetInfo packet */
uint8_t sendNetInfo = 0;

/*	netConnected stores the connection status, if connected to the net or not */
uint8_t netConnected = CAN_MODULE_ENUM_DIMMER230_NETINFO_CONNECTION_DISCONNECTED;

/*	frequency stores the evaluated frequency, 50 or 60 Hz
	don't calculate net frequency since the internal oscillator is not to be trusted, instead check if 50Hz or 60Hz system
	should be used for switching powerStepTable depending on frequency */
uint8_t frequency = CAN_MODULE_ENUM_DIMMER230_NETINFO_FREQUENCY_ERROR_UNKNOWN;
/*	zero cross sync (time between pos flank of xc-detection and real zero cross) */
uint16_t xcTimeDiff=0;

/*	dimmerValue is the current output
	value of 0 is zero output, value of 255 is max output (ACT_DIMMMER230_MIN_DIM, ACT_DIMMMER230_MAX_DIM)
	stored in eeprom, and restored at startup */
uint8_t dimmerValue = ACT_DIMMMER230_MIN_DIM;

uint8_t state = ACT_DIMMMER230_STATE_IDLE;

/*	powerStepTable is used for stepping the output for constant increase/decrease in power, this should be a quater of a sin^2-wave (since the second quater can be calculated) */
const uint16_t powerStepTable[] PROGMEM = {845, 1068, 1225, 1352, 1459, 1553, 1638, 1716, 1788, 1855, 1917, 1976, 2032, 2086, 2138, 2188, 2236, 2282, 2327, 2370, 2412, 2453, 2493, 2532, 2570, 2607, 2643, 2679, 2714, 2748, 2782, 2815, 2848, 2880, 2911, 2942, 2972, 3002, 3032, 3061, 3090, 3119, 3147, 3175, 3203, 3230, 3257, 3284, 3311, 3337, 3363, 3389, 3415, 3440, 3465, 3490, 3515, 3539, 3563, 3587, 3611, 3635, 3659, 3682, 3705, 3728, 3751, 3774, 3797, 3820, 3843, 3865, 3887, 3909, 3931, 3953, 3975, 3997, 4019, 4041, 4062, 4083, 4104, 4125, 4146, 4167, 4188, 4209, 4230, 4251, 4272, 4292, 4313, 4333, 4354, 4374, 4395, 4415, 4435, 4455, 4476, 4496, 4516, 4536, 4556, 4576, 4596, 4615, 4635, 4655, 4675, 4694, 4714, 4734, 4754, 4773, 4793, 4813, 4832, 4852, 4872, 4891, 4911, 4931, 4951, 4970, 4990};

#if act_dimmer230_SYNC>0
uint8_t zeroCrossCnt = 0;
#endif


/*	Set the variable to "disconnected" if no zerocross have arrived for a while */
void Net_Connection_callback(uint8_t timer)
{
	netConnected = CAN_MODULE_ENUM_DIMMER230_NETINFO_CONNECTION_DISCONNECTED;
}

/*	This function will send the dimmer-status-packet containing connection status and frequency */
void Send_Status_callback(uint8_t timer)
{
	sendNetInfo = 1;
}

/*	When the timer reaches its compare value the triac should be trigged */
ISR (TIMER1_COMPA_vect) 
{
	gpio_set_pin(act_dimmer230_CHAN1_IO);
	uint8_t dummycnt = 40;
	while (dummycnt > 0) 
	{
		dummycnt++;
		asm("nop");
		asm("nop");
	}
	gpio_clr_pin(act_dimmer230_CHAN1_IO);
	
	TIMSK1=0;							//disable timer interrupt
	state = ACT_DIMMMER230_STATE_IDLE;
}

#ifdef CHN_LOADED
void act_dimmer230_chn_update( uint16_t channel_id, uint16_t value ) {
    fadeTarget = dimmerValue = value>>8;
}
#endif

/*	Interrupted at a pin change on zero-cross pin 
	The zero-cross pin will go high just before the real zero-cross and
	low just after the real zero-cross. The time difference is measured
	and adjusted for. */
void act_dimmer230_pcint_callback(uint8_t id, uint8_t status) //ISR (act_dimmer230_ZC_PCINT_vect) 
{
	/* only execute if zerocross pin is low (after real zerocross). 
	Calculate the diff from real zero-cross.
	*/
	if (!status)
	{
		xcTimeDiff = TCNT1;
		xcTimeDiff = xcTimeDiff >> 1; //divide by two
		if (xcTimeDiff > 600) {
			xcTimeDiff = 600;
		}
	}	
	/* only execute if zerocross pin is high (before real zerocross) */
	else
	{
		/* Reset the timer for checking net connection. If no zero-cross has come in 500ms then consider us disconnected */
		Timer_SetTimeout(act_dimmer230_NET_CONNECT_TIMEOUT, 500, TimerTypeOneShot, &Net_Connection_callback);
		/* Currently we are connected */
		netConnected = CAN_MODULE_ENUM_DIMMER230_NETINFO_CONNECTION_CONNECTED;
		
		/* Check frequency by measure the time since last zero-cross */
		uint16_t periodTime = TCNT1;
		if (periodTime > ACT_DIMMMER230_50HZ_PERIOD_TIME-ACT_DIMMMER230_50HZ_PERIOD_TIME/10 && periodTime < ACT_DIMMMER230_50HZ_PERIOD_TIME+ACT_DIMMMER230_50HZ_PERIOD_TIME/10)
		{
			frequency = CAN_MODULE_ENUM_DIMMER230_NETINFO_FREQUENCY_50HZ;
		}
		else if (periodTime > ACT_DIMMMER230_60HZ_PERIOD_TIME-ACT_DIMMMER230_60HZ_PERIOD_TIME/10 && periodTime < ACT_DIMMMER230_60HZ_PERIOD_TIME+ACT_DIMMMER230_60HZ_PERIOD_TIME/10)
		{
			frequency = CAN_MODULE_ENUM_DIMMER230_NETINFO_FREQUENCY_60HZ;
		}
		else
		{
			frequency = CAN_MODULE_ENUM_DIMMER230_NETINFO_FREQUENCY_ERROR_UNKNOWN;
		}
		
		/* Check demo states */
		if (dimmerValue == fadeTarget) {
			switch (demoState)
			{
			case ACT_DIMMMER230_DEMO_STATE_NOT_RUNNING:
			break;
			case ACT_DIMMMER230_DEMO_STATE_DECREASE:
				demoState = ACT_DIMMMER230_DEMO_STATE_INCREASE;
				fadeTarget = demoHighValue;
				fadeSpeed = -fadeSpeed;
			break;
			case ACT_DIMMMER230_DEMO_STATE_INCREASE:
				demoState = ACT_DIMMMER230_DEMO_STATE_GOBACK;
				fadeTarget = demoEndValue;
				fadeSpeed = -fadeSpeed;
			break;
			case ACT_DIMMMER230_DEMO_STATE_GOBACK:
				demoState = ACT_DIMMMER230_DEMO_STATE_NOT_RUNNING;
			break;
			}
		}
		
		/* Calculate new time to triac triggering */
		uint16_t newTimerVal = xcTimeDiff;		/* add the zerocross timediff */

		if (dimmerValue > 0 && dimmerValue <= 127)
		{
			newTimerVal += ACT_DIMMMER230_50HZ_PERIOD_TIME - pgm_read_word(&powerStepTable[dimmerValue-1]);
		} 
		else if (dimmerValue < 255)
		{
			newTimerVal += pgm_read_word(&powerStepTable[254-dimmerValue]);
		} 
		else if (dimmerValue == 255)
		{
			newTimerVal += 100;	/* almost max, to make sure we don't fire before zerocross */
			gpio_set_pin(act_dimmer230_CHAN1_IO);
		}
		
		if (newTimerVal > ACT_DIMMMER230_50HZ_PERIOD_TIME)
		{
			newTimerVal -= ACT_DIMMMER230_50HZ_PERIOD_TIME;
		}

		TCNT1 = 0;				/* reset timer1 count register */
		if (dimmerValue > 0 && dimmerValue != 255 ) 	/* dont trigger the triac if dimmerValue is 0 */
		{
			OCR1A = newTimerVal;

			/* enable timer and timer interrupt */
			TIFR1=(1<<OCF1A);		/* clear interrupt flag before enabling interrupt */
			TIMSK1|=(1<<OCIE1A);

			state = ACT_DIMMMER230_STATE_TIMER_ON;
		}
		
		/* Change the dimmerValue according to the current fading */
		fadeSpeedCnt++;
		if (fadeSpeedCnt == fadeSpeedFrac && dimmerValue != fadeTarget) 
		{
			fadeSpeedCnt = 0;
			uint8_t tempDimVal = dimmerValue;
			dimmerValue += fadeSpeed;
			if ((fadeSpeed > 0 && (dimmerValue < tempDimVal || dimmerValue >= fadeTarget)) || 
				(fadeSpeed < 0 && (dimmerValue > tempDimVal || dimmerValue <= fadeTarget))) 
			{
				dimmerValue = fadeTarget;
			}
			
			/* if targetvalue was reached then send the netinfo packet */
			if (dimmerValue == fadeTarget)
			{
				sendNetInfo = 1;
			}
			/* 5 seconds after we have stopped fading we should store the current dimmerValue to eeprom */
			Timer_SetTimeout(act_dimmer230_STORE_VALUE_TIMEOUT, 5000, TimerTypeOneShot, 0);
		}
		
#if act_dimmer230_SYNC>0
		/* Currently disabled, must add this packet to data.xml */
		zeroCrossCnt++;
		if (zeroCrossCnt >= act_dimmer230_SYNC)
		{
			zeroCrossCnt = 0;
			//TODO: send sync-message on can
#if 0
			StdCan_Msg_t txMsg;
			txMsg.Length = 6;
			txMsg.Data[0] = (newTimerVal>>8)&0xff;
			txMsg.Data[1] = newTimerVal&0xff;
			txMsg.Data[2] = frequency;	//(periodTime>>8)&0xff;
			//txMsg.Data[3] = periodTime&0xff;
			txMsg.Data[4] = (xcTimeDiff>>8)&0xff;
			txMsg.Data[5] = xcTimeDiff&0xff;
			StdCan_Put(&txMsg); 
#endif
		}
#endif		
	}
}

#if act_dimmer230_USEEEPROM==1
struct eeprom_act_dimmer230 EEMEM eeprom_act_dimmer230 = 
{
	{
		/* Define initialization values on the EEPROM variables here, 
		this will generate a *.eep file that can be used to store this values to the node, 
		can in future be done with a EEPROM module and the make-scrips. 
		Write the values in the exact same order as the struct is defined in the *.h file. */
		0x00	/* eeDimmerValue */
	},
	0	/* crc, must be a correct value, but this will also be handled by the EEPROM module or make scripts */
}; 
#endif

/* 	Init function of dimmer-module, will init hardware such as timer and gpio and load 
	saved eeprom value */
void act_dimmer230_Init(void)
{
#if act_dimmer230_USEEEPROM==1
	if (EEDATA_OK)
	{
	  /* Use stored data to set initial values for the module */
	  dimmerValue = eeprom_read_byte(EEDATA.eeDimmerValue);
	} else
	{	/* The CRC of the EEPROM is not correct, store default values and update CRC */
	  eeprom_write_byte_crc(EEDATA.eeDimmerValue, 0x00, WITHOUT_CRC);
	  EEDATA_UPDATE_CRC;
	}
#endif

	/* set dimmer port to output 0 */
	gpio_clr_pin(act_dimmer230_CHAN1_IO);
	gpio_set_out(act_dimmer230_CHAN1_IO);
	/* set zero cross detection port to input, no pullup */
	gpio_set_in(act_dimmer230_ZC_IO);
	gpio_clr_pullup(act_dimmer230_ZC_IO);
	
	TIMSK1=0;							/* disable timer interrupt */
	TCCR1A=0;
	
	TCCR1B=(1<<CS11);					/* enable timer, set to prescaler 8, must be changed if cpu freq is changed */

#ifdef CHN_LOADED
    chn_ChnMaster_RegisterListener( act_dimmer230_CHN_CHANNEL, act_dimmer230_chn_update );
#endif

	/* setup interrupt on zerocross, pcint */
	//act_dimmer230_ZC_PCMSK=(1<<(act_dimmer230_ZC_PCINT_BIT));
	//PCIFR=(1<<act_dimmer230_ZC_PCIF);	/* clear any pending interrupt before enabling interrupts */
	//PCICR=(1<<act_dimmer230_ZC_PCIE);	/* enable interrupt for PCINT */
	Pcint_SetCallbackPin(act_dimmer230_PCINT, act_dimmer230_ZC_IO, &act_dimmer230_pcint_callback);
	
	/* Setup timeout for sending the net status packet */
	Timer_SetTimeout(act_dimmer230_SEND_STATUS_TIMEOUT, act_dimmer230_SEND_STATUS_INTERVAL*1000, TimerTypeFreeRunning, &Send_Status_callback);
}


/*	Process funtion of dimmer-module, currently everything is done interrupted */
void act_dimmer230_Process(void)
{
	/* Send netinfo packet (if dimmervalue has changed, and periodically) */
	if (sendNetInfo)
	{
		sendNetInfo = 0;
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_DIMMER230;
		txMsg.Header.ModuleId = act_dimmer230_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_DIMMER230_NETINFO;
		txMsg.Length = 3;
		txMsg.Data[0] = ((netConnected&0x1)<<7 | (frequency&0x3)<<5);
		txMsg.Data[1] = dimmerValue;
		txMsg.Data[2] = 0;  //Channel
		StdCan_Put(&txMsg);
	}
	
	/* If dimmervalue was changed over 5 seconds ago then store the new dimmervalue */
	if (Timer_Expired(act_dimmer230_STORE_VALUE_TIMEOUT))
	{
		eeprom_write_byte_crc(EEDATA.eeDimmerValue, dimmerValue, WITH_CRC);
	}
}


/*	Handle incoming message function of dimmer-module */
void act_dimmer230_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_ACT &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_TO_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_ACT_DIMMER230 && 
		rxMsg->Header.ModuleId == act_dimmer230_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_MODULE_CMD_DIMMER230_DEMO:		/* Demo(channel, speed, steps) */
			if (rxMsg->Length == 3) {
				uint8_t channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint8_t steps = rxMsg->Data[2];
				
				fadeSpeedCnt = 0;
				fadeSpeed = 0;
				if (speed == 0) {
					/* do nothing */
				} else {
					uint8_t diffToMin = dimmerValue - ACT_DIMMMER230_MIN_DIM;
					uint8_t diffToMax = ACT_DIMMMER230_MAX_DIM - dimmerValue;
				
					demoEndValue = dimmerValue;
					if (diffToMin >= steps && diffToMax >= steps)
					{
						/* not close to min or max */
						fadeTarget = dimmerValue - steps;
						demoHighValue = dimmerValue + steps;
					}
					else if (diffToMin >= steps)
					{
						/* close to max */
						fadeTarget = dimmerValue - steps - steps + diffToMax;
						demoHighValue = ACT_DIMMMER230_MAX_DIM;
					}
					else if (diffToMax >= steps)
					{
						/* close to min */
						fadeTarget = ACT_DIMMMER230_MIN_DIM;
						demoHighValue = dimmerValue + steps + steps - diffToMin;
					}
					demoState = ACT_DIMMMER230_DEMO_STATE_DECREASE;
				
					if ((speed&0x80) == 0x80) {
						fadeSpeed = (speed&0x7f)+1;
						fadeSpeedFrac = 1;
					} else {
						fadeSpeed = 1;
						fadeSpeedFrac = 0x80-(speed&0x7f);
					}
					if (fadeTarget <= dimmerValue) {
						fadeSpeed = -fadeSpeed;
					}
				}
			}
		break;
		
		case CAN_MODULE_CMD_DIMMER230_START_FADE:	/* StartFade(channel, speed, direction) */
			if (rxMsg->Length == 3) {
				demoState = ACT_DIMMMER230_DEMO_STATE_NOT_RUNNING;
				uint8_t channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint8_t direction = rxMsg->Data[2];
				
				fadeSpeedCnt = 0;
				fadeSpeed = 0;
				uint8_t endValue = 0;
				if (direction == CAN_MODULE_ENUM_DIMMER230_START_FADE_DIRECTION_INCREASE) {
					endValue = ACT_DIMMMER230_MAX_DIM;
				} else if (direction == CAN_MODULE_ENUM_DIMMER230_START_FADE_DIRECTION_DECREASE) {
					endValue = ACT_DIMMMER230_MIN_DIM;
				}
					
				if (speed == 0) {
					dimmerValue = endValue;	/* set dimmer value immediately */
					sendNetInfo = 1;		/* send netinfo with the current dimmervalue*/
				} else {
					fadeTarget = endValue;
					if (fadeTarget != dimmerValue) {
						if ((speed&0x80) == 0x80) {
							fadeSpeed = (speed&0x7f)+1;
							fadeSpeedFrac = 1;
						} else {
							fadeSpeed = 1;
							fadeSpeedFrac = 0x80-(speed&0x7f);
						}
						if (fadeTarget < dimmerValue) {
							fadeSpeed = -fadeSpeed;
						}
					}
				}
			}
		break;

		case CAN_MODULE_CMD_DIMMER230_STOP_FADE:	/* StopFade(channel) */
			if (rxMsg->Length == 1) {
				demoState = ACT_DIMMMER230_DEMO_STATE_NOT_RUNNING;
				fadeSpeed = 0;
				sendNetInfo = 1;		/* send netinfo with the current dimmervalue*/
			}
		break;

		case CAN_MODULE_CMD_DIMMER230_ABS_FADE:	/* AbsFade(channel, speed, endValue) */
			if (rxMsg->Length == 3) {
				demoState = ACT_DIMMMER230_DEMO_STATE_NOT_RUNNING;
				uint8_t channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint8_t endValue = rxMsg->Data[2];

				fadeSpeedCnt = 0;
				fadeSpeed = 0;
				if (speed == 0) {
					dimmerValue = endValue;	/* set dimmer value immediately */
					sendNetInfo = 1;		/* send netinfo with the current dimmervalue*/
				} else {
					fadeTarget = endValue;
					if (fadeTarget != dimmerValue) {
						if ((speed&0x80) == 0x80) {
							fadeSpeed = (speed&0x7f)+1;
							fadeSpeedFrac = 1;
						} else {
							fadeSpeed = 1;
							fadeSpeedFrac = 0x80-(speed&0x7f);
						}
						if (fadeTarget < dimmerValue) {
							fadeSpeed = -fadeSpeed;
						}
					}
				}
			}
		break;

		case CAN_MODULE_CMD_DIMMER230_REL_FADE:	/* RelFade(channel, speed, direction, steps) */
			if (rxMsg->Length == 4) {
				demoState = ACT_DIMMMER230_DEMO_STATE_NOT_RUNNING;
				uint8_t channel = rxMsg->Data[0];
				uint8_t speed = rxMsg->Data[1];
				uint8_t direction = rxMsg->Data[2];
				uint8_t steps = rxMsg->Data[3];
				
				fadeSpeedCnt = 0;
				fadeSpeed = 0;
				uint8_t tempDimVal = dimmerValue;
				uint8_t tempDimVal2 = dimmerValue;
				if (direction == CAN_MODULE_ENUM_DIMMER230_REL_FADE_DIRECTION_INCREASE) {					/* if increase */
					tempDimVal2 += steps;				/* calculate new value */
					if (tempDimVal2 < tempDimVal) {		/* make overflow test */
						tempDimVal2 = ACT_DIMMMER230_MAX_DIM;
					}
				} else if (direction == CAN_MODULE_ENUM_DIMMER230_REL_FADE_DIRECTION_DECREASE) {			/* if decrease */
					tempDimVal2 -= steps;
					if (tempDimVal2 > tempDimVal) {
						tempDimVal2 = ACT_DIMMMER230_MIN_DIM;
					}
				}
				if (speed == 0) {
					dimmerValue = tempDimVal2;		/* set dimmer value immediately */
					sendNetInfo = 1;				/* send netinfo with the current dimmervalue*/
				} else {
					fadeTarget = tempDimVal2;		/* set the fade target */
					
					if (fadeTarget != dimmerValue) {
						if ((speed&0x80) == 0x80) {
							fadeSpeed = (speed&0x7f)+1;
							fadeSpeedFrac = 1;
						} else {
							fadeSpeed = 1;
							fadeSpeedFrac = 0x80-(speed&0x7f);
						}
						if (fadeTarget < dimmerValue) {
							fadeSpeed = -fadeSpeed;
						}
					}
				}
			}
		break;
		}
	}
}

/* List function of dimmer-module */
void act_dimmer230_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_ACT); 
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_ACT_DIMMER230;
	txMsg.Header.ModuleId = act_dimmer230_ID;
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

