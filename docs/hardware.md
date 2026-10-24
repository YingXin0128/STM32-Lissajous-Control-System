# 源码确认的硬件配置

本表记录软件 GPIO/外设配置，不是完整原理图或已经验证的接线图。板修订、完整模块型号、连接器方向、模拟调理/放大/衰减链路：TODO：待根据实际工程确认。

| 项目 | 配置证据 |
|---|---|
| 控制 MCU | Keil Device STM32F103ZE；512 KiB Flash / 64 KiB SRAM；`STM32F10X_HD`；高密度启动文件 |
| 采集 MCU | Keil Device STM32F407ZGTx；1 MiB Flash / 128 KiB SRAM + 64 KiB CCM 配置；`STM32F40_41xxx` |
| 库与时钟 | F103 标准库 V3.5.0，系统文件配置 72 MHz；F407 标准库 V1.8.0，系统文件 PLL_M=8、PLL_N=336、PLL_P=2，对应 8 MHz HSE 时 168 MHz。实际晶振待核实，Keil Cpu 字符串中的 12 MHz 不能替代系统代码 |
| DDS | AD9910；`AD9910.H`、`AD9910V1.C`；应用频率字按 1 GHz 系统时钟计算 |
| 本地显示 | F103 SPI2 LCD，保留供应商驱动；屏模块具体批次/接线以实物核实，不能只凭通用驱动推断 |

## 引脚与资源

| 端 | 信号 / 资源 | 源码 |
|---|---|---|
| F407 | PC0 ADC1 channel10 参考；PC1 ADC2 channel11 反馈 | `MY/measurement.c` |
| F407 | TIM2 TRGO；DMA2 Stream0 channel0，32 位双 ADC 打包 | 同上 |
| F407 | USART1 PA9 TX、PA10 复用配置但仅 TX 模式；DMA2 Stream7 channel4 | `MY/protocol.c` |
| F103 | USART1 PA9/PA10，测量接收 DMA1 channel5 | `SYSTEM/usart/usart.c` |
| F103 | USART2 PD5 TX，115200，诊断 CSV | `SYSTEM/debug_uart/debug_uart.c` |
| F103 | TIM6 毫秒时基 | `USER/main.c` |
| F103 | PE12 确认、PE13 上、PE14 下、PE15 返回 | `HARDWARE/BOARD/board.c` |
| F103 | PC13 LED/标记输出；PD2 旁路继电器控制 | 同上；菜单有调用 |
| F103 | DDS SDIO PA5；SCLK PA2；CS PB10；IO_UPDATE PC3；RESET PA6；PLL_LOCK PC0 | `HARDWARE/ad9910/AD9910.H` |
| F103 | DDS PWR PB11；PROFILE0 PC4、1 PC10、2 PC5；DRHOLD PC1、DROVER PC2、DRCTL PA4、OSK PC8 | 同上 |

两个 MCU 的 PC0/PC1 分别属于不同芯片，不构成同板冲突。完整 LCD 引脚请直接核对 `HARDWARE/LCD/lcd.h` 与 `lcd.c`，不要把 DDS 的 PA2/PA5 当作 LCD SPI2 引脚。PA13/PA14 保留 SWD 调试。

模拟输入必须经过合适的偏置、分压/调理，不能把负电压或超出 ADC 电源轨的波形直接输入。PD2 是逻辑控制，不能直接驱动继电器线圈。调试器与外部供电应核对电源关系并共地。本次未连接硬件、未烧录、未改选项字节。

## 幅值事实

`USER/app_config.h` 的历史“4V/3V/2V/1V”标签对应 `NOMINAL_MV_X10` 6660/5000/3330/1665（0.1 mV 单位），即约 0.666/0.500/0.333/0.1665 Vpp。该注释称来自先前示波器标定，但本次没有独立复测。需用户补充模块端、ADC 测点和最终后级端的对应关系、负载和完整电路后，才能给出输出电压验收指标。
