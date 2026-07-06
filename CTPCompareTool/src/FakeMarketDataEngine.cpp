#include "FakeMarketDataEngine.h"
#include <thread>

FakeMarketDataEngine::FakeMarketDataEngine(
    const ExperimentConfig& cfg,
    MdSpiEx& spi)
    : m_cfg(cfg),
    m_rand(cfg.seed),
    m_spi(spi)
{
}

static Tick MakeBase()
{
    Tick t;
    t.instrumentID = "IF0001";
    t.lastPrice = 3000.0;
    t.volume = 100;
    return t;
}

void FakeMarketDataEngine::Run(int count)
{
    for (int i = 0; i < count; i++)
    {
        Tick base = MakeBase();

        // =========================
        // LEFT LINE
        // =========================
        Tick left = base;
        left.line = LineType::Left;

        int leftDelay = m_cfg.leftDelayUs +
            m_rand.RandInt(0, m_cfg.jitterUs);

        std::this_thread::sleep_for(
            std::chrono::microseconds(leftDelay));

        left.recvTimeUs = GetCurrentTimeUs();

        m_spi.OnRtnDepthMarketData(&left);

        // =========================
        // RIGHT LINE
        // =========================
        Tick right = base;
        right.line = LineType::Right;

        int rightDelay = m_cfg.rightDelayUs +
            m_rand.RandInt(0, m_cfg.jitterUs);

        std::this_thread::sleep_for(
            std::chrono::microseconds(rightDelay));

        right.recvTimeUs = GetCurrentTimeUs();

        m_spi.OnRtnDepthMarketData(&right);
    }
}