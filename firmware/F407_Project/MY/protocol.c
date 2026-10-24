/*
 * MCU 间发送端：把 Measurement_Result 序列化为 48 字节测量帧。
 * 对应接收端：F103_Project/HARDWARE/CAPTURE/signal_capture.c。
 * 完整字节偏移见 docs/communication_protocol.md；多字节字段小端，CRC 覆盖字节 2..45。
 */
#include "protocol.h"
#include "app_config.h"

#define PROTOCOL_TX_BUFFER_COUNT 2U

static uint8_t tx_frame[PROTOCOL_TX_BUFFER_COUNT]
                       [APP_PROTOCOL_FRAME_SIZE];
static volatile uint8_t tx_active_index;
static volatile uint8_t tx_pending_index;
static volatile uint8_t tx_busy;
static volatile uint8_t tx_pending_valid;

static void Protocol_PutU16(uint8_t *destination, uint16_t value)
{
    destination[0] = (uint8_t)value;
    destination[1] = (uint8_t)(value >> 8);
}

static void Protocol_PutU32(uint8_t *destination, uint32_t value)
{
    destination[0] = (uint8_t)value;
    destination[1] = (uint8_t)(value >> 8);
    destination[2] = (uint8_t)(value >> 16);
    destination[3] = (uint8_t)(value >> 24);
}

static uint16_t Protocol_Crc16(const uint8_t *data, uint16_t length)
{
    uint16_t crc;
    uint16_t index;
    uint8_t bit;

    crc = 0xFFFFU;
    for (index = 0U; index < length; index++)
    {
        crc ^= (uint16_t)data[index] << 8;
        for (bit = 0U; bit < 8U; bit++)
        {
            if ((crc & 0x8000U) != 0U)
                crc = (uint16_t)((crc << 1) ^ 0x1021U);
            else
                crc <<= 1;
        }
    }
    return crc;
}

static void Protocol_StartDma(uint8_t index)
{
    DMA_Cmd(DMA2_Stream7, DISABLE);
    DMA2_Stream7->M0AR = (uint32_t)&tx_frame[index][0];
    DMA_SetCurrDataCounter(DMA2_Stream7, APP_PROTOCOL_FRAME_SIZE);
    DMA_ClearFlag(DMA2_Stream7,
                  DMA_FLAG_FEIF7 | DMA_FLAG_DMEIF7 |
                  DMA_FLAG_TEIF7 | DMA_FLAG_HTIF7 |
                  DMA_FLAG_TCIF7);
    tx_active_index = index;
    tx_busy = 1U;
    DMA_Cmd(DMA2_Stream7, ENABLE);
}

/* 帧布局的唯一发送定义：同步字、版本/类型/长度、测量字段、CRC。 */
static void Protocol_BuildFrame(uint8_t *frame,
                                const Measurement_Result *result)
{
    uint16_t crc;

    frame[0] = APP_PROTOCOL_SYNC0;
    frame[1] = APP_PROTOCOL_SYNC1;
    frame[2] = APP_PROTOCOL_VERSION;
    frame[3] = APP_PROTOCOL_TYPE_MEASUREMENT;
    frame[4] = APP_PROTOCOL_PAYLOAD_SIZE;
    Protocol_PutU16(&frame[5], result->frame_sequence);
    Protocol_PutU32(&frame[7], result->sample_timestamp_us);
    Protocol_PutU32(&frame[11],
                    result->reference_raw_frequency_millihz);
    Protocol_PutU32(&frame[15],
                    result->reference_frequency_millihz);
    Protocol_PutU32(&frame[19],
                    (uint32_t)result->relative_phase_millidegrees);
    Protocol_PutU32(&frame[23],
                    result->feedback_frequency_millihz);
    Protocol_PutU16(&frame[27], result->reference_vpp);
    Protocol_PutU16(&frame[29], result->feedback_vpp);
    frame[31] = result->flags;
    Protocol_PutU16(&frame[32], result->dropped_frames);
    Protocol_PutU16(&frame[34], result->reference_min);
    Protocol_PutU16(&frame[36], result->reference_max);
    Protocol_PutU16(&frame[38], result->feedback_min);
    Protocol_PutU16(&frame[40], result->feedback_max);
    Protocol_PutU16(&frame[42], result->analysis_time_us);
    Protocol_PutU16(&frame[44], result->firmware_tag);
    crc = Protocol_Crc16(&frame[2], 44U);
    Protocol_PutU16(&frame[46], crc);
}

void Protocol_Init(void)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef uart;
    DMA_InitTypeDef dma;
    NVIC_InitTypeDef nvic;

    tx_active_index = 0U;
    tx_pending_index = 1U;
    tx_busy = 0U;
    tx_pending_valid = 0U;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |
                           RCC_AHB1Periph_DMA2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    USART_StructInit(&uart);
    uart.USART_BaudRate = APP_UART_BAUD;
    uart.USART_WordLength = USART_WordLength_8b;
    uart.USART_StopBits = USART_StopBits_1;
    uart.USART_Parity = USART_Parity_No;
    uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart.USART_Mode = USART_Mode_Tx;
    USART_Init(USART1, &uart);

    DMA_DeInit(DMA2_Stream7);
    DMA_StructInit(&dma);
    dma.DMA_Channel = DMA_Channel_4;
    dma.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    dma.DMA_Memory0BaseAddr = (uint32_t)&tx_frame[0][0];
    dma.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    dma.DMA_BufferSize = APP_PROTOCOL_FRAME_SIZE;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma.DMA_Mode = DMA_Mode_Normal;
    dma.DMA_Priority = DMA_Priority_High;
    dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_Init(DMA2_Stream7, &dma);
    DMA_ITConfig(DMA2_Stream7, DMA_IT_TC | DMA_IT_TE, ENABLE);

    nvic.NVIC_IRQChannel = DMA2_Stream7_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2U;
    nvic.NVIC_IRQChannelSubPriority = 0U;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

/* 发送忙时只保留最新 pending 帧，后来的结果可能覆盖尚未发送的旧结果。 */
void Protocol_SendMeasurement(const Measurement_Result *result)
{
    uint8_t index;

    if (result == 0)
        return;

    __disable_irq();
    if (tx_busy == 0U)
    {
        index = (uint8_t)(tx_active_index ^ 1U);
        Protocol_BuildFrame(&tx_frame[index][0], result);
        Protocol_StartDma(index);
    }
    else
    {
        index = (uint8_t)(tx_active_index ^ 1U);
        Protocol_BuildFrame(&tx_frame[index][0], result);
        tx_pending_index = index;
        tx_pending_valid = 1U;
    }
    __enable_irq();
}

void Protocol_DmaIrqHandler(void)
{
    if (DMA_GetITStatus(DMA2_Stream7, DMA_IT_TEIF7) != RESET)
    {
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TEIF7);
        tx_busy = 0U;
        tx_pending_valid = 0U;
    }
    if (DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7) != RESET)
    {
        DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TCIF7);
        if (tx_pending_valid != 0U)
        {
            tx_pending_valid = 0U;
            Protocol_StartDma(tx_pending_index);
        }
        else
        {
            tx_busy = 0U;
        }
    }
}
