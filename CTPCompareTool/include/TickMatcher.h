#pragma once

#include "Common.h"
#include "TickStruct.h"

#include <unordered_map>

//=============================================================================
// 配对结果回调
//=============================================================================

class ITickMatchListener
{
public:

    virtual ~ITickMatchListener() = default;

    virtual void OnTickMatched(
        const Tick& left,
        const Tick& right) = 0;
};

//=============================================================================
// TickMatcher
//=============================================================================

class TickMatcher
{
public:

    TickMatcher();

    void SetListener(ITickMatchListener* listener);

    void Push(const Tick& tick);

    void Clear();

private:

    void MatchLeft(const Tick& tick);

    void MatchRight(const Tick& tick);

private:
    mutable std::mutex m_mutex;

    std::unordered_map<std::string, Tick> m_leftTicks;

    std::unordered_map<std::string, Tick> m_rightTicks;

    ITickMatchListener* m_listener = nullptr;

};