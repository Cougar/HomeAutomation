#ifndef UART_H
#define UART_H

#include "../Include/compiler.h"
#include "../Include/typedefs.h"

void uartInit(void);
void uartPutc(BYTE c);
void uartPuts(BYTE c[]);
void uartPutrs(const rom char *c);
BYTE uartGet(void);
void uartParse(BYTE c);
void uartISR(void);

#endif //UART_H
