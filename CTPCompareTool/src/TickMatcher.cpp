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
    std::lock_guard<std::mutex> lock(m_mutex);
    m_leftTicks.clear();
    m_rightTicks.clear();
}


void TickMatcher::MatchLeft(const Tick& tick)
{
    Tick rightTick;
    bool matched = false;

    std::string key =
        TickKeyBuilder::Build(tick);

    {
        std::lock_guard<std::mutex> lock(m_mutex);


        auto it =
            m_rightTicks.find(key);


        if (it != m_rightTicks.end())
        {
            rightTick = it->second;

            m_rightTicks.erase(it);

            matched = true;
        }
        else
        {
            m_leftTicks.emplace(key, tick);
        }
    }


    if (matched && m_listener)
    {
        m_listener->OnTickMatched(
            tick,
            rightTick);
    }
}

void TickMatcher::MatchRight(const Tick& tick)
{
    Tick leftTick;
    bool matched = false;


    std::string key =
        TickKeyBuilder::Build(tick);


    {
        std::lock_guard<std::mutex> lock(m_mutex);


        auto it =
            m_leftTicks.find(key);


        if (it != m_leftTicks.end())
        {
            leftTick = it->second;

            m_leftTicks.erase(it);

            matched = true;
        }
        else
        {
            m_rightTicks.emplace(key, tick);
        }
    }


    if (matched && m_listener)
    {
        m_listener->OnTickMatched(
            leftTick,
            tick);
    }
}