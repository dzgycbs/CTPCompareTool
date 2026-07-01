#pragma once

#include "Common.h"
#include "MdSpiEx.h"

//======================================================
// FakeMarketDataGenerator
//
// 用于模拟双线路行情数据
// 测试 TickMatcher / Statistics
//======================================================

class FakeMarketDataGenerator
{
public:

    FakeMarketDataGenerator();

    // 生成一组“左/右”行情
    void Generate(MdSpiEx& spi);
};