#include "FakeMarketDataGenerator.h"

FakeMarketDataGenerator::FakeMarketDataGenerator(
    const ExperimentConfig& cfg,
    MdSpiEx& spi)
    : m_cfg(cfg)
    , m_spi(spi)
    , m_rng(cfg.seed)
    , m_priceJitter(-0.01, 0.01)
{
}

// =========================
// 基础Tick模板
// =========================
Tick FakeMarketDataGenerator::MakeBaseTick()
{
    Tick t;
    t.instrumentID = "IF0001";
    t.lastPrice = 3000.0;
    t.volume = 100;
    return t;
}

// =========================
// 对外入口（唯一）
// =========================
void FakeMarketDataGenerator::Run(int count)
{
    for (int i = 0; i < count; i++)
    {
        GenerateOne();
    }
}

// =========================
// 单次生成（核心逻辑）
// =========================
void FakeMarketDataGenerator::GenerateOne()
{
    Tick base = MakeBaseTick();

    // =========================
    // LEFT LINE
    // =========================
    {
        Tick left = base;
        left.line = LineType::Left;

        left.lastPrice += m_priceJitter(m_rng);

        int delay = m_cfg.leftDelayUs +
            (m_rng() % m_cfg.jitterUs);

        std::this_thread::sleep_for(
            std::chrono::microseconds(delay));

        m_spi.OnRtnDepthMarketData(&left);
    }

    // =========================
    // RIGHT LINE
    // =========================
    {
        Tick right = base;
        right.line = LineType::Right;

        right.lastPrice += m_priceJitter(m_rng);

        int delay = m_cfg.rightDelayUs +
            (m_rng() % m_cfg.jitterUs);

        std::this_thread::sleep_for(
            std::chrono::microseconds(delay));

        m_spi.OnRtnDepthMarketData(&right);
    }
}