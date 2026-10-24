# 功能核验与证据

“已确认实现”指已定位到实现及调用，不代表实物验收。以下路径相对 `firmware/`。

| 检查项 | 状态 | 代码证据及边界 |
|---|---|---|
| F407 采集端 | 已确认实现 | `F407_Project/Project/STM32F407.uvprojx` Device；`MY/main.c` 直接调用 Measurement、Protocol |
| F103 控制端 | 已确认实现 | `F103_Project/USER/VirtualCOMPort.uvprojx` Device；`USER/main.c` 调用 ManualControl、Menu、SignalCapture |
| 双 ADC 同步 + DMA | 已确认实现 | `MY/measurement.c:Measurement_Init`：ADC_DualMode_RegSimult、ADC_CDR、DMA2_Stream0、TIM2；IRQ 由 `MY/stm32f4xx_it.c` 转发 |
| 频率测量 | 已确认实现 | `Measurement_Analyze` → `Measurement_AnalyzeChannel`；带滞回上升穿越及 Q16 插值 |
| 峰峰值 | 已确认实现 | 同上，max-min，ADC counts；不等于已标定物理电压 |
| RMS | 未找到实现 | Channel_Result 和 Measurement_Result 无 RMS；分析路径无平方平均开方；芯片库 ADC API 不等于 RMS 实现 |
| 相位差 | 已确认实现 | `Measurement_CalculateRelativePhase` → `Measurement_CalculateIqPhase`，1F/2F IQ 相位组合及偏置修正 |
| UART 测量通信 | 已确认实现 | `MY/protocol.c` → F103 `SYSTEM/usart/usart.c` → `HARDWARE/CAPTURE/signal_capture.c`；发送/接收字段逐项匹配 |
| AD9910 DDS | 已确认实现 | `ManualControl_Update` 经控制状态机调用 `Control_WriteToneFtw` → `AD9910_ProfileWriteRaw`；主入口调用 Init_AD9910 |
| AD9910 硬件 SPI 外设驱动 | 未找到实现 | `AD9910V1.C:txd_8bit` 用 GPIO 拉动 SCLK/SDIO；存在的是模拟串行写。LCD 的 SPI2 不能算作 DDS 硬件 SPI |
| FTW | 已确认实现 | `AD9910_FtwFromMilliHz`、`Control_ErrorToFtw`、`Control_ApplyDpllOutput`；真实控制路径调用 |
| POW 对齐 | 已确认实现 | `Control_UpdateDpll` 的持续相位误差判断 → `Control_WriteToneFtw`；`AD9910_PowFromMilliDegrees` 生成 POW |
| FLL 粗调/细调 | 已确认实现 | `Control_UpdateAcquire` 按频差调 FTW；`Control_UpdateFine` 确认条件后转入相位环，不直接改 FTW。相位斜率观测在 DPLL 中使用；不新增 FLL 模块 |
| DPLL-PI | 已确认实现 | `Control_UpdateDpll`：相位滤波、时间戳积分、限幅、相邻 FTW 输出；所有参数来自 USER/app_config.h |
| 幅值档位 | 已确认实现 | `APP/manual_control.c:voltage_settings` 和 `Control_WriteToneFtw` 写 ASF；无电压误差闭环 |
| DAC/VCA810 增益控制 | 未找到实现 | 应用/板级调用无 VCA810 和 DAC_SetChannel*；库内 stm32f10x_dac.c 不能当作应用功能 |
| 继电器 GPIO | 已确认实现 | `BOARD/board.c:Board_RelaySet`，`APP/menu_v2.c:Menu_HandleKey` 在旁路菜单调用 |
| 继电器衰减网络 | 待核实 | 仅有 PD2 输出与旁路状态，无多档网络或完整原理图支持 |
| 自动模式 | 部分实现 | `AutoMode_Enter/Exit` 状态存在且被菜单调用；`AutoMode_Service` 只有 `(void)capture`，无算法 |
| 视觉反馈/三板协议 | 未找到实现 | 所选两 MCU 应用无摄像/识别调用；独立 External_Collaboration 被排除，不能将其草案写入当前实现 |
| LCD/按键/CSV | 已确认实现 | main → Menu/Board/DebugUart；分片渲染、20 次稳定采样消抖、DMA CSV 发送 |

逐个 C/H 文件的函数定义和显式 include 见 [source_index.md](source_index.md)。静态索引不推断条件编译下的完整可达性；运行链路以上表和架构说明为准。
