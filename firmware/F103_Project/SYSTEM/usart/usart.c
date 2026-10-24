#include "usart.h"

#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
};

FILE __stdout;

void _sys_exit(int x)
{
    (void)x;
}

/*
 * USART1 is reserved for the binary F407 measurement link.  Suppress legacy
 * printf traffic so text cannot corrupt the CRC-framed protocol.
 */
int fputc(int ch, FILE *f)
{
    (void)f;
    return ch;
}

static volatile uint8_t rx_dma_buffer[USART1_RX_DMA_SIZE];
static uint32_t rx_read_total;
static volatile uint32_t rx_dma_wrap_count;
static volatile uint16_t rx_idle_count;
static volatile uint32_t rx_overflow_count;

void USARTx_Init(uint32_t bound)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef uart;
    DMA_InitTypeDef dma;

    rx_read_total = 0UL;
    rx_dma_wrap_count = 0UL;
    rx_idle_count = 0U;
    rx_overflow_count = 0U;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 |
                           RCC_APB2Periph_GPIOA, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio);
    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    USART_StructInit(&uart);
    uart.USART_BaudRate = bound;
    uart.USART_WordLength = USART_WordLength_8b;
    uart.USART_StopBits = USART_StopBits_1;
    uart.USART_Parity = USART_Parity_No;
    uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart.USART_Mode = USART_Mode_Rx;
    USART_Init(USART1, &uart);

    DMA_DeInit(DMA1_Channel5);
    DMA_StructInit(&dma);
    dma.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    dma.DMA_MemoryBaseAddr = (uint32_t)&rx_dma_buffer[0];
    dma.DMA_DIR = DMA_DIR_PeripheralSRC;
    dma.DMA_BufferSize = USART1_RX_DMA_SIZE;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma.DMA_Mode = DMA_Mode_Circular;
    dma.DMA_Priority = DMA_Priority_High;
    dma.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &dma);
    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);

    MY_NVIC_Init(0U, 0U, DMA1_Channel5_IRQn, NVIC_PriorityGroup_2);
    MY_NVIC_Init(1U, 1U, USART1_IRQn, NVIC_PriorityGroup_2);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    DMA_Cmd(DMA1_Channel5, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

static uint32_t USART1_GetWriteTotal(void)
{
    uint32_t wraps_before;
    uint32_t wraps_after;
    uint16_t remaining;

    do
    {
        wraps_before = rx_dma_wrap_count;
        remaining = DMA_GetCurrDataCounter(DMA1_Channel5);
        wraps_after = rx_dma_wrap_count;
    } while (wraps_before != wraps_after);

    return wraps_before * (uint32_t)USART1_RX_DMA_SIZE +
           ((uint32_t)USART1_RX_DMA_SIZE - (uint32_t)remaining);
}

uint8_t USART1_ReadByte(uint8_t *value)
{
    uint32_t write_total;
    uint32_t available;
    uint32_t lost;
    uint16_t index;

    if (value == 0)
        return 0U;

    write_total = USART1_GetWriteTotal();
    available = write_total - rx_read_total;
    if (available > (uint32_t)USART1_RX_DMA_SIZE)
    {
        lost = available - (uint32_t)USART1_RX_DMA_SIZE;
        if ((0xFFFFFFFFUL - rx_overflow_count) < lost)
            rx_overflow_count = 0xFFFFFFFFUL;
        else
            rx_overflow_count += lost;
        rx_read_total = write_total - (uint32_t)USART1_RX_DMA_SIZE;
        available = (uint32_t)USART1_RX_DMA_SIZE;
    }
    if (available == 0UL)
        return 0U;

    index = (uint16_t)(rx_read_total % (uint32_t)USART1_RX_DMA_SIZE);
    *value = rx_dma_buffer[index];
    rx_read_total++;
    return 1U;
}

uint32_t USART1_GetOverflowCount(void)
{
    return rx_overflow_count;
}

void USART1_ClearOverflowCount(void)
{
    rx_overflow_count = 0UL;
}

void DMA1_Channel5_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC5) != RESET)
    {
        DMA_ClearITPendingBit(DMA1_IT_TC5);
        rx_dma_wrap_count++;
    }
}

void USART1_IRQHandler(void)
{
    volatile uint32_t status;
    volatile uint32_t data;

    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        status = USART1->SR;
        data = USART1->DR;
        (void)status;
        (void)data;
        rx_idle_count++;
    }
}
