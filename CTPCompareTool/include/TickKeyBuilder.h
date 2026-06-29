#pragma once

#include "Common.h"
#include "TickStruct.h"

//=============================================================================
// TickKeyBuilder
//
// 根据Tick生成唯一Key，用于双线路行情匹配
//=============================================================================

class TickKeyBuilder
{
public:

    // 根据Tick生成唯一Key
    static std::string Build(const Tick& tick);
};