# Changelog

2026-07-16

First real multi-thread race
found after Dual MdApi.

Reason:

Statistics was originally designed
under Fake single-thread environment.

After Dual MdApi,
OnTickMatched()
was entered simultaneously by
Left MdApi thread and Right MdApi thread.

Solution:

Statistics becomes a
thread-safe object.

Protect whole OnTickMatched()
instead of protecting deque only.


Version : v0.9.1

Mission
──────────────────────────
✔ Dual Login

Completed
──────────────────────────
✔ Front Connected
✔ ReqUserLogin
✔ OnRspUserLogin
✔ TradingDay Output
✔ DebugPrint()
✔ Utils Improvement

Verification
──────────────────────────
✔ 0 Error
✔ 0 Warning
✔ LEFT Login Success
✔ RIGHT Login Success


#v0.9.0 —— Dual MdApi Foundation

新增

双 MdApi 架构
双 MdSpiEx
TickConverter 接入真实 CTP 行情
双 Front 独立连接
Left / Right 独立 Flow 目录
MainWindow 生命周期接入 Start()

完成

双行情连接成功
LEFT / RIGHT Front Connected
0 Error
0 Warning

## v0.3.0 (2026-06-26)

### Added
- Tick 数据模型
- TickKeyBuilder
- TickMatcher
- ITickMatchListener

### Changed
- Key 不再保存在 Tick 中
- 使用 TickKeyBuilder 统一生成 Key

### Status
- Core Match Engine Completed

## v0.2.0

- 完成Win32框架
- 完成Application
- 完成MainWindow
- 完成StatusBar
- 完成ListView
- 完成窗口生命周期

## Next

- TickMatcher
- 双MdApi
- Statistics