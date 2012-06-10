
#include "sns_heatPower.h"

const char sns_heatPower_request[sns_heatPower_REQ_STRING_LEN] = sns_heatPower_REQ_STRING;

typedef enum {
	PARITY_NONE = 0,
	PARITY_EVEN = 1,
	PARITY_ODD = 2,
} parityMode_t;

void sns_heatPower_Init(void)
{
	uart_setDatabits(sns_heatPower_CNF_DATABITS);
	uart_setStopbits(sns_heatPower_CNF_STOPBITS);
	uart_setParity(sns_heatPower_CNF_PARITY!=PARITY_NONE, sns_heatPower_CNF_PARITY==PARITY_ODD);
	uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(sns_heatPower_BAUD_RX, F_CPU));

	Timer_SetTimeout(sns_heatPower_REQ_TIMER, sns_heatPower_REQ_INTERVAL_S*1000 , TimerTypeFreeRunning, 0);
}

void sns_heatPower_Process(void)
{
	/* At timer timeout send a new request */
	if (Timer_Expired(sns_heatPower_REQ_TIMER))
	{
		/* Set baudrate to transmit baudrate */
		uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(sns_heatPower_BAUD_TX, F_CPU));
		
		/* Send request string */
		uart_puts(sns_heatPower_request);
		
		/* TX buffer must be empty before we change buadrate */
		while(!uart_txbufempty()) {}
		/* Set baudrate to receive baudrate */
		uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(sns_heatPower_BAUD_RX, F_CPU));
	}
	
	unsigned char status;
	do
	{
		/* ask uart driver for more data */
		unsigned int data = uart_getc();
		/* character is contained in LSB */
		unsigned char c = (unsigned char)(data & 0x00FF);

		/* status is contained in MSB */
		status = (unsigned char)((data & 0xFF00) >> 8);
		
		/* status == 0 means we just received a new char */
		if (status == 0)
		{
			//printf("RX \n"); ADD c !
			// TODO c contains data, use it! filter with config.inc parameters and send can package
			
		}
	/* keep going until uart RXBUF is empty */
	} while (status == 0);
}

void sns_heatPower_HandleMessage(StdCan_Msg_t *rxMsg)
{
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_HEATPOWER &&
		rxMsg->Header.ModuleId == sns_heatPower_ID)
	{
		switch (rxMsg->Header.Command)
		{
		//case CAN_CMD_MODULE_DUMMY:
		///TODO: Do something dummy
		break;
		}
	}
}

void sns_heatPower_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_HEATPOWER;
	txMsg.Header.ModuleId = sns_heatPower_ID;
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
