#pragma once

#include "TickContext.h"
#include "TickStruct.h"

//======================================================
// TickConverter
//======================================================

class TickConverter
{
public:

    static Tick Convert(const TickContext& ctx);
};