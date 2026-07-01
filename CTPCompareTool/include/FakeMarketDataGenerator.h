#pragma once

#include <random>
#include <thread>
#include <chrono>

#include "MdSpiEx.h"
#include "TickStruct.h"
#include "ExperimentConfig.h"

// =========================
// Fake行情生成器
// =========================
class FakeMarketDataGenerator
{
public:
    FakeMarketDataGenerator(const ExperimentConfig& cfg, MdSpiEx& spi);

    // 对外唯一入口（收口！）
    void Run(int count);

private:
    // 单次生成（内部用）
    void GenerateOne();

    Tick MakeBaseTick();

private:
    ExperimentConfig m_cfg;
    MdSpiEx& m_spi;

    std::mt19937 m_rng;
    std::uniform_real_distribution<double> m_priceJitter;
};