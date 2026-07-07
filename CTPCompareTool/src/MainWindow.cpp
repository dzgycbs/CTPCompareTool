#include "MainWindow.h"
#include "StatisticsSnapshot.h"
#include "Statistics.h"

MainWindow::MainWindow()
{
}

MainWindow::~MainWindow()
{
}

bool MainWindow::Create(HINSTANCE hInstance)
{
    m_hInstance = hInstance;

    // 初始化公共控件
    INITCOMMONCONTROLSEX icc = {};
    icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icc.dwICC =
        ICC_BAR_CLASSES |
        ICC_LISTVIEW_CLASSES |
        ICC_STANDARD_CLASSES;

    InitCommonControlsEx(&icc);

    if (!RegisterWindowClass())
    {
        return false;
    }

    m_hWnd = CreateWindowExW(
        0,
        L"CTPCompareMainWindow",
        APP_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1200,
        800,
        nullptr,
        nullptr,
        m_hInstance,
        this);

    return (m_hWnd != nullptr);
}

void MainWindow::Show(int nCmdShow)
{
    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);
}

void MainWindow::DrawLatencyChart(HDC hdc)
{
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    RECT rcLine;
    GetWindowRect(m_ui.hLeftGroup, &rcLine);
    MapWindowPoints(HWND_DESKTOP, m_hWnd, (LPPOINT)&rcLine, 2);

    RECT rcList;
    GetWindowRect(m_ui.hEventList, &rcList);
    MapWindowPoints(HWND_DESKTOP, m_hWnd, (LPPOINT)&rcList, 2);

    RECT rcChart =
    {
        10,          // 左
        rcLine.bottom + 10,     // 上（Left/Right 面板下面）
        rcClient.right - 10,
        rcList.top - 10         // 下（ListView 上面）
    };

    // 白色背景
    FillRect(hdc, &rcChart, (HBRUSH)GetStockObject(WHITE_BRUSH));

    // 黑色边框
    Rectangle(
        hdc,
        rcChart.left,
        rcChart.top,
        rcChart.right,
        rcChart.bottom);

    // 标题
    TextOutA(
        hdc,
        rcChart.left + 8,
        rcChart.top + 6,
        "Latency Chart",
        13);

   
    std::vector<POINT> pts;
    const auto& history =
        m_statistics->GetLatencyHistory();
    if (history.size() < 2)
    {
        return;
    }

    const auto& snapshot =
        m_statistics->GetSnapshot();

    char info[128];

    sprintf_s(
        info,
        "AVG %.2fms   MAX %.2fms   Samples %zu",
        snapshot.avgLatencyUs / 1000.0,
        snapshot.maxLatencyUs / 1000.0,
        history.size());


    TextOutA(
        hdc,
        rcChart.left + 150,
        rcChart.top + 6,
        info,
        static_cast<int>(strlen(info)));

    double avgLatency =
        m_statistics->GetSnapshot().avgLatencyUs;

    const int left = rcChart.left + 15;
    const int right = rcChart.right - 15;

    const int top = rcChart.top + 35;
    const int bottom = rcChart.bottom - 15;

    const int width = right - left;
    const int height = bottom - top;

    uint64_t maxLatency = 1;

    for (auto value : history)
    {
        if (value > maxLatency)
        {
            maxLatency = value;
        }
    }

    if (maxLatency == 0)
    {
        return;
    }

    // 增加20%顶部空间
    maxLatency =
        static_cast<uint64_t>(
            maxLatency * 1.2);

    double displayMax =
        static_cast<double>(maxLatency) * 1.2;

    if (avgLatency > displayMax)
    {
        displayMax = avgLatency * 1.2;
    }


    double stepX =
        static_cast<double>(width) /
        static_cast<double>(history.size() - 1);

    pts.reserve(history.size());

    for (size_t i = 0; i < history.size(); ++i)
    {
        double ratio =
            static_cast<double>(history[i]) /
            displayMax;

        int x =
            left +
            static_cast<int>(i * stepX);

        int y =
            bottom -
            static_cast<int>(ratio * height);

        pts.push_back({ x, y });
    }

    Polyline(
        hdc,
        pts.data(),
        static_cast<int>(pts.size())
    );

    int avgY =
        bottom -
        static_cast<int>(
            (avgLatency / displayMax) * height
            );


    MoveToEx(
        hdc,
        left,
        avgY,
        nullptr);

    LineTo(
        hdc,
        right,
        avgY);

    //----------------------------------------------------
    // Y Axis Grid
    //----------------------------------------------------

    int gridCount = 4;

    for (int i = 0; i <= gridCount; i++)
    {
        double ratio =
            static_cast<double>(i) /
            static_cast<double>(gridCount);


        int y =
            bottom -
            static_cast<int>(ratio * height);


        // 横向辅助线
        MoveToEx(
            hdc,
            left,
            y,
            nullptr);

        LineTo(
            hdc,
            right,
            y);


        // Y轴数值
        double valueMs =
            ratio *
            static_cast<double>(maxLatency)
            / 1000.0;


        char text[64];

        sprintf_s(
            text,
            "%.1fms",
            valueMs);


        TextOutA(
            hdc,
            rcChart.left + 5,
            y - 8,
            text,
            (int)strlen(text));
    }
}

