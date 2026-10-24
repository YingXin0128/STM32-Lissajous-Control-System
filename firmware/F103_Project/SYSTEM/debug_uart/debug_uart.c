#include "debug_uart.h"
#include "sys.h"

static uint8_t tx_buffer[2][DEBUG_UART_BUFFER_SIZE];
static volatile uint8_t tx_active_index;
static volatile uint8_t tx_pending_index;
static volatile uint8_t tx_busy;
static volatile uint8_t tx_pending_valid;
static volatile uint16_t tx_pending_length;
static volatile uint32_t tx_dropped_messages;

static void DebugUart_StartDma(uint8_t index, uint16_t length)
{
    DMA_Cmd(DMA1_Channel7, DISABLE);
    DMA1_Channel7->CMAR = (uint32_t)&tx_buffer[index][0];
    DMA_SetCurrDataCounter(DMA1_Channel7, length);
    DMA_ClearFlag(DMA1_FLAG_GL7);
    tx_active_index = index;
    tx_busy = 1U;
    DMA_Cmd(DMA1_Channel7, ENABLE);
}

void DebugUart_Init(void)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef uart;
    DMA_InitTypeDef dma;

    tx_active_index = 0U;
    tx_pending_index = 1U;
    tx_busy = 0U;
    tx_pending_valid = 0U;
    tx_pending_length = 0U;
    tx_dropped_messages = 0UL;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO |
                           RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_5;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &gpio);

    USART_StructInit(&uart);
    uart.USART_BaudRate = DEBUG_UART_BAUD_RATE;
    uart.USART_WordLength = USART_WordLength_8b;
    uart.USART_StopBits = USART_StopBits_1;
    uart.USART_Parity = USART_Parity_No;
    uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart.USART_Mode = USART_Mode_Tx;
    USART_Init(USART2, &uart);

    DMA_DeInit(DMA1_Channel7);
    DMA_StructInit(&dma);
    dma.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
    dma.DMA_MemoryBaseAddr = (uint32_t)&tx_buffer[0][0];
    dma.DMA_DIR = DMA_DIR_PeripheralDST;
    dma.DMA_BufferSize = DEBUG_UART_BUFFER_SIZE;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma.DMA_Mode = DMA_Mode_Normal;
    dma.DMA_Priority = DMA_Priority_Medium;
    dma.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel7, &dma);
    DMA_ITConfig(DMA1_Channel7, DMA_IT_TC | DMA_IT_TE, ENABLE);

    MY_NVIC_Init(2U, 0U, DMA1_Channel7_IRQn, NVIC_PriorityGroup_2);
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
    USART_Cmd(USART2, ENABLE);
}

uint8_t DebugUart_Send(const char *text, uint16_t length)
{
    uint8_t index;
    uint16_t offset;

    if ((text == 0) || (length == 0U))
        return 0U;
    if (length > DEBUG_UART_BUFFER_SIZE)
        length = DEBUG_UART_BUFFER_SIZE;

    __disable_irq();
    index = (uint8_t)(tx_active_index ^ 1U);
    if ((tx_busy != 0U) && (tx_pending_valid != 0U))
    {
        if (tx_dropped_messages != 0xFFFFFFFFUL)
            tx_dropped_messages++;
    }
    for (offset = 0U; offset < length; offset++)
        tx_buffer[index][offset] = (uint8_t)text[offset];

    if (tx_busy == 0U)
    {
        DebugUart_StartDma(index, length);
    }
    else
    {
        tx_pending_index = index;
        tx_pending_length = length;
        tx_pending_valid = 1U;
    }
    __enable_irq();
    return 1U;
}

uint32_t DebugUart_GetDroppedMessages(void)
{
    return tx_dropped_messages;
}

void DMA1_Channel7_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TE7) != RESET)
    {
        DMA_ClearITPendingBit(DMA1_IT_TE7);
        tx_busy = 0U;
        tx_pending_valid = 0U;
        if (tx_dropped_messages != 0xFFFFFFFFUL)
            tx_dropped_messages++;
    }
    if (DMA_GetITStatus(DMA1_IT_TC7) != RESET)
    {
        DMA_ClearITPendingBit(DMA1_IT_TC7);
        if (tx_pending_valid != 0U)
        {
            tx_pending_valid = 0U;
            DebugUart_StartDma(tx_pending_index, tx_pending_length);
        }
        else
        {
            tx_busy = 0U;
        }
    }
}
