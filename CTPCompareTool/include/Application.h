#pragma once

#include "Common.h"

#include "MainWindow.h"
#include "Statistics.h"
#include "TickMatcher.h"
#include "MdSpiEx.h"

class Application
{
public:

    Application();
    ~Application();

public:

    bool Initialize(HINSTANCE hInstance);

    int Run(int nCmdShow);

    void Shutdown();

    bool Start();

    void Stop();

private:

    HINSTANCE m_hInstance = nullptr;

    MainWindow     m_mainWindow;

    Statistics     m_statistics;

    TickMatcher    m_tickMatcher;

    std::unique_ptr<MdSpiEx> m_leftMd;

    std::unique_ptr<MdSpiEx> m_rightMd;
};