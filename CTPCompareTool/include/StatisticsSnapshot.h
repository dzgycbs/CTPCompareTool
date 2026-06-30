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
    // 已成功配对数量
    uint64_t matchedCount = 0;

    // Left线路领先次数
    uint64_t leftWinCount = 0;

    // Right线路领先次数
    uint64_t rightWinCount = 0;

    // 两条线路同时到达次数
    uint64_t drawCount = 0;

    // 最小延迟(us)
    uint64_t minLatencyUs = 0;

    // 最大延迟(us)
    uint64_t maxLatencyUs = 0;

    // 平均延迟(us)
    double avgLatencyUs = 0.0;
};