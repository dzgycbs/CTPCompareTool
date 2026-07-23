#pragma once
#include "Common.h"

struct StatisticsReport
{
    uint64_t samples;

    double avg;

    double p50;

    double p95;

    double p99;

    double max;

    double leftRate;

    double rightRate;

    double drawRate;

    double advantageAvg;

    double advantageMax;
};

//StatisticsReport BuildReport() const;