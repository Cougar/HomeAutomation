
#include "tst_Benchmark.h"


/*
Benchmark test usage of timer1

CanSend
Uses Timer1 as free running timer, only reads counter value

CpuTime
Uses Timer1 as free running timer, only reads counter value


*/


#if tst_Benchmark_CanSend_TestActive == 1
uint16_t tst_Benchmark_CanSend_Min = 0xffff;
uint16_t tst_Benchmark_CanSend_Max = 0x0000;
uint16_t tst_Benchmark_CanSend_Prev = 0xffff;
StdCan_Msg_t tst_Benchmark_CanSend_txMsg;
#endif

#if tst_Benchmark_CpuTime_TestActive == 1
uint16_t tst_Benchmark_CpuTime_Min = 0xffff;
uint16_t tst_Benchmark_CpuTime_Max = 0x0000;
uint16_t tst_Benchmark_CpuTime_Avg = 0x0000;
uint16_t tst_Benchmark_CpuTime_Avg_Cnt = 0;
StdCan_Msg_t tst_Benchmark_CpuTime_txMsg;
uint16_t tst_Benchmark_CpuTime_timer_val;
#endif


void tst_Benchmark_Init(void)
{
	/* Set up benchmark timer (timer1, 16bit) */
	//Normal port operation, Normal mode
	TCCR1A = ((0<<COM1A1)|(0<<COM1A0)|(0<<COM1B1)|(0<<COM1B0)|(0<<WGM11)|(0<<WGM10));
	TCCR1B = ((0<<ICNC1)|(0<<ICES1)|(0<<WGM12)|(0<<WGM13));
	
	/* Start timer */
	//prescaler /8
	TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));

