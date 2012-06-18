#define UART_RX_BUFFER_SIZE 128

#include "sns_heatPower.h"

uint32_t sns_heatPower_energy;
uint16_t sns_heatPower_power;
uint8_t sns_heatPower_values_ok;

const char sns_heatPower_request[sns_heatPower_REQ_STRING_LEN] = sns_heatPower_REQ_STRING;

/* TODO: add filtering of power (power32) */

typedef enum {
	PARITY_NONE = 0,
	PARITY_EVEN = 1,
	PARITY_ODD = 2,
} parityMode_t;

uint8_t RxdCnt;
uint8_t RxdEnable;

uint8_t a2d (unsigned char chr) { // Convert ascii to decimal
	return (chr-48);
}

void sns_heatPower_Init(void)
{
	/* Set up UART */
	uart_setDatabits(sns_heatPower_CNF_DATABITS);
	uart_setStopbits(sns_heatPower_CNF_STOPBITS);
	uart_setParity(sns_heatPower_CNF_PARITY!=PARITY_NONE, sns_heatPower_CNF_PARITY==PARITY_ODD);
	uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(sns_heatPower_BAUD_RX, F_CPU));

	/* Start request data timer */
	Timer_SetTimeout(sns_heatPower_REQ_TIMER, sns_heatPower_REQ_INTERVAL_S*1000 , TimerTypeFreeRunning, 0);

	RxdCnt = 0;
	RxdEnable = 0;
	sns_heatPower_values_ok = 0;
}

void sns_heatPower_Process(void)
{
	/* At timer timeout send a new request */
	if (Timer_Expired(sns_heatPower_REQ_TIMER))
	{
#if (CAN_PRINTF == 1)
		printf("TX\n");
#endif
		/* Set baudrate to transmit baudrate */
		uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(sns_heatPower_BAUD_TX, F_CPU));
		
		/* Send request string */
		uart_puts(sns_heatPower_request);
		
		/* TX buffer must be empty before we change buadrate */
		//while(uart_txbufempty()) {;}
		
		Timer_SetTimeout(sns_heatPower_REQ_TIMER2, 300 , TimerTypeOneShot, 0);		
	}

	/* At timer timeout send a new request */
	if (Timer_Expired(sns_heatPower_REQ_TIMER2))
	{
		/* Set baudrate to receive baudrate */
		uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(sns_heatPower_BAUD_RX, F_CPU));
		RxdCnt = 0;
		RxdEnable = 1;
		sns_heatPower_energy = 0;
		sns_heatPower_power = 0;
		sns_heatPower_values_ok = 0;
#if (CAN_PRINTF == 1)
		printf("RX\n");
