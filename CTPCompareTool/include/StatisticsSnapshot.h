#pragma once

#include "Common.h"

//=============================================================================
// StatisticsSnapshot
//
// 统计结果快照
// 用于UI显示，不包含任何统计逻辑
//=============================================================================

struct StatisticsSnapshot
{
    uint64_t matchedCount;
    uint64_t leftWinCount;
    uint64_t rightWinCount;
    uint64_t drawCount;
    uint64_t minLatencyUs;
    uint64_t maxLatencyUs;
    double avgLatencyUs;
    // v0.8.0
    double p50LatencyUs;
    double p95LatencyUs;
    double p99LatencyUs;
};

struct LatencyPoint
{
    uint64_t timeUs;
    uint64_t leftUs;
    uint64_t rightUs;
};