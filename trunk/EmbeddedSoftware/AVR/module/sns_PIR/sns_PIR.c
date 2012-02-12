#include "sns_PIR.h"

#define ABS(a) ((a) < 0 ? (-a) : (a))


// a weighted moving avarage value is used to remove "DC offset"
static uint32_t zeroOffset = 0;

// a second avarage value is used to detect changes in avarage "energy"
#define NR_AVG_VALUES	15
static int16_t avgValues[NR_AVG_VALUES];
static uint8_t avgIndex = 0;


void sns_PIR_Init(void)
{
	ADC_Init();
	ltc2450_init();
	Timer_SetTimeout(sns_PIR_SAMPLE_TIMER, 	sns_PIR_SAMPLE_PERIOD, 	TimerTypeFreeRunning, 0);
	Timer_SetTimeout(sns_PIR_SEND_TIMER, 	sns_PIR_SEND_PERIOD, 	TimerTypeFreeRunning, 0);
}


void sns_PIR_Process(void)
{
	static uint8_t motionDetect = 0;
	uint16_t brightness;
	if (Timer_Expired(sns_PIR_SAMPLE_TIMER)) {
		/**
		 * Read PIR sensor (via external ADC)
		 */
		uint16_t pirValue;
		do {
			// if ADC is busy, returned value is 0xFFFF
			pirValue = ltc2450_read();
		} while (pirValue >= 0xFFFF);
		// first time?
		if (zeroOffset == 0) {
			zeroOffset = pirValue;
		}
		else {
			zeroOffset = (zeroOffset*4 + (uint32_t)pirValue)/5;
		}
		int8_t test = (int8_t)((int32_t)pirValue - (int32_t)zeroOffset);
		avgValues[avgIndex++] = test;
		if (avgIndex >= NR_AVG_VALUES) {
			avgIndex = 0;
		}
		uint16_t avgValue = 0;
		for (uint8_t i=0; i<NR_AVG_VALUES; i++) {
			avgValue += ABS(avgValues[i]);
		}
		
		/* Or to keep motiondetect==1 true for at least one CAN send period, so detections are never lost */
		motionDetect |= (avgValue >= 80);
		
	}
	if (Timer_Expired(sns_PIR_SEND_TIMER)) {
		/**
		 * Read brightness sensor (via internal ADC).
		 */
		brightness = ADC_Get(sns_PIR_BRIGHTNESS_AD);

		/**
		 * Time to send CAN message.
		 */
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_PIR;
		txMsg.Header.ModuleId = sns_PIR_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PIR_MOTION;
		txMsg.Length = 4;
		txMsg.Data[0] = 0;
		txMsg.Data[1] = (uint8_t)((brightness&0xFF00) >> 8);
		txMsg.Data[2] = (uint8_t)((brightness&0x00FF) >> 0);
		txMsg.Data[3] = motionDetect;
		StdCan_Put(&txMsg);
		// also print data to debug
		//printf("M:%u,B:%d\n", motionDetect, brightness);
		motionDetect = 0;
	}
}

void sns_PIR_HandleMessage(StdCan_Msg_t *rxMsg)
{
	/*if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_xyz && ///TODO: Change this to the actual class type
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_xyz && ///TODO: Change this to the actual module type
		rxMsg->Header.ModuleId == sns_PIR_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}*/
}

void sns_PIR_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_PIR;
	txMsg.Header.ModuleId = sns_PIR_ID;
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