#endif
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
		if (status == 0 && RxdEnable)
		{
			//printf("%c", c&0x7F);
			/* c contains an ascii value to be processed */
			/* get the decimal representation of ascii value */
			uint8_t decimalval = a2d(c);

			if (RxdCnt >= sns_heatPower_RES_ENERGY_START && RxdCnt < (sns_heatPower_RES_ENERGY_START + sns_heatPower_RES_ENERGY_LEN))
			{
				/* Check that recied char was between 0-9 */
				if (decimalval > 9)
				{
					/* ERROR!! */
					RxdEnable = 0;
#if (CAN_PRINTF == 1)
					printf("E0 %u\n", c&0x7F);
#endif
				}
				if (decimalval > 0)
				{
					uint32_t multiplier = 1;
					/* multiply received char with 1000000, 100000, 10000, 1000 etc */
					for (uint8_t i = 0; i < sns_heatPower_RES_ENERGY_LEN-(RxdCnt-sns_heatPower_RES_ENERGY_START)-1; i++)
					{
						multiplier = multiplier*10;
					}
					sns_heatPower_energy += decimalval*multiplier;
				}
			}
			/* When all data in energy are fetched */
			if (RxdCnt == (sns_heatPower_RES_ENERGY_START + sns_heatPower_RES_ENERGY_LEN))
			{
				/* Recieved data is in kWh, data to be transmitted on CAN must be in Wh */
				sns_heatPower_energy = sns_heatPower_energy*1000;
#if (CAN_PRINTF == 1)
				printf("%lu", sns_heatPower_energy);
#endif
			}

			if (RxdCnt >= sns_heatPower_RES_POWER_START && RxdCnt < (sns_heatPower_RES_POWER_START + sns_heatPower_RES_POWER_LEN))
			{
				/* Check that recied char was between 0-9 */
				if (decimalval > 9)
				{
					/* ERROR!! */
					RxdEnable = 0;
#if (CAN_PRINTF == 1)
					printf("E1 %u\n", c&0x7F);
#endif
				}
				if (decimalval > 0)
				{
					uint32_t multiplier = 1;
					/* multiply received char with 1000000, 100000, 10000, 1000 etc */
					for (uint8_t i = 0; i < sns_heatPower_RES_POWER_LEN-(RxdCnt-sns_heatPower_RES_POWER_START)-1; i++)
					{
						multiplier = multiplier*10;
					}

					sns_heatPower_power += decimalval*multiplier;
				}
			}
			if (RxdCnt == (sns_heatPower_RES_POWER_START + sns_heatPower_RES_POWER_LEN))
			{
				/* Recieved data is in 100Wh, data to be transmitted on CAN must be in Wh */
				sns_heatPower_power = sns_heatPower_power*100;
#if (CAN_PRINTF == 1)
				printf("P%u\n", sns_heatPower_power);
#endif
			}

#if (sns_heatPower_RES_POWER_START > sns_heatPower_RES_ENERGY_START)
			if (RxdCnt == (sns_heatPower_RES_POWER_START + sns_heatPower_RES_POWER_LEN))
#else
			if (RxdCnt == (sns_heatPower_RES_ENERGY_START + sns_heatPower_RES_ENERGY_LEN))
#endif
			{
				sns_heatPower_values_ok = 1;
			}

			RxdCnt++;
		}
		else if (status == 0x10 && RxdEnable)
		{
			/* Frame error */
#if (CAN_PRINTF == 1)
			printf("E2%x\n", c);
#endif
			RxdEnable = 0;
		}
		else if (status == 0x08 && RxdEnable)
		{
			/* Overrun error */
#if (CAN_PRINTF == 1)
			printf("E3%x\n", c);
#endif
			RxdEnable = 0;
		}
		else if (status == 0x02 && RxdEnable)
		{
			/* Buffer overflow error */
#if (CAN_PRINTF == 1)
			printf("E4%x\n", c);
#endif
			RxdEnable = 0;
		}
		else if (status != 0 && data != UART_NO_DATA && RxdEnable)
		{
#if (CAN_PRINTF == 1)
			printf("E5%x%x\n", status, c);
#endif
			RxdEnable = 0;
		}
	/* keep going until uart RXBUF is empty */
	} while (status == 0);
	
	if (sns_heatPower_values_ok)
	{
		StdCan_Msg_t txMsg;
		StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
		StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
		txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_HEATPOWER;
		txMsg.Header.ModuleId = sns_heatPower_ID;
		txMsg.Header.Command = CAN_MODULE_CMD_PHYSICAL_ELECTRICPOWER;
		txMsg.Length = 8;
		txMsg.Data[0] = (uint8_t)((sns_heatPower_power>>8) & 0xff);
		txMsg.Data[1] = (uint8_t)(sns_heatPower_power & 0xff);
		txMsg.Data[5] = (uint8_t)sns_heatPower_energy & 0xff;
		txMsg.Data[4] = (uint8_t)(sns_heatPower_energy >> 8) & 0xff;
		txMsg.Data[3] = (uint8_t)(sns_heatPower_energy >> 16) & 0xff;
		txMsg.Data[2] = (uint8_t)(sns_heatPower_energy >> 24) & 0xff;
		txMsg.Data[6] = (uint8_t)((sns_heatPower_power>>8) & 0xff);
		txMsg.Data[7] = (uint8_t)(sns_heatPower_power & 0xff);
		while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
		sns_heatPower_values_ok = 0;
	}
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
