#include "TickConverter.h"
#include <chrono>

Tick TickConverter::Convert(const TickContext& ctx)
{
    Tick tick;

    const auto* md = ctx.md;
    if (!md) return tick;

    tick.instrumentID = md->InstrumentID;
    tick.lastPrice = md->LastPrice;
    tick.volume = md->Volume;
    tick.turnover = md->Turnover;

    // ⭐关键统一点
    tick.recvTimeUs = GetCurrentTimeUs();
    tick.line = ctx.line;

    return tick;
}