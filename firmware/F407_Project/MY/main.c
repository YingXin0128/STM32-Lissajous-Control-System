/*
 * 采集端阅读入口：同步采样 → 频率/相位计算 → UART 测量帧。
 * 采样与发送的 DMA 中断只负责搬运/就绪通知；分析在主循环执行。
 */
#include "stm32f4xx.h"
#include "led.h"
#include "measurement.h"
#include "protocol.h"

int main(void)
{
    Measurement_Result result;
    uint8_t frame_index;

    LED_Init();
    Protocol_Init();
    Measurement_Init();
    LED1_OFF;
    while (1)
    {
        if (Measurement_TakeReadyFrame(&frame_index) != 0U)
        {
            /* 处理已复制的 8000 对采样，结果包含频率、相位与 ADC 峰峰计数。 */
            Measurement_Analyze(frame_index, &result);
            /* 把测量结果交给 protocol.c 打包，通过 USART1 TX DMA 发给 F103。 */
            Protocol_SendMeasurement(&result);
            LED1_Toggle;
        }
    }
}
