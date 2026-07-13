#pragma once

#include "ThostFtdcMdApi.h"

#include "TickMatcher.h"
#include "TickConverter.h"

class MdSpiEx : public CThostFtdcMdSpi
{
public:

    MdSpiEx(
        LineType line,
        TickMatcher& matcher);

public:

    virtual void OnFrontConnected() override;

    virtual void OnFrontDisconnected(int nReason) override;

    virtual void OnRspUserLogin(
        CThostFtdcRspUserLoginField* pRspUserLogin,
        CThostFtdcRspInfoField* pRspInfo,
        int nRequestID,
        bool bIsLast) override;

    virtual void OnRtnDepthMarketData(
        CThostFtdcDepthMarketDataField* pData) override;

public:
    void SetApi(CThostFtdcMdApi* api);

private:

    LineType      m_line;

    TickMatcher& m_matcher;

    TickConverter m_converter;

    CThostFtdcMdApi* m_api = nullptr;
};