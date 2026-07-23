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
    uint64_t matchedCount = 0;

    uint64_t leftWinCount = 0;
    uint64_t rightWinCount = 0;
    uint64_t drawCount = 0;


    double leftWinRate = 0;
    double rightWinRate = 0;
    double drawRate = 0;


    uint64_t minLatencyUs = 0;
    uint64_t maxLatencyUs = 0;

    double avgLatencyUs = 0;

    double p50LatencyUs = 0;
    double p95LatencyUs = 0;
    double p99LatencyUs = 0;

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

