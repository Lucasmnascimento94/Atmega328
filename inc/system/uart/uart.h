#ifndef UART_H
#define UART_H
#include "types.h"

uint8_t uartInit();
void uartWrite(char *c, uint16_t size);
void uartWrite_(char *c);

#endif