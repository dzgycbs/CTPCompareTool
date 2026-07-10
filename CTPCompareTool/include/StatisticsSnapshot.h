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


    double leftWinRate;
    double rightWinRate;
    double drawRate;


    uint64_t minLatencyUs;
    uint64_t maxLatencyUs;

    double avgLatencyUs;

    double p50LatencyUs;
    double p95LatencyUs;
    double p99LatencyUs;

    //================================
    // Advantage Analysis
    //================================

    // 平均领先幅度
    double avgAdvantageUs = 0;

    // 最大领先幅度
    uint64_t maxAdvantageUs = 0;


    // 当前领先方连续次数
    uint64_t leftStreak = 0;
    uint64_t rightStreak = 0;
};

struct LatencyPoint
{
    uint64_t timeUs;
    uint64_t leftUs;
    uint64_t rightUs;
};