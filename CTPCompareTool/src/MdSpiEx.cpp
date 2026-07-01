#include "MdSpiEx.h"

MdSpiEx::MdSpiEx(TickMatcher& matcher)
    : m_matcher(matcher)
{
}

void MdSpiEx::OnRtnDepthMarketData(
    const CThostFtdcDepthMarketDataField* p)
{
    if (!p) return;

    // =========================
    // Left Line
    // =========================
    {
        TickContext ctx;
        ctx.md = p;
        ctx.line = LineType::Left;

        m_matcher.Push(
            TickConverter::Convert(ctx)
        );
    }

    // =========================
    // Right Line
    // =========================
    {
        TickContext ctx;
        ctx.md = p;
        ctx.line = LineType::Right;

        m_matcher.Push(
            TickConverter::Convert(ctx)
        );
    }
}