#pragma once

#include "StatisticsReport.h"

#include <string>


class ReportWriter
{
public:

    static bool SaveJson(
        const StatisticsReport& report,
        const std::string& filePath);
};