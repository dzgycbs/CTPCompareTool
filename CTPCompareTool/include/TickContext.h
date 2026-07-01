#pragma once

#include "TickStruct.h"
#include "ThostFtdcUserApiStruct.h"

//======================================================
// TickContext
//
// 作用：
//   作为CTP → 内部系统之间的“传输容器”
//
// 特点：
//   ✔ 不修改CTP结构
//   ✔ 承载扩展字段（line等）
//======================================================



struct TickContext
{
    const CThostFtdcDepthMarketDataField* md; // CTP原始数据

    LineType line;     // ⭐线路标识（关键扩展点）

    TickContext()
        : md(nullptr), line(LineType::Left)
    {
    }
};