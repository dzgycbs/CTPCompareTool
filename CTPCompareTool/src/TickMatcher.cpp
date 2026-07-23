#include "TickMatcher.h"

#include "TickKeyBuilder.h"

TickMatcher::TickMatcher()
{
}

void TickMatcher::SetListener(ITickMatchListener* listener)
{
    m_listener = listener;
}

void TickMatcher::Push(const Tick& tick)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (tick.line == LineType::Left)
    {
        MatchLeft(tick);
    }
    else
    {
        MatchRight(tick);
    }
}

void TickMatcher::Clear()
{
    m_leftTicks.clear();
    m_rightTicks.clear();
}

void TickMatcher::MatchLeft(const Tick& tick)
{
    std::string key = TickKeyBuilder::Build(tick);

    auto it = m_rightTicks.find(key);

    if (it != m_rightTicks.end())
    {
        if (m_listener != nullptr)
        {
            m_listener->OnTickMatched(
                tick,
                it->second);
        }

        m_rightTicks.erase(it);

        return;
    }

    m_leftTicks.emplace(key, tick);
}

void TickMatcher::MatchRight(const Tick& tick)
{
    std::string key = TickKeyBuilder::Build(tick);

    auto it = m_leftTicks.find(key);

    if (it != m_leftTicks.end())
    {
        if (m_listener != nullptr)
        {
            m_listener->OnTickMatched(
                it->second,
                tick);
        }

        m_leftTicks.erase(it);

        return;
    }

    m_rightTicks.emplace(key, tick);
}