#include "TickConverter.h"
#include <chrono>
#include "Utils.h"

Tick TickConverter::Convert(const CThostFtdcDepthMarketDataField& src)
{
    Tick tick;

    tick.instrumentID = src.InstrumentID;
    tick.lastPrice = src.LastPrice;
    tick.volume = src.Volume;
    tick.turnover = src.Turnover;

    /*tick.exchangeID
    tick.bidPrice1
    tick.askPrice1
    tick.bidVolume1
    tick.askVolume1
    tick.updateTime
    tick.updateMillisec
    tick.tradingDay
    tick.actionDay*/

    //// ⭐关键统一点
    tick.recvTimeUs = GetCurrentTimeUs();

    return tick;
}