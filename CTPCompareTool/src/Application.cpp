#include "Application.h"
#include "Utils.h"
#include "ReportWriter.h"


Application::Application():
    m_leftSpi(LineType::Left, m_tickMatcher),
    m_rightSpi(LineType::Right, m_tickMatcher)
{

}

Application::~Application()
{
    Shutdown();
}

bool Application::Initialize(HINSTANCE hInstance)
{
    if (!m_config.Load(
        "./config/config.json"))
    {
        DebugPrint(
            "Load config failed\n");

        return false;
    }

    if (m_config.Validate() == false)
    {
        DebugPrint(
            "config Validate faild\n");

        return false;
    }

    m_hInstance = hInstance;

    m_tickMatcher.SetListener(&m_statistics);
    //
    // 创建主窗口
    //
    if (!m_mainWindow.Create(m_hInstance))
    {
        return false;
    }
    
    m_mainWindow.SetStatistics(&m_statistics);
   
    return true;
}

int Application::Run(int nCmdShow)
{
    m_mainWindow.Show(nCmdShow);

    if (!Start())
    {
        return -1;
    }


    MSG msg = {};

    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}

bool Application::Start()
{
    m_runStartTime = std::chrono::steady_clock::now();
    m_reportStartTime = CurrentDateTime();

    OnConnectionStateChanged(LineType::Left, ConnectionState::Idle);
    OnConnectionStateChanged(LineType::Right, ConnectionState::Idle);

    auto leftFlow = EnsureDirectory(m_config.Left().flowPath.c_str());
    auto rightFlow = EnsureDirectory(m_config.Right().flowPath.c_str());
    EnsureDirectory("reports");

    m_leftApi = CThostFtdcMdApi::CreateFtdcMdApi(leftFlow.c_str());

    m_rightApi = CThostFtdcMdApi::CreateFtdcMdApi(rightFlow.c_str());

    if (!m_leftApi || !m_rightApi)
    {
        return false;
    }

    m_leftFront = m_config.Left().front;
    m_rightFront = m_config.Right().front;

    m_leftSpi.SetApi(m_leftApi);
    m_rightSpi.SetApi(m_rightApi);

    m_leftApi->RegisterSpi(&m_leftSpi);

    m_rightApi->RegisterSpi(&m_rightSpi);

    m_leftApi->RegisterFront(const_cast<char*>(m_leftFront.c_str()));

    m_rightApi->RegisterFront(const_cast<char*>(m_rightFront.c_str()));

    m_leftSpi.SetLoginInfo(m_config.Left().brokerID, m_config.Left().userID, m_config.Left().password);

    m_rightSpi.SetLoginInfo(m_config.Right().brokerID, m_config.Right().userID, m_config.Right().password);

    m_leftSpi.SetInstrument(m_config.Instruments().front());

    m_rightSpi.SetInstrument(m_config.Instruments().front());

    m_leftSpi.SetConnectionListener(this);

    m_rightSpi.SetConnectionListener(this);

    m_leftApi->Init();

    m_rightApi->Init();

    OnConnectionStateChanged(LineType::Left, ConnectionState::Connecting);
    OnConnectionStateChanged(LineType::Right, ConnectionState::Connecting);

    return true;
}

void Application::Stop()
{
    OnConnectionStateChanged(LineType::Left, ConnectionState::Stopped);
    OnConnectionStateChanged(LineType::Right, ConnectionState::Stopped);

    auto now = std::chrono::steady_clock::now();

    auto seconds =
        std::chrono::duration_cast<
        std::chrono::seconds>(
            now - m_runStartTime)
        .count();

    auto report = m_statistics.BuildReport();

    report.startTime = m_reportStartTime;

    report.endTime = CurrentDateTime();

    report.runtimeSeconds = seconds;

    report.tradingDay =  GetTradingDay();

    report.instrument =  m_config.Instruments()[0];

    report.version = WStringToString(APP_VERSION_STRING);

    ReportWriter::SaveJson(
        report,
        "./reports/report.json");

    DebugPrint(
        "Report Samples=" +
        std::to_string(
            report.snapshot.matchedCount)
        +
        "\n");

}

void Application::OnConnectionStateChanged(
    LineType line,
    ConnectionState newState)
{
    ConnectionState& current =
        (line == LineType::Left)
        ? m_leftState
        : m_rightState;

    if (current == newState)
        return;

    DebugStateTransition(line,current,newState);

    current = newState;
}


void Application::Shutdown()
{
    Stop();

    if (m_leftApi)
    {
        m_leftApi->Release();
        m_leftApi = nullptr;
    }

    if (m_rightApi)
    {
        m_rightApi->Release();
        m_rightApi = nullptr;
    }

}