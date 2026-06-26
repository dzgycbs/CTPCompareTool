#include "MainWindow.h"

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
        margin + topHeight + gap,
        clientWidth - margin * 2,
        clientHeight - topHeight - gap - margin,
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
}