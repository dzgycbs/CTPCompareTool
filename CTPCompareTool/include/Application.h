#pragma once

#include "Common.h"

#include "MainWindow.h"
#include "Statistics.h"
#include "TickMatcher.h"
#include "MdSpiEx.h"
#include "ThostFtdcMdApi.h"
#include "Config.h"
#include "ConnectionState.h"

class Application :  public IConnectionListener
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

public:
    void OnConnectionStateChanged(LineType line, ConnectionState state);

private:

    HINSTANCE m_hInstance = nullptr;

    MainWindow     m_mainWindow;

    Statistics     m_statistics;

    TickMatcher    m_tickMatcher;

    // Left
    CThostFtdcMdApi* m_leftApi = nullptr;
    MdSpiEx          m_leftSpi;

    // Right
    CThostFtdcMdApi* m_rightApi = nullptr;
    MdSpiEx          m_rightSpi;

    std::string      m_leftFront;
    std::string      m_rightFront;

    Config m_config;

    ConnectionState m_leftState =  ConnectionState::Idle;

    ConnectionState m_rightState =  ConnectionState::Idle;

};