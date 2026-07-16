#pragma once

#include "Common.h"
#include "TickMatcher.h"
#include "StatisticsSnapshot.h"
#include <deque>

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

    const std::deque<uint64_t>& LatencyChartData() const;

    void UpdatePercentile();

    //StatisticsReport BuildReport() const;

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

    // 保留全部历史数据，供 Percentile / Report 使用
    std::deque<uint64_t> m_statisticsHistory;

    std::deque<uint64_t> m_chartHistory;

    static constexpr size_t MAX_CHART_HISTORY = 1000;

    uint64_t m_totalAdvantageUs = 0;

    uint64_t m_maxAdvantageUs = 0;

    uint64_t m_leftStreak = 0;

    uint64_t m_rightStreak = 0;

    mutable std::mutex m_historyMutex;
};

