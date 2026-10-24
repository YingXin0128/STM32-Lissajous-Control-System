# 本次构建与验证（2026-10-21）

在最终 `firmware/` 目录内执行 Keil 全量重建，没有从原包复制旧 OBJ/Objects。未连接、烧录或观察实物。

## 工具链

本机 μVision V5.14.0.0 / MDK-ARM Standard；实际编译器 **ARMCC V5.05 update 1 build 106**。版本来自生成的 build_log.htm 并摘录至 `validation/*_tool_versions.log`，不是照抄工程的 pCCUsed。

F103 工程字段记录 ARMCC 5.06 update 5 build 528，F407 字段记录 5.06 update 7 build 960；当前 Keil 实际调用本机 5.05。两个工程均可在本机工具链编译，但不声称与原版本二进制一致，也未验证所记录的两版 5.06。未修改工程以掩盖此差异。

## 原生构建

| 工程 / Keil Target | 结果 | Code | RO-data | RW-data | ZI-data |
|---|---|---:|---:|---:|---:|
| F103 / AD9910_Fanke_Programmable_Controller | 0 Error / 0 Warning | 31234 | 180070 | 292 | 10852 |
| F407 / STM32F407 | 0 Error / 0 Warning | 7760 | 456 | 68 | 97124 |

数值为 Keil Program Size，单位字节。日志：[F103](validation/F103_build.log)、[F407](validation/F407_build.log)。本地启动了 Keil `-j0 -r <uvprojx> -o <log>`，进程退出码均为 0，并检查末尾错误/警告与新生成 HEX。

## HEX 验证

使用原有 `firmware/Tools/validate_ihex.py` 检查本次新生成的两个 HEX，退出码均为 0，EOF 与校验和均通过：

| 固件 | 有效数据字节 | 最低地址 | 最高地址（含） |
|---|---:|---|---|
| F103 | 211596 | 0x08000000 | 0x08033A8B |
| F407 | 8284 | 0x08000000 | 0x0800205B |

路径、文件长度与 SHA-256 见 [artifacts.json](validation/artifacts.json)，地址检查原始结果见 `validation/*_hex_validation.log`。新输出只保留在本地忽略的 OBJ/Objects 中，不作为 GitHub 文件提交。HEX 结构正确不代表硬件可正常工作。

## 主机回归

以下原有脚本在最终目录执行，退出码全部为 0，原输出存于 `validation/` 的同名日志；机器可读汇总见 [host_results.json](validation/host_results.json)。

- `test_ftw_quantization.py`
- `test_measurement_protocol.py`
- `test_phase_hold_dpll_model_v212.py`
- `test_runtime_ui_scheduler_v226.py`
- `test_timestamp_dpll_model_v220.py`
- `test_v226_high_frequency_resilience.py`

它们是 Python 模型或源码静态检查；部分文件名保留历史版本号，不等于对该历史固件执行了测试。不在 MCU 上执行 C 算法，不覆盖模拟器件和硬件时序，也不单独证明采样无竞争、实时吞吐或电压/相位指标。

## 结构与来源检查

两工程文件引用分别为 45 与 16，源文件及 include 路径缺失均为 0。首轮整理时 277 个收录原文件的 SHA-256 与原交接包一致。之后为可读性仅给 7 个 C 文件增加中文注释，其余 270 个文件保留原字节；修订后执行代码词法单元一致性检查，记录见 [readability_review.json](validation/readability_review.json)。Keil 工程保持原配置。见 [structure_check.json](validation/structure_check.json) 和 [file_manifest.csv](file_manifest.csv)。新建说明文件不计入“原文件一致”数量。

## 原材料中的实物结论边界

原交接包 `Documents/03_已知问题与待验证清单.md` 明确记录：75 kHz 以上重复稳定性不足、参考 ADC 曾削顶、1 kHz 边界需复测、最终输出幅值抖动及菜单档位与单端输出不一致。本仓库只引用其问题状态，没有将旧日志的单点通过改写成当前全频段通过，也没有将独立视觉/斜坡实验的“10k 实测”用于证明双 MCU 控制工程。

TODO：补充与固件哈希对应的完整测试记录、仪器/负载/测点、跨三种形状的重复测试、相位/频率/电压指标和异常工况结果。当前无可公开宣告达标的全系统性能指标。

## 中文注释修订复验

7 个核心 C 文件增加中文注释后，两工程再次全量重建，均为 0 Error / 0 Warning；6 项原有主机回归再次通过，两个 HEX 校验通过。新 HEX 与注释前哈希一致，代码大小一致。除注释/空白外的词法单元一致性已逐项检查。旧版 Keil 退出时自动改写了 IDE 元数据，构建结束后已将两份 uvprojx 按原包逐字节恢复；未将工具改写带入交付。
