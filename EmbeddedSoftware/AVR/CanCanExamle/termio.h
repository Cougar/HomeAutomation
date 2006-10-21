#ifndef termio_h_
#define termio_h_

#include <avr/pgmspace.h>

/* 
** high byte error return code of term_getc() - from P. Fleury's lib.
*/
#define TERM_FRAME_ERROR      0x0800              /* Framing Error by UART       */
#define TERM_OVERRUN_ERROR    0x0400              /* Overrun condition by UART   */
#define TERM_BUFFER_OVERFLOW  0x0200              /* receive ringbuffer overflow */
#define TERM_NO_DATA          0x0100              /* no receive data available   */

uint16_t term_getc(void);

void term_putc(const char c);
void term_puts(const char *tx_data);
void term_puts_p(const char *progmem_tx_data);
void term_puti(const int num);
void term_put_cr(void);
#define term_puts_P(s) term_puts_p(PSTR(s))
void term_puthex_byte(const uint8_t val);
void term_putbin_byte(const uint8_t val);


extern unsigned char term_timeout_flag;

#endif
