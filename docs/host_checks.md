# Python 主机检查

这些检查使用 Python 运行模型或检查源码中的静态约束，用于帮助维护测量协议、控制模型和界面调度。它们不在 MCU 上执行固件，也不证明输出电压、相位精度或全频段稳定性。

## 本地运行

环境：Python 3.11；这六个脚本使用 Python 标准库，无额外 pip 依赖。在仓库根目录运行：

```powershell
python firmware/Tools/test_ftw_quantization.py
python firmware/Tools/test_measurement_protocol.py
python firmware/Tools/test_phase_hold_dpll_model_v212.py
python firmware/Tools/test_runtime_ui_scheduler_v226.py
python firmware/Tools/test_timestamp_dpll_model_v220.py
python firmware/Tools/test_v226_high_frequency_resilience.py
```

每个脚本应返回退出码 0。具体断言与模型条件以脚本自身为准，不从脚本名称推断硬件测试覆盖率。

## 检查结果的含义

六项脚本均返回退出码 0，只能说明其中的模型和静态断言通过。历史执行记录保存在 `docs/validation/`；脚本运行时会打印本次结果。

脚本不安装 Keil/ARMCC，不自动烧录，也不生成或发布 HEX。芯片工程的编译入口和本地验证结果仍以 [构建与测试记录](test_results.md) 为准。
