#pragma once
#include "TickStruct.h"

enum class ConnectionState
{
    Idle,               // 初始

    Connecting,         // Init之后

    FrontConnected,     // TCP建立

    Logining,           // 登录中

    LoginSuccess,       // 登录成功

    Subscribing,        // 订阅中

    Running,            // 正常运行

    Disconnected,       // 掉线

    Stopped             // Stop()
};

class IConnectionListener
{
public:
    virtual void OnConnectionStateChanged(
        LineType line,
        ConnectionState state) = 0;
};

