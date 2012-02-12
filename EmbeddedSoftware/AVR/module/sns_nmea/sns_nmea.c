
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
	if (nmea_buffer[nmea_comindex[1]+1] != 0x2C)
	{
		
		//Get time in UTC
		NMEA_data.time_h = 10*a2d(nmea_buffer[6]) + a2d(nmea_buffer[7]);
		NMEA_data.time_m = 10*a2d(nmea_buffer[8]) + a2d(nmea_buffer[9]);
		NMEA_data.time_s = 10*a2d(nmea_buffer[10]) + a2d(nmea_buffer[11]);
		
		//Get fixtype
		NMEA_data.fixvalid=a2d(nmea_buffer[nmea_comindex[5]+1]);
		
		//Get number of satellites used in fix
		NMEA_data.usedsat = 10*a2d(nmea_buffer[nmea_comindex[6]+1]) + a2d(nmea_buffer[nmea_comindex[6]+2]);
		
		// Get latitude
		if (nmea_comindex[2]-nmea_comindex[1] == LAT_LENGTH) {
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
		if (nmea_comindex[4]-nmea_comindex[3] == LON_LENGTH) {
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
   /*
      Gets fixtype only, 1=No fix, 2=2D fix, 3=3D fix
   */
	NMEA_data.fixtype=a2d(nmea_buffer[nmea_comindex[1]+1]);
}

void nmea_parse_GSV(void) {
	//Information about tracked satellites, angle, snr etc. Ignore for now.
}

void nmea_parse_RMC(void) {
	//TODO
	/*	Follows comindex	length	info
		 0			6	time of fix
		 1			1	validity, A=ok, V=invalid
		 2			X	latitude
		 3			1	lat CD
		 4			X	longitude
		 5			1	lon CD
		 6			5	Speed over ground, knots, NNN.N
		 7			5	Course made good, true
		 8			6	Date of fix, DDMMYY
		 9			5	Magnetic variation, degrees
		 10			1	Magnetic variation, CD
	 
	*/
	uint8_t varlen=0;
	uint8_t validity=0;
	if (nmea_buffer[nmea_comindex[1]+1]==0x41) //0x41='A', valid fix
	 validity=1;
	NMEA_data.speed=0;
	NMEA_data.cmg_h=0;
	NMEA_data.cmg_l=0;

	if (validity==1) {
	   NMEA_data.speed=0;
	   varlen = nmea_comindex[7]-nmea_comindex[6];
	   switch(varlen-4) {
	      case 3:
		 NMEA_data.speed = 100*a2d(nmea_buffer[nmea_comindex[6]+1])+
				   10*a2d(nmea_buffer[nmea_comindex[6]+2])+
				   a2d(nmea_buffer[nmea_comindex[6]+3]);
		break;
	      case 2:
		 NMEA_data.speed = 10*a2d(nmea_buffer[nmea_comindex[6]+1])+
				   a2d(nmea_buffer[nmea_comindex[6]+2]);
		break;
	      case 1:
		 NMEA_data.speed = a2d(nmea_buffer[nmea_comindex[6]+1]);
		 break;
	   }

	   varlen = nmea_comindex[8]-nmea_comindex[7];
	   switch (varlen-4) {
	    case 3:
	       NMEA_data.cmg_h = a2d(nmea_buffer[nmea_comindex[7]+1]);
	       NMEA_data.cmg_l = 10*a2d(nmea_buffer[nmea_comindex[7]+2])+
				 a2d(nmea_buffer[nmea_comindex[7]+3]);
	       break;
	    case 2:
		NMEA_data.cmg_l = 10*a2d(nmea_buffer[nmea_comindex[7]+1])+
				  a2d(nmea_buffer[nmea_comindex[7]+2]);
	       break;
	    case 1:
	       NMEA_data.cmg_l = a2d(nmea_buffer[nmea_comindex[7]+1]);
	       break;
	   }
	      NMEA_data.date_d = 10*a2d(nmea_buffer[nmea_comindex[8]+1])+a2d(nmea_buffer[nmea_comindex[8]+2]);
	      NMEA_data.date_m = 10*a2d(nmea_buffer[nmea_comindex[8]+3])+a2d(nmea_buffer[nmea_comindex[8]+4]);
	      NMEA_data.date_y = 10*a2d(nmea_buffer[nmea_comindex[8]+5])+a2d(nmea_buffer[nmea_comindex[8]+6]);
	}
	#if CAN_PRINTF==1
		//printf("%02d-%02d-%02d",NMEA_data.date_y,NMEA_data.date_m,NMEA_data.date_d);
	#endif
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

#ifdef TSIP_AT_STARTUP
	// Tell GPS to switch to NMEA and 9600 baud
	//TODO: put in progmem to save space
	//TODO: do not send this directly at power on, the GPS module is quite slow at startup
	uint8_t tsip_command[] = {0x10, 0xBC, 0x00, 0x07, 0x07, 0x03, 0x00, 0x00, 0x00, 0x02, 0x04, 0x00, 0x10, 0x03};
	for(i = 0; i<14; i++){
		uart_putc(tsip_command[i]);
	}
#endif
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

	uint32_t HwId=BIOS_GetHwId();
	txMsg.Data[0] = HwId&0xff;
	txMsg.Data[1] = (HwId>>8)&0xff;
	txMsg.Data[2] = (HwId>>16)&0xff;
	txMsg.Data[3] = (HwId>>24)&0xff;
	
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

	txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_GPSTIME;
	txMsg.Length = 4;
	txMsg.Data[0] = NMEA_data.time_h;
	txMsg.Data[1] = NMEA_data.time_m;
	txMsg.Data[2] = NMEA_data.time_s;
	txMsg.Data[3] = 0; //Timezone diff from GMT

	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);

	txMsg.Header.Command = CAN_MODULE_CMD_GLOBAL_GPSDATE;
	txMsg.Length = 3;
	txMsg.Data[0] = NMEA_data.date_y;
	txMsg.Data[1] = NMEA_data.date_m;
	txMsg.Data[2] = NMEA_data.date_d;

	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);

	txMsg.Header.Command = CAN_MODULE_CMD_NMEA_HEADING;
	txMsg.Length = 6;
	txMsg.Data[0] = NMEA_data.fixvalid;
	txMsg.Data[1] = NMEA_data.fixtype;
	txMsg.Data[2] = NMEA_data.usedsat;
	txMsg.Data[3] = NMEA_data.speed;
	txMsg.Data[4] = NMEA_data.cmg_h;
	txMsg.Data[5] = NMEA_data.cmg_l;
	
	while (StdCan_Put(&txMsg) != StdCan_Ret_OK);
	
	#if CAN_PRINTF==1
	  //printf("Lat: %02d %02d.%02d%02d, Lon: %03d %02d.%02d%02d\n",NMEA_data.lat_deg, NMEA_data.lat_min, NMEA_data.lat_sec_h, NMEA_data.lat_sec_l >> 1,NMEA_data.lon_deg, NMEA_data.lon_min, NMEA_data.lon_sec_h, NMEA_data.lon_sec_l >> 1);
	  //printf("Time: %01d:%01d:%01d\n",NMEA_data.time_h,NMEA_data.time_m,NMEA_data.time_s);
	  //printf("Fixvalid: %01d\n",NMEA_data.fixvalid);
	  //printf("Sat: %02d\n",NMEA_data.usedsat);
	  //printf("Type:%01d\n",NMEA_data.fixtype);
	#endif

	/*
	 Add to msg:
	 Valid fix,		NMEA_data.fixvalid
	 # of satellites,	NMEA_data.numsat
	 fixtype (no/2d/3d)	NMEA_data.fixtype
	 Speed			NMEA_data.speed
	 Course made good	NMEA_data.cmg_h, NMEA_data.cmg_l
	 
	*/
}

