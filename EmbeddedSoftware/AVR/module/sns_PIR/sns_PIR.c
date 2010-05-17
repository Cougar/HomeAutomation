#include "sns_PIR.h"

#define ABS(a) ((a) < 0 ? (-1*a) : (a))


// a weighted moving avarage value is used to remove "DC offset"
static uint32_t zeroOffset = 0;

// a second avarage value is used to detect changes in avarage "energy"
#define NR_AVG_VALUES	15
static int8_t avgValues[NR_AVG_VALUES];
static uint8_t avgIndex = 0;


void sns_PIR_Init(void)
{
	ltc2450_init();
	Timer_SetTimeout(sns_PIR_BRIGHT_TIMER, 40, TimerTypeFreeRunning, 0);
}


void sns_PIR_Process(void)
{
	static uint8_t printCounter = 0;
	if (Timer_Expired(sns_PIR_BRIGHT_TIMER)) {
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
		uint8_t motionDetect = (avgValue >= 80);
		
				
		if (printCounter++ >= 200/40) {
			printCounter = 0;
			//printf("AD:%u\n", pirValue);
			printf("Motion: %u\n", motionDetect);
			
			/*StdCan_Msg_t txMsg;
			StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
			StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
			txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_PIR;
			txMsg.Header.ModuleId = sns_PIR_ID;
			txMsg.Header.Command = CAN_MODULE_CMD_PIR_MOTION;
			txMsg.Length = 3;
			txMsg.Data[0] = 0;
			txMsg.Data[1] = (uint8_t)((pirValue & 0xFF00) >> 2);
			txMsg.Data[2] = (uint8_t)((pirValue & 0x00FF) >> 0);
			StdCan_Put(&txMsg);*/
		}
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

	txMsg.Data[0] = NODE_HW_ID_BYTE0;
	txMsg.Data[1] = NODE_HW_ID_BYTE1;
	txMsg.Data[2] = NODE_HW_ID_BYTE2;
	txMsg.Data[3] = NODE_HW_ID_BYTE3;
	
	txMsg.Data[4] = NUMBER_OF_MODULES;
	txMsg.Data[5] = ModuleSequenceNumber;
	
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
}

