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

    void OnRtnDepthMarketData(const CThostFtdcDepthMarketDataField* p);

private:

    TickMatcher& m_matcher;
};