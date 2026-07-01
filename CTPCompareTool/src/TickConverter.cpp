#include "TickConverter.h"
#include <chrono>

static int64_t NowUs()
{
    using namespace std::chrono;

    return duration_cast<microseconds>(
        system_clock::now().time_since_epoch()
    ).count();
}

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
    tick.recvTimeUs = NowUs();
    tick.line = ctx.line;

    return tick;
}