void MainWindow::SetStatistics(Statistics* statistics)
{
    m_statistics = statistics;
}

LRESULT CALLBACK MainWindow::WndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    MainWindow* pThis = nullptr;

    if (message == WM_NCCREATE)
    {
        LPCREATESTRUCT pcs =
            reinterpret_cast<LPCREATESTRUCT>(lParam);

        pThis =
            reinterpret_cast<MainWindow*>(pcs->lpCreateParams);

        SetWindowLongPtr(
            hWnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(pThis));

        pThis->m_hWnd = hWnd;
    }
    else
    {
        pThis = reinterpret_cast<MainWindow*>(
            GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (pThis != nullptr)
    {
        return pThis->HandleMessage(
            message,
            wParam,
            lParam);
    }

    return DefWindowProc(
        hWnd,
        message,
        wParam,
        lParam);
}

bool MainWindow::RegisterWindowClass()
{
    WNDCLASSEXW wc = {};

    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWindow::WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = m_hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = L"CTPCompareMainWindow";
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    return RegisterClassExW(&wc) != 0;
}

LRESULT MainWindow::HandleMessage(
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        CreateControls();

        SetTimer(
            m_hWnd,
            IDT_REFRESH,
            500,
            nullptr);

        return 0;
    }

    case WM_SIZE:
    {
        LayoutControls();
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hWnd, &ps);

        DrawLatencyChart(hdc);

        EndPaint(m_hWnd, &ps);
        return 0;
    }

    case WM_TIMER:
    {
        if (wParam == IDT_REFRESH)
        {
            UpdateControls();
        }

        return 0;
    }

    case WM_CLOSE:
    {
        DestroyWindow(m_hWnd);
        return 0;
    }

    case WM_DESTROY:
    {
        KillTimer(
            m_hWnd,
            IDT_REFRESH);

        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProcW(
        m_hWnd,
        message,
        wParam,
        lParam);
}

void MainWindow::CreateControls()
{
    CreateStatusBar();

    CreateEventList();

    //
    // 左线路 GroupBox
    //
    m_ui.hLeftGroup = CreateWindowExW(
        0,
        L"Button",
        L"Left Line",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        0,
        0,
        100,
        100,
        m_hWnd,
        (HMENU)IDC_LEFT_GROUP,
        m_hInstance,
        nullptr);

    //
    // 右线路 GroupBox
    //
    m_ui.hRightGroup = CreateWindowExW(
        0,
        L"Button",
        L"Right Line",
        WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        0,
        0,
        100,
        100,
        m_hWnd,
        (HMENU)IDC_RIGHT_GROUP,
        m_hInstance,
        nullptr);
}

void MainWindow::CreateStatusBar()
{
    m_ui.hStatusBar = CreateWindowExW(
        0,
        STATUSCLASSNAMEW,
        nullptr,
        WS_CHILD |
        WS_VISIBLE |
        SBARS_SIZEGRIP,
        0,
        0,
        0,
        0,
        m_hWnd,
        (HMENU)IDC_STATUSBAR,
        m_hInstance,
        nullptr);

    SendMessageW(
        m_ui.hStatusBar,
        SB_SETTEXT,
        0,
        (LPARAM)L"Ready");

    int parts[] = { 120, 380, -1 };
    SendMessage(m_ui.hStatusBar, SB_SETPARTS, 3, (LPARAM)parts);
}

