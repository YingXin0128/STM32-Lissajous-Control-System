# 实际测量协议 v4

发送：`firmware/F407_Project/MY/protocol.c` 的 `Protocol_BuildFrame` / `Protocol_SendMeasurement`，由 F407 main 分析后调用。
接收：F103 `SYSTEM/usart/usart.c` 的循环 DMA，`HARDWARE/CAPTURE/signal_capture.c` 的 `Link_ProcessByte` / `Link_AcceptFrame`，由主循环 `SignalCapture_Service` 驱动。

USART1，921600 baud，8 数据位、1 停止位、无校验、无硬件流控。F407 PA9 TX → F103 PA10 RX，共地。当前协议只发送测量，没有反向控制/ACK/重发。

所有多字节整数字段为小端，偏移从 0 计。固定 48 字节，无结束标志；payload 长度字段 41，包含字节 5–45。

| 偏移 | 字节数 / 类型 | 含义 |
|---:|---|---|
| 0 | 1 / u8 | 0xA5 |
| 1 | 1 / u8 | 0x5A |
| 2 | 1 / u8 | 版本 0x04 |
| 3 | 1 / u8 | 测量类型 0x10 |
| 4 | 1 / u8 | payload 长度 41 |
| 5 | 2 / u16 | 帧序号，回绕 |
| 7 | 4 / u32 | 分析窗口中心时间戳，微秒，按采样周期累计 |
| 11 | 4 / u32 | 参考原始频率，mHz |
| 15 | 4 / u32 | 归整后的参考频率，mHz |
| 19 | 4 / i32 | 相对相位，毫度，补码 |
| 23 | 4 / u32 | 反馈频率，mHz |
| 27 | 2 / u16 | 参考峰峰值，ADC counts |
| 29 | 2 / u16 | 反馈峰峰值，ADC counts |
| 31 | 1 / u8 | flags，见下表 |
| 32 | 2 / u16 | F407 dropped_frames |
| 34 / 36 | 各 2 / u16 | 参考最小 / 最大 ADC counts |
| 38 / 40 | 各 2 / u16 | 反馈最小 / 最大 ADC counts |
| 42 | 2 / u16 | 分析耗时，微秒 |
| 44 | 2 / u16 | 固件 tag，0x0226 |
| 46 | 2 / u16 | CRC16 |

| flags 位 | 含义 |
|---|---|
| 0x01 | reference_valid |
| 0x02 | phase_valid |
| 0x04 | feedback_valid |
| 0x08 | DMA overrun |
| 0x10 | phase_harmonic_2 |
| 0x20 | reference_clipped |
| 0x40 | feedback_clipped |
| 0x80 | clip_recovery |

CRC 初值 0xFFFF，多项式 0x1021，逐字节左移、高位先处理，无最终异或，覆盖字节 2–45 共 44 字节；结果小端存放。两端 `Protocol_Crc16` / `Link_Crc16` 一致；所有字段偏移和版本/类型/长度常量已逐项对照，无发现布局差异。

接收端查找同步字，检查版本、类型、长度及 CRC，错误计数后重置解析状态；并非可靠传输协议。统计序号间隔、RX DMA 溢出、快照队列溢出和无效控制帧。`SignalCapture_GetSnapshot` 对年龄超过 100 ms 的参考/反馈数据失效处理；这是数据新鲜度检查，不是 ACK 超时。未找到逐字节组帧超时、重发或握手机制。

诊断 CSV 是另一条 USART2 / 115200 通道，由 `USER/main.c:App_SendCsvHeader/App_SendDiagnosticCsv` 定义，不能与二进制测量串口混接或将 CSV 列当作线协议字段。当前 CSV 列名含 `mhz` 时表示 millihertz（mHz），并非 MHz。
