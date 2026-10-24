#include "stm32f4xx_it.h"
#include "measurement.h"
#include "protocol.h"

void NMI_Handler(void) {}
void HardFault_Handler(void) { while (1) {} }
void MemManage_Handler(void) { while (1) {} }
void BusFault_Handler(void) { while (1) {} }
void UsageFault_Handler(void) { while (1) {} }
void SVC_Handler(void) {}
void DebugMon_Handler(void) {}
void PendSV_Handler(void) {}
void SysTick_Handler(void) {}

void DMA2_Stream0_IRQHandler(void)
{
    Measurement_DmaIrqHandler();
}

void DMA2_Stream7_IRQHandler(void)
{
    Protocol_DmaIrqHandler();
}
