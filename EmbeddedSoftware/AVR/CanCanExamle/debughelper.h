#ifndef DEBUGHELPER_H
#define DEBUGHELPER_H

#include <avr/pgmspace.h>

void Debug_ByteToUart_p(const char *s, const uint8_t val);
#define Debug_ByteToUart_P(s, v) Debug_ByteToUart_p(PSTR(s), v)

#endif
