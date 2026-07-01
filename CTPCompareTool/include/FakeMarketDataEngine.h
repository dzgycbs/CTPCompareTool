#pragma once

#include "ExperimentConfig.h"
#include "RandomEngine.h"
#include "MdSpiEx.h"

//======================================================
// Fake行情引擎（核心调度器）
//======================================================

class FakeMarketDataEngine
{
public:
    FakeMarketDataEngine(const ExperimentConfig& cfg, MdSpiEx& spi);

    void Run(int count);

private:
    ExperimentConfig m_cfg;
    RandomEngine m_rand;
    MdSpiEx& m_spi;
};