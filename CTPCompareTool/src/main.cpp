#include "Application.h"

int APIENTRY wWinMain(
    HINSTANCE hInstance,
    HINSTANCE /*hPrevInstance*/,
    PWSTR /*lpCmdLine*/,
    int nCmdShow)
{
    //
    // DPI 感知（Win10/Win11）
    //
    SetProcessDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    //
    // 创建应用程序
    //
    Application app;

    //
    // 初始化
    //
    if (!app.Initialize(hInstance))
    {
        MessageBoxW(
            nullptr,
            L"Application initialize failed.",
            APP_TITLE,
            MB_ICONERROR | MB_OK);

        return -1;
    }

    //
    // 进入消息循环
    //
    return app.Run(nCmdShow);
}