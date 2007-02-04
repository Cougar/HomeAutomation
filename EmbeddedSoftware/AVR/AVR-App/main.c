#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <bios.h>

static int app_putchar(char c, FILE *stream);
static int app_getchar(FILE *stream);

static FILE mystdio = FDEV_SETUP_STREAM(app_putchar, app_getchar,
                                         _FDEV_SETUP_RW);

static int app_putchar(char c, FILE *stream) {
	bios->debug_putchar(c);
	return 0; 
}
    
static int app_getchar(FILE *stream) {
	return bios->debug_getchar();
}

void can_receive(Can_Message_t *msg) {
	printf("candata!\n");
}

int main(void)
{
	unsigned long time;
	unsigned long time1 = bios->timebase_get();
	unsigned long time2 = bios->timebase_get();
	
	sei();
	
	stdout = &mystdio;
	stdin = &mystdio;

	Can_Message_t txMsg;
	txMsg.Id = 100;
	txMsg.DataLength = 0;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	
	//set up callback for candata
	bios->can_callback = &can_receive;
	
	printf("AVR Test Application\n");
	printf("Using AVR BIOS version %x\n", bios->version);
	
	while (1) {
		time = bios->timebase_get();
		if ((time - time1) >= 2000) {
			//test-send a canmessage
			bios->can_send(&txMsg);
			time1 = time;
			printf("Two seconds passed, time is now %d.\n", (int)time);
		}

		if ((time - time2) >= 20000) {
			time2 = time;
			printf("Will now hang node.\n");
			cli();
		}
	}
	
	return 0;
}
