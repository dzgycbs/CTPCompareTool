#include "FakeMarketDataEngine.h"
#include <thread>
#include "Utils.h"

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
    for (int i = 0; i < count; ++i)
    {
        Tick base = MakeBase();

        uint64_t startTime = GetCurrentTimeUs();

        //-----------------------------------
        // LEFT
        //-----------------------------------

        Tick left = base;
        left.line = LineType::Left;

        int leftDelay =
            m_cfg.leftDelayUs +
            m_rand.RandInt(0, m_cfg.jitterUs);

        left.recvTimeUs =
            startTime + leftDelay +
            m_rand.RandInt(-100, 100);

        //-----------------------------------
        // RIGHT
        //-----------------------------------

        Tick right = base;
        right.line = LineType::Right;

        int rightDelay =
            m_cfg.rightDelayUs +
            m_rand.RandInt(0, m_cfg.jitterUs);

        right.recvTimeUs =
            startTime + rightDelay +
            m_rand.RandInt(-100, 100);

        //-----------------------------------
        // 按时间先后发送
        //-----------------------------------

        if (left.recvTimeUs <= right.recvTimeUs)
        {
            //m_spi.OnRtnDepthMarketData(&left);
            //m_spi.OnRtnDepthMarketData(&right);
        }
        else
        {
            //m_spi.OnRtnDepthMarketData(&right);
            //m_spi.OnRtnDepthMarketData(&left);
        }

        // 控制Tick产生速度，避免CPU空转
        std::this_thread::sleep_for(
            std::chrono::milliseconds(1));
    }
}