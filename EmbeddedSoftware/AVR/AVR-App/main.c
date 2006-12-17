#include <inttypes.h>
#include <bios.h>

static int app_putchar(char c, FILE *stream);
static int app_getchar(FILE *stream);

static FILE mystdio = FDEV_SETUP_STREAM(app_putchar, app_getchar,
                                         _FDEV_SETUP_RW);

static int app_putchar(char c, FILE *stream) {
	return bios->put_char(c, stream);
}
    
static int app_getchar(FILE *stream) {
	return bios->get_char(stream);
}

int main(void)
{
	unsigned ver;
	ver = bios->version();
	
	stdout = &mystdio;
	stdin = &mystdio;
	
	printf("AVR Test Application\n");
	printf("Using AVR BIOS version %x\n", ver);
	
	return ver;
}