void MainWindow::CreateEventList()
{
    m_ui.hEventList = CreateWindowExW(
        WS_EX_CLIENTEDGE,
        WC_LISTVIEWW,
        L"",
        WS_CHILD |
        WS_VISIBLE |
        LVS_REPORT |
        LVS_SINGLESEL |
        LVS_SHOWSELALWAYS,
        0,
        0,
        100,
        100,
        m_hWnd,
        (HMENU)IDC_EVENTLIST,
        m_hInstance,
        nullptr);

    ListView_SetExtendedListViewStyle(
        m_ui.hEventList,
        LVS_EX_FULLROWSELECT |
        LVS_EX_GRIDLINES);

    LVCOLUMNW column = {};
    column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    //
    // Time
    //
    column.cx = 150;
    column.pszText = const_cast<LPWSTR>(L"Time");
    ListView_InsertColumn(
        m_ui.hEventList,
        0,
        &column);

    //
    // Winner
    //
    column.cx = 120;
    column.pszText = const_cast<LPWSTR>(L"Winner");
    ListView_InsertColumn(
        m_ui.hEventList,
        1,
        &column);

    //
    // Latency(us)
    //
    column.cx = 120;
    column.pszText = const_cast<LPWSTR>(L"Latency(us)");
    ListView_InsertColumn(
        m_ui.hEventList,
        2,
        &column);

    //
    // Instrument
    //
    column.cx = 150;
    column.pszText = const_cast<LPWSTR>(L"Instrument");
    ListView_InsertColumn(
        m_ui.hEventList,
        3,
        &column);
}

void MainWindow::LayoutControls()
{
    RECT rcClient{};
    GetClientRect(m_hWnd, &rcClient);

    //
    // StatusBar 自动计算高度
    //
    SendMessageW(m_ui.hStatusBar, WM_SIZE, 0, 0);

    RECT rcStatus{};
    GetWindowRect(m_ui.hStatusBar, &rcStatus);

    int statusHeight = rcStatus.bottom - rcStatus.top;

    int clientWidth = rcClient.right;
    int clientHeight = rcClient.bottom - statusHeight;

    constexpr int margin = 10;
    constexpr int gap = 10;

    //
    // 上半部分高度
    //
    int topHeight = 220;

    //
    // 左右Group宽度
    //
    int groupWidth =
        (clientWidth - margin * 2 - gap) / 2;

    //
    // 左线路
    //
    MoveWindow(
        m_ui.hLeftGroup,
        margin,
        margin,
        groupWidth,
        topHeight,
        TRUE);

    //
    // 右线路
    //
    MoveWindow(
        m_ui.hRightGroup,
        margin + groupWidth + gap,
        margin,
        groupWidth,
        topHeight,
        TRUE);

    //
    // Event List
    //
    MoveWindow(
        m_ui.hEventList,
        margin,
        margin + topHeight + gap + 150,
        clientWidth - margin * 2,
        clientHeight - topHeight - gap - margin - 150,
        TRUE);

    //
    // StatusBar
    //
    SendMessageW(
        m_ui.hStatusBar,
        WM_SIZE,
        0,
        0);
}

void MainWindow::UpdateControls()
{
    //
    // 第一版先刷新状态栏时间
    // 后面这里会改成 Statistics Snapshot
    //

    SYSTEMTIME st{};
    GetLocalTime(&st);

    wchar_t text[128]{};

    swprintf_s(
        text,
        L"Ready    %02d:%02d:%02d",
        st.wHour,
        st.wMinute,
        st.wSecond);

    SendMessageW(
        m_ui.hStatusBar,
        SB_SETTEXT,
        0,
        (LPARAM)text);


    StatisticsSnapshot snap = m_statistics->GetSnapshot();

    wchar_t statStr[128]{};

    swprintf_s(
        statStr,
        L"Matched:%llu Left:%llu Right:%llu Draw:%llu",
        snap.matchedCount,
        snap.leftWinCount,
        snap.rightWinCount,
        snap.drawCount);

    SendMessageW(
        m_ui.hStatusBar,
        SB_SETTEXT,
        1,
        (LPARAM)statStr);

    wchar_t latStr[128]{};

    swprintf_s(
        latStr,
        L"Avg:%.2fus",
        snap.avgLatencyUs);

    SendMessageW(
        m_ui.hStatusBar,
        SB_SETTEXT,
        2,
        (LPARAM)latStr);


    InvalidateRect(
        m_hWnd,
        nullptr,
        FALSE);


}