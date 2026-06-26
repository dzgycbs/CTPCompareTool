#include "Application.h"

Application::Application()
{
}

Application::~Application()
{
    Shutdown();
}

bool Application::Initialize(HINSTANCE hInstance)
{
    m_hInstance = hInstance;

    //
    // 创建主窗口
    //
    if (!m_mainWindow.Create(m_hInstance))
    {
        return false;
    }

    return true;
}

int Application::Run(int nCmdShow)
{
    m_mainWindow.Show(nCmdShow);

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
    //
    // v0.3 开始实现
    //
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

    m_leftMd.reset();

    m_rightMd.reset();
}