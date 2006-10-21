#include <ctype.h>

#include "termio.h"
#include "debughelper.h"

void Debug_ByteToUart_p(const char *s, const uint8_t val)
{
	term_puts_p(s);
	term_puts_P(" = 0x");
	term_puthex_byte(val);
	term_puts_P(" = ");
	term_putbin_byte(val);
	if ( isprint(val) ) {
		term_puts_P(" (");
		term_putc(val);
		term_puts_P(")");
	}
	term_puts_P("\n");
}
