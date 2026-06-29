#pragma once

#include "Common.h"

enum class LineType
{
    Left = 0,
    Right = 1
};

struct Tick
{
    // 行情来源
    LineType line = LineType::Left;

    // 本机收到行情时间（微秒）
    uint64_t recvTimeUs = 0;

    // 本程序收到的第几个Tick（调试用）
    uint64_t sequence = 0;

    // ===== 以下来自CTP =====

    std::string tradingDay;

    std::string instrumentID;

    std::string updateTime;

    int updateMillisec = 0;

    double lastPrice = 0.0;

    int volume = 0;

    double turnover = 0.0;
};