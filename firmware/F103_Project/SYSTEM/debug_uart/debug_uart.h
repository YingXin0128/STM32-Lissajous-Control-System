#ifndef __DEBUG_UART_H
#define __DEBUG_UART_H

#include "stm32f10x.h"

#define DEBUG_UART_BAUD_RATE      115200UL
#define DEBUG_UART_BUFFER_SIZE    768U

void DebugUart_Init(void);
uint8_t DebugUart_Send(const char *text, uint16_t length);
uint32_t DebugUart_GetDroppedMessages(void);

#endif
