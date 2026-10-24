# 项目文档导航

先从根目录 [项目首页](../README.md) 查看源码入口和推荐阅读顺序，再按问题查阅下面的资料。

| 想了解什么 | 阅读文档 |
|---|---|
| 测量、控制、DDS 的职责和调用关系 | [系统架构](architecture.md) |
| 频率测量、IQ 相位、POW 对齐与 DPLL-PI | [算法说明](algorithms.md) |
| 两 MCU 之间发送哪些数据、如何校验 | [通信协议](communication_protocol.md) |
| 芯片、引脚、时钟、模拟链路与接线边界 | [硬件说明](hardware.md) |
| 如何打开 Keil 工程，以及实际验证结果 | [构建与测试记录](test_results.md) |
| 如何运行随源码提供的 Python 检查 | [主机检查说明](host_checks.md) |
| 哪些功能有实现，哪些仍待补充 | [功能核验表](feature_audit.md) |

## 来源与整理记录

这组文件用于核对工程来源，不必在初次阅读时逐个打开。

- [整理前源码盘点](source_inventory.md)
- [逐文件函数与依赖索引](source_index.md)
- [原文件及 SHA-256 清单](file_manifest.csv)
- [交付与差异报告](delivery_report.md)
- [公开文件路径清单](repository_tree.txt)
- [第三方代码说明](../THIRD_PARTY_NOTICES.md)

`validation/` 保存构建日志、HEX 校验和主机脚本检查记录。2026-10-21 的 Keil 构建属于历史验证；Python 主机检查属于另一种验证，不替代 MCU 编译或实物测试。
