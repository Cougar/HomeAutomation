#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <bios.h>
#include <serial.h>

#define APP_TYPE    0xf001
#define APP_VERSION 0x0001

void can_receive(Can_Message_t *msg) {
	printf("candata!\n");
}

int main(void)
{
	unsigned long time;
	unsigned long time1 = bios->timebase_get();
	
	sei();
	
	Serial_Init();
	
	Can_Message_t txMsg;
	txMsg.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) | (NODE_ID << CAN_SHIFT_NMT_SID);
	txMsg.DataLength = 4;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.Data.words[0] = APP_TYPE;
	txMsg.Data.words[1] = APP_VERSION;
	
	//set up callback for candata
	bios->can_callback = &can_receive;
	// Send CAN_NMT_APP_START
	bios->can_send(&txMsg);
	
	printf("AVR Test Application\n");
	printf("Using AVR BIOS version %x\n", bios->version);
	
	txMsg.Id = (CAN_TST << CAN_SHIFT_CLASS) | NODE_ID;
	//txMsg.Data.dwords[0] = 0x01020304;
	//txMsg.Data.dwords[1] = 0xfffefdfc;
	txMsg.DataLength = 4;

	while (1) {
		time = bios->timebase_get();
		if ((time - time1) >= 2000) {
			//test-send a canmessage
			txMsg.Data.bytes[0] = time & 0x000000FF;
			txMsg.Data.bytes[1] = (time & 0x0000FF00)>>8;
			txMsg.Data.bytes[2] = (time & 0x00FF0000)>>16;
			txMsg.Data.bytes[3] = (time & 0xFF000000)>>24;
			bios->can_send(&txMsg);
			time1 = time;
			printf("Two seconds passed, time is now %ld.\n", time);
		}
	}
	
	return 0;
}
