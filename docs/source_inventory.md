# 整理前源码盘点（2026-10-21）

主基线：20260801 交接包 Project/v2.2.6；目标目录整理前为空。另一资料包包含 v2.2.0、v2.2.1 与独立斜坡/视觉工程，仅作版本线索，不混用。

两套工程均为标准外设库 + CMSIS，无 CubeMX .ioc / HAL 工程。

工程 `F103_Project/USER/VirtualCOMPort.uvprojx`：STM32F103ZE；入口 `USER/main.c`；文件引用 45 个，缺失 0 个。
工程编译器字段：`5060528::V5.06 update 5 (build 528)::ARMCC`。
内存配置：`IRAM(0x20000000,0x10000) IROM(0x08000000,0x80000) CPUTYPE("Cortex-M3") CLOCK(72000000) ELITTLE`。

工程 `F407_Project/Project/STM32F407.uvprojx`：STM32F407ZGTx；入口 `MY/main.c`；文件引用 16 个，缺失 0 个。
工程编译器字段：`5060960::V5.06 update 7 (build 960)::.\ARMCC`。
内存配置：`IRAM(0x20000000,0x20000) IRAM2(0x10000000,0x10000) IROM(0x08000000,0x100000) CPUTYPE("Cortex-M4") FPU2 CLOCK(12000000) ELITTLE`。

已确认调用链：F407 main → Measurement_Init/TakeReadyFrame/Analyze → Protocol_SendMeasurement；F103 main → USART DMA 接收 → SignalCapture_Service/TakeSnapshot → ManualControl_Update → AD9910_ProfileWriteRaw。
核心：双 ADC 同步、DMA、零交叉频率、IQ 相位、ADC 峰峰计数、UART CRC、频率粗细调及相位 PI、ASF 档位、按键与 LCD。
AutoMode_Service 为空；未发现应用层 RMS、VCA810 或 DAC 增益闭环；PD2 继电器存在菜单调用，完整衰减网络未确认。
视觉边界：External_Collaboration 的 K230 摄像/识别及配套斜坡实验属于独立协作基线，不收录。Project 内的 auto_mode 是被主入口和菜单引用的空扩展接口，保留以维持工程；LCD 是本地嵌入式菜单，不属于视觉识别。
整理策略：完整保留两工程内部路径；不改 .c/.h/.s/.uvprojx；复制 Tools 到 firmware/Tools 以保留现有测试相对路径；排除构建输出、GUI 状态、旧说明与清理批处理；逐项清单另附。
