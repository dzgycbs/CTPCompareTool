#include "MdSpiEx.h"

MdSpiEx::MdSpiEx(
    LineType line,
    TickMatcher& matcher)
    :
    m_line(line),
    m_matcher(matcher)
{
}

void MdSpiEx::OnFrontConnected()
{
    if (m_line == LineType::Left)
    {
        OutputDebugStringA("[LEFT] Front Connected\n");
    }
    else
    {
        OutputDebugStringA("[RIGHT] Front Connected\n");
    }
}

void MdSpiEx::OnFrontDisconnected(int)
{
    if (m_line == LineType::Left)
    {
        OutputDebugStringA("[LEFT] OnFrontDisconnected\n");
    }
    else
    {
        OutputDebugStringA("[RIGHT] OnFrontDisconnected\n");
    }
}

void MdSpiEx::OnRspUserLogin(
    CThostFtdcRspUserLoginField*,
    CThostFtdcRspInfoField*,
    int,
    bool)
{
}

void MdSpiEx::OnRtnDepthMarketData(
    CThostFtdcDepthMarketDataField* pData)
{
    if (!pData)
        return;

    Tick tick = m_converter.Convert(*pData);

    tick.line = m_line;

    m_matcher.Push(tick);
}

void MdSpiEx::SetApi(CThostFtdcMdApi* api)
{
    m_api = api;
}
