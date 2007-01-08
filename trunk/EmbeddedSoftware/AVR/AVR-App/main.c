#include <inttypes.h>
#include <avr/interrupt.h>
#include <bios.h>

static int app_putchar(char c, FILE *stream);
static int app_getchar(FILE *stream);

static FILE mystdio = FDEV_SETUP_STREAM(app_putchar, app_getchar,
                                         _FDEV_SETUP_RW);

static int app_putchar(char c, FILE *stream) {
	bios->can_send(c);
	return 0; 
}
    
static int app_getchar(FILE *stream) {
	return bios->can_receive();
}

int main(void)
{
	long time;
	long time1 = 0;
	long time2 = 0;
	
	sei();
	
	stdout = &mystdio;
	stdin = &mystdio;
	
	printf("AVR Test Application\n");
	printf("Using AVR BIOS version %x\n", bios->version);
	
	while (1) {
		time = bios->timebase_get();
		if ((time - time1) >= 2000) {
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
