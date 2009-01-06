
#include "sns_nmea.h"
#include <drivers/uart/serial.h>
#include <drivers/uart/uart.h>

void nmea_send_can(uint8_t timer);

uint8_t nmea_buffer[80];
uint8_t nmea_comindex[25];
uint8_t nmea_buffer_index, nmea_buflen, nmea_comlen;

uint8_t a2d (uint8_t chr) { // Convert ascii to decimal
	return (chr-48);
}

void nmea_clear_comindex(void) {
	uint8_t i;
	
	for (i=0;i<25;i++) {
		nmea_comindex[i]=0;
	}
}

void nmea_parse_GGA(void) {
	if (nmea_buffer[nmea_comindex[1]+1] != ",")
	{
		uint8_t i;
		
		//Get time in UTC
		NMEA_data.time_h = 10*a2d(nmea_buffer[6]) + a2d(nmea_buffer[7]);
		NMEA_data.time_m = 10*a2d(nmea_buffer[8]) + a2d(nmea_buffer[9]);
		NMEA_data.time_s = 10*a2d(nmea_buffer[10]) + a2d(nmea_buffer[11]);
		
		//Get fixtype
		NMEA_data.fixvalid=a2d(nmea_buffer[nmea_comindex[5]+1]);
		
		//Get number of satellites used in fix
		NMEA_data.usedsat = 10*a2d(nmea_buffer[nmea_comindex[6]+1]) + a2d(nmea_buffer[nmea_comindex[6]+2]);
		
		// Get latitude
		if (nmea_comindex[2]-nmea_comindex[1] == 10) {
			NMEA_data.lat_deg = 10*a2d(nmea_buffer[nmea_comindex[1]+1])
			+ a2d(nmea_buffer[nmea_comindex[1]+2]);
			
			NMEA_data.lat_min = 10*a2d(nmea_buffer[nmea_comindex[1]+3])
			+ a2d(nmea_buffer[nmea_comindex[1]+4]);

			NMEA_data.lat_sec_h = 10*a2d(nmea_buffer[nmea_comindex[1]+6]);
			NMEA_data.lat_sec_h += a2d(nmea_buffer[nmea_comindex[1]+7]);
			NMEA_data.lat_sec_l = 10*a2d(nmea_buffer[nmea_comindex[1]+8]);
			NMEA_data.lat_sec_l += a2d(nmea_buffer[nmea_comindex[1]+9]);
			NMEA_data.lat_sec_l = NMEA_data.lat_sec_l << 1;
			
			//Last bit contains cardinal direction, N=0, S=1
			if (nmea_buffer[nmea_comindex[2]+1] == 'N') {
				NMEA_data.lat_sec_l &=0b11111110;
			} else {
				NMEA_data.lat_sec_l |=1;
			}
		}
		
		
		// Get longitude
		if (nmea_comindex[4]-nmea_comindex[3] == 11) {
			NMEA_data.lon_deg = 100*a2d(nmea_buffer[nmea_comindex[3]+1])
			+ 10*a2d(nmea_buffer[nmea_comindex[3]+2])
			+ a2d(nmea_buffer[nmea_comindex[3]+3]);
			
			NMEA_data.lon_min = 10*a2d(nmea_buffer[nmea_comindex[3]+4])
			+ a2d(nmea_buffer[nmea_comindex[3]+5]);
			
			NMEA_data.lon_sec_h = 10*a2d(nmea_buffer[nmea_comindex[3]+7]);
			NMEA_data.lon_sec_h += a2d(nmea_buffer[nmea_comindex[3]+8]);
			NMEA_data.lon_sec_l = 10*a2d(nmea_buffer[nmea_comindex[3]+9]);
			NMEA_data.lon_sec_l += a2d(nmea_buffer[nmea_comindex[3]+10]);
			NMEA_data.lon_sec_l = NMEA_data.lon_sec_l << 1;
			
			//Last bit contains cardinal direction, E=0, W=1
			if (nmea_buffer[nmea_comindex[4]+1] == 'E') {
				NMEA_data.lon_sec_l &=0b11111110;
			} else {
				NMEA_data.lon_sec_l |=1;
			}
		}
	}
}

void nmea_parse_GSA(void) {
	//TODO
}

void nmea_parse_GSV(void) {
	//TODO
}

void nmea_parse_RMC(void) {
	//TODO
}

