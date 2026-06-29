#include "TickKeyBuilder.h"

#include <charconv>
#include <string>

namespace
{
    inline void AppendInt(std::string& str, int value)
    {
        char buffer[32];
        auto result = std::to_chars(buffer, buffer + sizeof(buffer), value);

        if (result.ec == std::errc())
        {
            str.append(buffer, result.ptr);
        }
    }

    inline void AppendDouble(std::string& str, double value)
    {
        // 第一版采用固定6位小数，保证不同线路生成完全一致
        char buffer[64];
        sprintf_s(buffer, "%.6f", value);
        str.append(buffer);
    }
}

std::string TickKeyBuilder::Build(const Tick& tick)
{
    std::string key;

    // 提前分配，避免多次扩容
    key.reserve(128);

    key.append(tick.tradingDay);
    key.push_back('|');

    key.append(tick.instrumentID);
    key.push_back('|');

    key.append(tick.updateTime);
    key.push_back('|');

    AppendDouble(key, tick.lastPrice);
    key.push_back('|');

    AppendInt(key, tick.volume);
    key.push_back('|');

    AppendDouble(key, tick.turnover);

    return key;
}