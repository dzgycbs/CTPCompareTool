#include "MdSpiEx.h"
#include "Utils.h"

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
    m_listener->OnConnectionStateChanged(m_line, ConnectionState::FrontConnected);

    Login();

    m_listener->OnConnectionStateChanged(m_line, ConnectionState::Logining);
}

void MdSpiEx::OnFrontDisconnected(int)
{
    m_listener->OnConnectionStateChanged(m_line, ConnectionState::Disconnected);
}

void MdSpiEx::OnRspUserLogin(
    CThostFtdcRspUserLoginField* pRspUserLogin,
    CThostFtdcRspInfoField* pRspInfo,
    int nRequestID,
    bool bIsLast)
{
    std::ostringstream oss;
    if (m_line == LineType::Left)
    {
        oss << "[LEFT]";

    }
    else
    {
        oss << "[RIGHT]";
    }

    if (pRspInfo &&
        pRspInfo->ErrorID != 0)
    {
        oss << " [" << pRspInfo->ErrorID << "] " << pRspInfo->ErrorMsg;
    }
    else
    {
        m_listener->OnConnectionStateChanged(m_line, ConnectionState::LoginSuccess);
        oss << " TradingDay=" << pRspUserLogin->TradingDay;
        m_loginSuccess = true;
        Subscribe();
        m_listener->OnConnectionStateChanged(m_line, ConnectionState::Subscribing);
    }
    oss << "\n";
    DebugPrint(oss.str());
}

void MdSpiEx::OnRspSubMarketData(CThostFtdcSpecificInstrumentField* pSpecificInstrument, CThostFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast)
{
    m_listener->OnConnectionStateChanged(m_line, ConnectionState::Running);
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

void MdSpiEx::SetLoginInfo(const std::string& broker, const std::string& user, const std::string& password)
{
    m_brokerID = broker;
    m_userID = user;
    m_password = password;
}

void MdSpiEx::Login()
{
    CThostFtdcReqUserLoginField req = {};

    strcpy(req.BrokerID, m_brokerID.c_str());
    strcpy(req.UserID, m_userID.c_str());
    strcpy(req.Password, m_password.c_str());

    m_api->ReqUserLogin(&req, m_requestID++);
}

void MdSpiEx::SetInstrument(const std::string& instrument)
{
    m_instrument = instrument;
}

void MdSpiEx::SetConnectionListener(IConnectionListener* listener)
{
    m_listener = listener;
}

void MdSpiEx::Subscribe()
{
    const char* instruments[] =
    {
        m_instrument.c_str()
    };

    m_api->SubscribeMarketData(
        const_cast<char**>(instruments),
        1);
}
