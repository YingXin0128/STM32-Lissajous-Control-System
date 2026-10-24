# 测量与控制算法（源码解释）

本页只解释 v2.2.6 的实现，不给出未测试的收敛或精度保证。源码路径见功能表。

## 测量

`Measurement_AnalyzeChannel` 在 8000 对采样窗口统计均值、min/max 与 Vpp；Vpp 小于 80 counts 时无效。以均值及 Vpp/8 滞回阈值寻找上升穿越，采用 Q16 线性插值。至少 3 次穿越，以首尾穿越之间的周期数估频：`f_mHz = Fs × 1000 × (crossings-1) × 65536 / (last_q16-first_q16)`，实现中含整数舍入。无 RMS 计算。

`Measurement_Analyze` 对参考频率做边界判断及 100 Hz 网格归整。`Measurement_CalculateIqPhase` 去均值后与递推正交振荡器相关，再用 `atan2f` 取相位；两路共用参考推导的频率基准。`Measurement_CalculateRelativePhase` 比较反馈与 1F、2F 目标的频差，选较近谐波，超过 5 Hz 捕获差则无效。计算 `wrap(feedback_phase - h × reference_phase - (h-1) × 90000)`，单位毫度，范围 (-180000,180000]。

ADC 低端 ≤32 或高端 ≥4063 counts 置削顶；恢复计数要求连续干净帧，参数为 3。源码存在恢复/无效相位标志，不能视为恢复失真波形的算法。

## DDS 字与频率环

`AD9910_FtwFromMilliHz` 按配置 1 GHz SYSCLK 生成 32 位 FTW，对应关系 `FTW ≈ f × 2^32 / 10^9`；`AD9910_PowFromMilliDegrees` 将周期相位映射到 16 位 POW。`AD9910_ProfileWriteRaw` 把 ASF、POW、FTW 按器件串行寄存器格式发送（这与 MCU 间协议的小端无关）。1 GHz 是软件假设，需核对模块时钟/倍频配置与实物。

`ManualControl_Update` 在新快照上处理频率，参考 1–100 kHz、100 Hz 步进；无穷形状将目标乘 2。反馈采用 3 点中值、1/4 IIR 与锁定期间离群确认。控制经 `Control_UpdateAcquire`、`Control_UpdateFine`、`Control_UpdateDpll` 状态推进。

粗调将直接频差换算 FTW，比例为 1/2，单次 FTW 步长限 ±128；当前 `Control_UpdateFine` 只检查有效相位、谐波和频差，并累计稳定帧后进入相位环，不直接改 FTW；不能将配置中保留的 `APP_FINE_*` 宏都当成已生效算法。时间戳相位斜率观测在 DPLL 中调用。这属于代码中的频率闭环阶段，不声称存在独立硬件 FLL。

## POW 与 DPLL-PI

进入相位环后，误差为目标相位减校正后的测量相位，再做周期折返和 1/8 低通。未对齐且误差超过 4°、连续 10 帧时做一次 POW 对齐；之后保持 POW，通过频率修正维持相位。目标/模式重置会重新开始这套状态。

`Control_UpdateDpll` 的比例项为 `2 mHz/degree × phase_error_degree`；积分项按真实采样时间戳增量积分，Ki 为 `0.2 mHz/(degree·s)`。积分前将相位误差限到 ±20°，0.2° 内进入死区；另有 50 帧相位斜率观测对积分状态的有限修正，不能把实现简化成无观测器的理想 PI。

总频率修正低于 50 kHz 限 ±500 mHz，自 50 kHz 起限 ±1000 mHz；每次输出修正步长限 2 mHz。`Control_ApplyDpllOutput` 使用 Q16 FTW 和分数累加器在相邻整数 FTW 间选择；提高的是平均频率分辨率，存在瞬时切换。

相位误差 1.5° 内连续 75 帧用于软件锁定标志；持续频差异常会回到捕获。锁定标志不是示波器 XY 稳定性的独立证据。全部参数以 `USER/app_config.h` 为准，本次未修改参数或算法。

## 幅值与自动模式

`voltage_settings` 选择 ASF 16383、12300、8192、4096；对应配置标称 0.666、0.500、0.333、0.1665 Vpp。未将 ADC 幅值误差闭环到 DAC/VCA810。`AutoMode_Service` 不处理输入，不构成自动控制。
