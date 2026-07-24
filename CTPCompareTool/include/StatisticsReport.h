#pragma once

#include <string>
#include <cstdint>

#include "StatisticsSnapshot.h"


struct StatisticsReport
{
    //---------------------------------
   // Report Metadata
   //---------------------------------

    std::string version = "";

    std::string startTime = "";

    std::string endTime = "";

    //---------------------------------
    // 基础信息
    //---------------------------------

    std::string instrument = "";

    std::string tradingDay = "";


    //---------------------------------
    // 运行信息
    //---------------------------------

    uint64_t runtimeSeconds = 0;


    //---------------------------------
    // 统计结果
    //---------------------------------

    StatisticsSnapshot snapshot;
};