/*********************************************************************
 *
 *                  uart header file
 *
 *********************************************************************
 * FileName:        uart.h
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Johan B�hlin     21-10-06 	Original        (Rev 1.0)
 ********************************************************************/

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
char uartDataRedy(void);

#endif //UART_H