#if tst_Benchmark_CanSend_TestActive == 1
	StdCan_Set_class(tst_Benchmark_CanSend_txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(tst_Benchmark_CanSend_txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	tst_Benchmark_CanSend_txMsg.Header.ModuleType = CAN_MODULE_TYPE_TST_DEBUG;
	tst_Benchmark_CanSend_txMsg.Header.ModuleId = tst_Benchmark_ID;
	tst_Benchmark_CanSend_txMsg.Header.Command = CAN_MODULE_CMD_DEBUG_BENCHMARK;
	tst_Benchmark_CanSend_txMsg.Length = 8;

	Timer_SetTimeout(tst_Benchmark_CanSend_TIMER, tst_Benchmark_CanSend_PERIOD_MS , TimerTypeFreeRunning, 0);
#endif

#if tst_Benchmark_CpuTime_TestActive == 1
	StdCan_Set_class(tst_Benchmark_CpuTime_txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(tst_Benchmark_CpuTime_txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	tst_Benchmark_CpuTime_txMsg.Header.ModuleType = CAN_MODULE_TYPE_TST_DEBUG;
	tst_Benchmark_CpuTime_txMsg.Header.ModuleId = tst_Benchmark_ID;
	tst_Benchmark_CpuTime_txMsg.Header.Command = CAN_MODULE_CMD_DEBUG_BENCHMARK;
	tst_Benchmark_CpuTime_txMsg.Length = 8;

	Timer_SetTimeout(tst_Benchmark_CpuTime_SEND_TIMER, tst_Benchmark_CpuTime_SEND_PERIOD_MS , TimerTypeFreeRunning, 0);
#endif
}

void tst_Benchmark_Process(void)
{
#if tst_Benchmark_CanSend_TestActive == 1
	if (Timer_Expired(tst_Benchmark_CanSend_TIMER))
	{
		uint16_t timer_val;
		uint16_t timer_val_new;
		
		for (uint8_t i=0; i < tst_Benchmark_CanSend_NumTx; i++)
		{
			tst_Benchmark_CanSend_txMsg.Data[0] = 0; // Implement avarage? Write test number?
			tst_Benchmark_CanSend_txMsg.Data[1] = 0;
			tst_Benchmark_CanSend_txMsg.Data[2] = (tst_Benchmark_CanSend_Prev>>8)&0xff;
			tst_Benchmark_CanSend_txMsg.Data[3] = tst_Benchmark_CanSend_Prev&0xff;
			tst_Benchmark_CanSend_txMsg.Data[4] = (tst_Benchmark_CanSend_Min>>8)&0xff;
			tst_Benchmark_CanSend_txMsg.Data[5] = tst_Benchmark_CanSend_Min&0xff;
			tst_Benchmark_CanSend_txMsg.Data[6] = (tst_Benchmark_CanSend_Max>>8)&0xff;
			tst_Benchmark_CanSend_txMsg.Data[7] = tst_Benchmark_CanSend_Max&0xff;

			timer_val = TCNT1;
			/* buffers will be filled when sending more than 2-3 messages, so retry until sent */
			while (StdCan_Put(&tst_Benchmark_CanSend_txMsg) != StdCan_Ret_OK) {}
			timer_val_new = TCNT1;
			
			tst_Benchmark_CanSend_Prev = timer_val_new - timer_val;
			if (tst_Benchmark_CanSend_Min > tst_Benchmark_CanSend_Prev)
			{
				tst_Benchmark_CanSend_Min = tst_Benchmark_CanSend_Prev;
			}
			if (tst_Benchmark_CanSend_Max < tst_Benchmark_CanSend_Prev)
			{
				tst_Benchmark_CanSend_Max = tst_Benchmark_CanSend_Prev;
			}
		}
		
	}
#endif

#if tst_Benchmark_CpuTime_TestActive == 1
	uint16_t timer_val_new = TCNT1;

	uint16_t timeSpent = timer_val_new - tst_Benchmark_CpuTime_timer_val;
	if (tst_Benchmark_CpuTime_Min > timeSpent)
	{
		tst_Benchmark_CpuTime_Min = timeSpent;
	}
	if (tst_Benchmark_CpuTime_Max < timeSpent)
	{
		tst_Benchmark_CpuTime_Max = timeSpent;
	}
	
	uint32_t sum = tst_Benchmark_CpuTime_Avg * tst_Benchmark_CpuTime_Avg_Cnt + timeSpent;
	if (tst_Benchmark_CpuTime_Avg_Cnt < 0xffff)
	{
		tst_Benchmark_CpuTime_Avg_Cnt++;
	}
	tst_Benchmark_CpuTime_Avg = sum / tst_Benchmark_CpuTime_Avg_Cnt;

	if (Timer_Expired(tst_Benchmark_CpuTime_SEND_TIMER))
	{
		tst_Benchmark_CpuTime_txMsg.Data[0] = 0; // Write test number?
		tst_Benchmark_CpuTime_txMsg.Data[1] = 0;
		tst_Benchmark_CpuTime_txMsg.Data[2] = (tst_Benchmark_CpuTime_Avg>>8)&0xff;
		tst_Benchmark_CpuTime_txMsg.Data[3] = tst_Benchmark_CpuTime_Avg&0xff;
		tst_Benchmark_CpuTime_txMsg.Data[4] = (tst_Benchmark_CpuTime_Min>>8)&0xff;
		tst_Benchmark_CpuTime_txMsg.Data[5] = tst_Benchmark_CpuTime_Min&0xff;
		tst_Benchmark_CpuTime_txMsg.Data[6] = (tst_Benchmark_CpuTime_Max>>8)&0xff;
		tst_Benchmark_CpuTime_txMsg.Data[7] = tst_Benchmark_CpuTime_Max&0xff;

		/* buffers will be filled when sending more than 2-3 messages, so retry until sent */
		while (StdCan_Put(&tst_Benchmark_CpuTime_txMsg) != StdCan_Ret_OK) {}
	}

	tst_Benchmark_CpuTime_timer_val = TCNT1;
#endif
}

void tst_Benchmark_HandleMessage(StdCan_Msg_t *rxMsg)
{
/*	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_xyz && ///TODO: Change this to the actual class type
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_xyz && ///TODO: Change this to the actual module type
		rxMsg->Header.ModuleId == tst_Benchmark_ID)
	{
		switch (rxMsg->Header.Command)
		{
		case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}
*/
}

void tst_Benchmark_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_TST);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_TST_DEBUG;
	txMsg.Header.ModuleId = tst_Benchmark_ID;
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
