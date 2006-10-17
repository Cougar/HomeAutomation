#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <avr/signal.h>
#include <avr/pgmspace.h>

void uartInit(unsigned char baudrate);
int uartPutChar(char c);
int uartGetChar (void);
void uartPutString(char* str);
void uartPutUInt8(uint8_t number, uint8_t base);
void uartPutUInt16(uint16_t number, uint8_t base);
void uartNewLine(void);

#endif /*UART_H_*/
