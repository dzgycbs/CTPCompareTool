#pragma once

#include "TickMatcher.h"
#include "TickConverter.h"

//======================================================
// MdSpiEx
//======================================================

class MdSpiEx
{
public:

    MdSpiEx(TickMatcher& matcher);

    void OnRtnDepthMarketData(const Tick* tick);

private:

    TickMatcher& m_matcher;
};