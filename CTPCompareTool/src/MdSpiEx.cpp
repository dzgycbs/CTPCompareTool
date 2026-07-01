#include "MdSpiEx.h"

MdSpiEx::MdSpiEx(TickMatcher& matcher)
    : m_matcher(matcher)
{
}

void MdSpiEx::OnRtnDepthMarketData(
    const Tick* tick)
{
    if (!tick) return;

    m_matcher.Push(*tick);
}