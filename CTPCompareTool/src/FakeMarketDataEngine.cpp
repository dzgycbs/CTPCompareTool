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

static CThostFtdcDepthMarketDataField MakeBase()
{
    CThostFtdcDepthMarketDataField md = {};
    strcpy_s(md.InstrumentID, "IF0001");
    md.LastPrice = 3000.0;
    md.Volume = 100;
    return md;
}

void FakeMarketDataEngine::Run(int count)
{
    for (int i = 0; i < count; i++)
    {
        auto md = MakeBase();

        // =========================
        // LEFT LINE
        // =========================
        {
            md.LastPrice += m_rand.RandDouble(-0.01, 0.01);

            int delay = m_cfg.leftDelayUs
                + m_rand.RandInt(0, m_cfg.jitterUs);

            std::this_thread::sleep_for(
                std::chrono::microseconds(delay));

            m_spi.OnRtnDepthMarketData(&md);
        }

        // =========================
        // RIGHT LINE
        // =========================
        {
            md.LastPrice += m_rand.RandDouble(-0.01, 0.01);

            int delay = m_cfg.rightDelayUs
                + m_rand.RandInt(0, m_cfg.jitterUs);

            std::this_thread::sleep_for(
                std::chrono::microseconds(delay));

            m_spi.OnRtnDepthMarketData(&md);
        }
    }
}