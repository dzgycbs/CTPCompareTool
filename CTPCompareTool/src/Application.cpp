#include "Application.h"
#include "Utils.h"

constexpr const char* LEFT_FLOW = "LeftFlow";
constexpr const char* RIGHT_FLOW = "RightFlow";



Application::Application():
    m_leftSpi(LineType::Left, m_tickMatcher),
    m_rightSpi(LineType::Right, m_tickMatcher)
{
    m_leftFront =  "tcp://116.236.239.140:42213";
    m_rightFront = "tcp://101.226.253.65:41313";
}

Application::~Application()
{
    Shutdown();
}

bool Application::Initialize(HINSTANCE hInstance)
{
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
    auto leftFlow = CreateFlowDir(LEFT_FLOW);
    auto rightFlow = CreateFlowDir(RIGHT_FLOW);

    m_leftApi =
        CThostFtdcMdApi::CreateFtdcMdApi(leftFlow.c_str());

    m_rightApi =
        CThostFtdcMdApi::CreateFtdcMdApi(rightFlow.c_str());

    if (!m_leftApi || !m_rightApi)
    {
        return false;
    }

    m_leftSpi.SetApi(m_leftApi);
    m_rightSpi.SetApi(m_rightApi);

    m_leftApi->RegisterSpi(&m_leftSpi);

    m_rightApi->RegisterSpi(&m_rightSpi);

    m_leftApi->RegisterFront(
        const_cast<char*>(m_leftFront.c_str()));

    m_rightApi->RegisterFront(
        const_cast<char*>(m_rightFront.c_str()));

    m_leftSpi.SetLoginInfo("9999","8020","123456");

    m_rightSpi.SetLoginInfo("9999", "8020", "123456");

    m_leftSpi.SetInstrument("rb2610");

    m_rightSpi.SetInstrument("rb2610");

    m_leftApi->Init();

    m_rightApi->Init();

    return true;
}

void Application::Stop()
{
    //
    // v0.3 开始实现
    //
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