void nmea_make_index(void) {

	nmea_clear_comindex();
	
	uint8_t i;
	uint8_t j=0;
	
	for (i=0;i<nmea_buflen;i++) {
		if (nmea_buffer[i] == ',') {
			nmea_comindex[j] = i;
			j++;
		}
	}
	nmea_comlen = j;
}

void nmea_parse(void) {
	// Supported messages: GSA, GSV, GGA, RMC
	nmea_make_index();
		
	if (
		nmea_buffer[2] == 'G' &&
		nmea_buffer[3] == 'S' &&
		nmea_buffer[4] == 'A')
	{
		nmea_parse_GSA();
	} else if (
		nmea_buffer[2] == 'G' &&
		nmea_buffer[3] == 'S' &&
		nmea_buffer[4] == 'V') {
		nmea_parse_GSV();
	} else if (
		nmea_buffer[2] == 'G' &&
		nmea_buffer[3] == 'G' &&
		nmea_buffer[4] == 'A') {
		nmea_parse_GGA();
	} else if (
		nmea_buffer[2] == 'R' &&
		nmea_buffer[3] == 'M' &&
		nmea_buffer[4] == 'C') {
		nmea_parse_RMC();
	}		
	
}

void sns_nmea_Init(void)
{
	Serial_Init();	
	Timer_SetTimeout(sns_nmea_cansend_TIMER, sns_nmea_cansend_SEND_PERIOD , TimerTypeFreeRunning, &nmea_send_can);
}

void sns_nmea_Process(void)
{
	uint16_t rx_uart;
	uint8_t rx_char;
	
	rx_uart = uart_getc();
	
	if (rx_uart != UART_NO_DATA) {
		rx_char = (uint8_t) rx_uart & 0x00ff;
		
		if (rx_char == 0x24) {
			nmea_buffer_index=0;
		} else if (rx_char == '\n') {
			nmea_buflen = nmea_buffer_index+1;
			nmea_parse();
		} else {			
			nmea_buffer[nmea_buffer_index] = rx_char;
			nmea_buffer_index++;
		}
	}
}

void sns_nmea_HandleMessage(StdCan_Msg_t *rxMsg)
{
/*
	if (	StdCan_Ret_class(rxMsg->Header) == CAN_MODULE_CLASS_SNS &&
		StdCan_Ret_direction(rxMsg->Header) == DIRECTIONFLAG_FROM_OWNER &&
		rxMsg->Header.ModuleType == CAN_MODULE_TYPE_SNS_NMEA &&
		rxMsg->Header.ModuleId == sns_nmea_ID)
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

void sns_nmea_List(uint8_t ModuleSequenceNumber)
{
	StdCan_Msg_t txMsg;
	
	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS); ///TODO: Change this to the actual class type
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_NMEA; ///TODO: Change this to the actual module type
	txMsg.Header.ModuleId = sns_nmea_ID;
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

void nmea_send_can(uint8_t timer)
{
	StdCan_Msg_t txMsg;

	StdCan_Set_class(txMsg.Header, CAN_MODULE_CLASS_SNS);
	StdCan_Set_direction(txMsg.Header, DIRECTIONFLAG_FROM_OWNER);
	txMsg.Header.ModuleType = CAN_MODULE_TYPE_SNS_NMEA;
	txMsg.Header.ModuleId = sns_nmea_ID;
	txMsg.Header.Command = CAN_MODULE_CMD_NMEA_POSITION; //definiera nåt vettigt för det sen
	
	txMsg.Length = 8;
	txMsg.Data[0] = NMEA_data.lat_deg;
	txMsg.Data[1] = NMEA_data.lat_min;
	txMsg.Data[2] = NMEA_data.lat_sec_h;	
	txMsg.Data[3] = NMEA_data.lat_sec_l;

	txMsg.Data[4] = NMEA_data.lon_deg;
	txMsg.Data[5] = NMEA_data.lon_min;
	txMsg.Data[6] = NMEA_data.lon_sec_h;	
	txMsg.Data[7] = NMEA_data.lon_sec_l;
	
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
	
	printf("Lat: %02d %02d.%02d%02d, Lon: %03d %02d.%02d%02d\n",NMEA_data.lat_deg, NMEA_data.lat_min, NMEA_data.lat_sec_h, NMEA_data.lat_sec_l >> 1,NMEA_data.lon_deg, NMEA_data.lon_min, NMEA_data.lon_sec_h, NMEA_data.lon_sec_l >> 1);
}

