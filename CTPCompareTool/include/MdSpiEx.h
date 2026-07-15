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

    virtual void OnRspSubMarketData(
        CThostFtdcSpecificInstrumentField* pSpecificInstrument,
        CThostFtdcRspInfoField* pRspInfo,
        int nRequestID,
        bool bIsLast) override;

    virtual void OnRtnDepthMarketData(
        CThostFtdcDepthMarketDataField* pData) override;

public:
    void SetApi(CThostFtdcMdApi* api);

    void SetLoginInfo(
        const std::string& broker,
        const std::string& user,
        const std::string& password);

    void Login();

    void SetInstrument(
        const std::string& instrument);

private:

    LineType      m_line;

    TickMatcher& m_matcher;

    TickConverter m_converter;

    CThostFtdcMdApi* m_api = nullptr;

private:
    bool m_loginSuccess = false;
    std::string m_brokerID;
    std::string m_userID;
    std::string m_password;
    std::string m_instrument;

    int m_requestID = 1;

    void Subscribe();
};