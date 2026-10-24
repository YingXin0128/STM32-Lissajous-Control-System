#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "sys.h"

#define USART1_RX_DMA_SIZE 4096U

void USARTx_Init(uint32_t bound);
uint8_t USART1_ReadByte(uint8_t *value);
uint32_t USART1_GetOverflowCount(void);
void USART1_ClearOverflowCount(void);

#endif
