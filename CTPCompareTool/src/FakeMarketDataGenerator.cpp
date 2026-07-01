#include "FakeMarketDataGenerator.h"
#include "MdSpiEx.h"

#include <chrono>
#include <thread>
#include <random>

static int64_t NowUs()
{
    using namespace std::chrono;
    return duration_cast<microseconds>(
        system_clock::now().time_since_epoch()
    ).count();
}

static double RandomPrice(double base)
{
    static std::default_random_engine eng((unsigned)time(nullptr));
    static std::uniform_real_distribution<double> dist(-0.2, 0.2);

    return base + dist(eng);
}

FakeMarketDataGenerator::FakeMarketDataGenerator()
{
}

void FakeMarketDataGenerator::Generate(MdSpiEx& spi)
{
    // 模拟一笔行情
    CThostFtdcDepthMarketDataField md = {};

    strcpy_s(md.InstrumentID, "IF0001");
    md.LastPrice = 3000.0;

    // ===== 左线路 =====
    md.LastPrice = RandomPrice(3000.0);
    spi.OnRtnDepthMarketData(&md);

    // 模拟延迟（关键！）
    std::this_thread::sleep_for(std::chrono::microseconds(30));

    // ===== 右线路 =====
    md.LastPrice = RandomPrice(3000.0);
    spi.OnRtnDepthMarketData(&md);

}