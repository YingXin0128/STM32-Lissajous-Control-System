# 架构、工程与调用关系

基线为 20260801 交接包 `Project/`，保留 v2.2.6 两套 Keil 内部结构。没有从 v2.2.0、v2.2.1 或斜坡发生器替换文件。

## F407

`startup_stm32f40_41xxx.s` → `SystemInit` → `MY/main.c`。主入口依次初始化 LED、协议、采样；轮询 `Measurement_TakeReadyFrame`，就绪后 `Measurement_Analyze` → `Protocol_SendMeasurement`。

`measurement.c` 配置 PC0/ADC1、PC1/ADC2、TIM2 TRGO、DMA2 Stream0。16000 个 32 位双通道打包采样在 800 kSa/s 下为 20 ms；DMA 完成中断标记就绪，主循环复制后半段 8000 对到分析缓冲。`frame_index` 参数目前被忽略，使用复制后的 `analysis_buffer`。有掉帧/溢出诊断，不应将本实现描述成每个半缓冲都分析的流水线。时间戳根据 DMA 周期累计，标记分析窗口中心。

Protocol 用 USART1 TX / DMA2 Stream7，双发送缓冲；忙时保留一帧 pending，新测量可覆盖旧 pending，属于最新数据优先，不保证逐帧无损。

## F103

`startup_stm32f10x_hd.s` → `SystemInit` → `USER/main.c`。先初始化板级、诊断 UART、按键、手动控制、自动占位、菜单及 AD9910；之后才初始化测量接收和 TIM6 毫秒时基，避免阻塞初始化造成 RX 环形缓冲溢出。

主循环 `SignalCapture_Service` 排空 USART1 DMA 接收字节，由 `Link_ProcessByte` 校验帧，再入快照队列。`SignalCapture_TakeSnapshot` 逐帧驱动手动控制；另有 10 ms 过期检查、20 ms 显示片段及 100 ms CSV 诊断调度。具体条件与优先级以 `USER/main.c` 为准。控制积分使用测量时间戳，不应把显示刷新周期当作 PI 的固定采样周期。

`ManualControl_Update` → 捕获/细调/相位环状态 → `Control_WriteToneFtw` → `AD9910_ProfileWriteRaw`。`Menu_HandleKey` 控制形状、幅度、模式及 PD2 旁路继电器。`AutoMode_Service` 不输出控制命令。

## 分类与未启用代码

- `CMSIS/`、F407 `STM32/`：启动、内核、芯片标准库，保留以便离线重建。
- `APP` / `MY`：应用、测量、控制及调度。
- `HARDWARE` / `SYSTEM` / `Drive`：板级、DDS、LCD、串口、延时与 LED。
- `Tools`：原有 Python 模型、静态回归与字体生成工具。
- F103 `lcd_stubs.c` 是已有供应商演示字体 ROM 测试的空桩，应用不调用该演示；保留并明确其边界。
- DDS 源码还带 RAM/DRG 等供应商辅助函数；不能仅因函数存在就宣称当前菜单已集成相应波形/扫频功能。
- 源码中有未被 Keil 直接列入的库/配置文件；保留而不扩大功能声明。完整分类与 include 记录见逐文件索引。

## 保留路径的理由

工程源文件及头文件搜索路径均使用工程内部的相对路径。仅整体搬到 `firmware/F103_Project` 和 `firmware/F407_Project`。`Tools` 放在同一级，因为原静态回归从其父目录寻找这两个工程。未重构源文件、未转 HAL、未新增空实现。
