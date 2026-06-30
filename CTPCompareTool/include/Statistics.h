#pragma once

#include "Common.h"
#include "TickMatcher.h"
#include "StatisticsSnapshot.h"

//=============================================================================
// Statistics
//
// 行情统计引擎
// 负责统计双线路延迟，不负责UI显示
//=============================================================================

class Statistics : public ITickMatchListener
{
public:

    Statistics();

    // 重置统计
    void Reset();

    // 获取统计快照
    StatisticsSnapshot GetSnapshot() const;

public:

    // ITickMatchListener
    virtual void OnTickMatched(
        const Tick& left,
        const Tick& right) override;

private:

    StatisticsSnapshot m_snapshot;

    // 延迟累计(us)
    uint64_t m_totalLatencyUs = 0;

    bool m_hasData = false;
};