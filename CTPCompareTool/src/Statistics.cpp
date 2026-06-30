#include "Statistics.h"
#include <cstdlib>   // std::llabs
#include <algorithm>


Statistics::Statistics()
{
    Reset();
}

void Statistics::Reset()
{
    m_snapshot = StatisticsSnapshot();

    m_totalLatencyUs = 0;

    m_hasData = false;
}

StatisticsSnapshot Statistics::GetSnapshot() const
{
    return m_snapshot;
}

void Statistics::OnTickMatched(
    const Tick& left,
    const Tick& right)
{
    int64_t delta =
        static_cast<int64_t>(left.recvTimeUs) -
        static_cast<int64_t>(right.recvTimeUs);

    uint64_t latencyUs =
        static_cast<uint64_t>(std::llabs(delta));

    //----------------------------------------------------
    // 基本统计
    //----------------------------------------------------

    ++m_snapshot.matchedCount;

    if (delta < 0)
    {
        ++m_snapshot.leftWinCount;
    }
    else if (delta > 0)
    {
        ++m_snapshot.rightWinCount;
    }
    else
    {
        ++m_snapshot.drawCount;
    }

    //----------------------------------------------------
    // Min / Max
    //----------------------------------------------------

    if (!m_hasData)
    {
        m_snapshot.minLatencyUs = latencyUs;
        m_snapshot.maxLatencyUs = latencyUs;

        m_hasData = true;
    }
    else
    {
        m_snapshot.minLatencyUs =
            std::min(m_snapshot.minLatencyUs, latencyUs);

        m_snapshot.maxLatencyUs =
            std::max(m_snapshot.maxLatencyUs, latencyUs);
    }

    //----------------------------------------------------
    // Average
    //----------------------------------------------------

    m_totalLatencyUs += latencyUs;

    m_snapshot.avgLatencyUs =
        static_cast<double>(m_totalLatencyUs) /
        static_cast<double>(m_snapshot.matchedCount);
}