#include "Statistics.h"
#include <cstdlib>   // std::llabs
#include <algorithm>
#include <iostream>


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

const std::deque<uint64_t>& Statistics::GetLatencyHistory() const
{
     return m_latencyHistory;
}

void Statistics::UpdatePercentile()
{
    if (m_latencyHistory.empty())
        return;


    std::vector<uint64_t> data(
        m_latencyHistory.begin(),
        m_latencyHistory.end());


    std::sort(
        data.begin(),
        data.end());


    auto getValue =
        [&](double ratio)
        {
            size_t index =
                static_cast<size_t>(
                    ratio * (data.size() - 1));

            return data[index];
        };


    m_snapshot.p50LatencyUs =
        static_cast<double>(
            getValue(0.50));


    m_snapshot.p95LatencyUs =
        static_cast<double>(
           getValue(0.95));


    m_snapshot.p99LatencyUs =
        static_cast<double>(
           getValue(0.99));
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

    /*char buf[256];

    sprintf_s(
        buf,
        "left=%llu right=%llu delta=%lld\n",
        left.recvTimeUs,
        right.recvTimeUs,
        delta);

    OutputDebugStringA(buf);*/

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


    m_latencyHistory.push_back(latencyUs);

    if (m_latencyHistory.size() > MAX_HISTORY)
    {
        m_latencyHistory.pop_front();
    }

    UpdatePercentile();
}