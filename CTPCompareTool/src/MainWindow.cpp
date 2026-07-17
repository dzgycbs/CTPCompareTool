#include "MainWindow.h"
#include "StatisticsSnapshot.h"
#include "Statistics.h"
#include <windowsx.h>

// ------------------------------------------------------------------------
// 1. 配色方案（统一管理，改起来方便）
// ------------------------------------------------------------------------
namespace ChartTheme {
    const COLORREF BACKGROUND = RGB(248, 250, 252);   // 极浅灰蓝
    const COLORREF GRID_LINE = RGB(225, 230, 240);   // 浅灰虚线
    const COLORREF BORDER_LINE = RGB(200, 208, 220);   // 边框色
    const COLORREF CARD_BG = RGB(255, 255, 255);   // 卡片底色
    const COLORREF CARD_SHADOW = RGB(220, 225, 235);   // 卡片边框
    const COLORREF TEXT_PRIMARY = RGB(60, 70, 80);      // 主文字色
    const COLORREF TEXT_SECONDARY = RGB(150, 160, 170);   // 辅助文字色
    const COLORREF COLOR_ORANGE = RGB(255, 152, 0);      // 橙色

    const COLORREF CURVE_GREEN = RGB(76, 175, 80);
    const COLORREF CURVE_YELLOW = RGB(255, 193, 7);
    const COLORREF CURVE_RED = RGB(244, 67, 54);
    const COLORREF CURVE_DEFAULT = RGB(63, 81, 181);    // 靛蓝

    // 参考线颜色
    const COLORREF LINE_AVG = RGB(0, 188, 212);   // 亮青
    const COLORREF LINE_P95 = RGB(233, 30, 99);      // 粉红
    const COLORREF LINE_P99 = RGB(156, 39, 176);     // 紫色
}

// ------------------------------------------------------------------------
// 2. 布局常量（拒绝 Magic Number）
// ------------------------------------------------------------------------
struct ChartLayout {
    int marginLeft = 55;    // Y轴标签预留宽度
    int marginRight = 15;
    int marginTop = 65;    // 标题卡片高度
    int marginBottom = 20;
    int gridCount = 5;     // 网格线数量
    int axisLabelWidth = 50;
};



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

void MainWindow::DrawEmptyState(HDC hdc, RECT& rcChart)
{
    // 背景（浅灰蓝）
    HBRUSH hBgBrush = CreateSolidBrush(ChartTheme::BACKGROUND);
    FillRect(hdc, &rcChart, hBgBrush);
    DeleteObject(hBgBrush);

    // 虚线边框
    HPEN hDashPen = CreatePen(PS_DASH, 1, ChartTheme::GRID_LINE);
    SelectObject(hdc, hDashPen);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc,
        rcChart.left + 10, rcChart.top + 10,
        rcChart.right - 10, rcChart.bottom - 10);
    DeleteObject(hDashPen);

    // 提示文字（居中）
    SetTextColor(hdc, ChartTheme::TEXT_SECONDARY);
    SetBkMode(hdc, TRANSPARENT);
    const char* msg = "Waiting for market data...";
    DrawTextA(hdc, msg, (int)strlen(msg), &rcChart,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

// ------------------------------------------------------------------------
// 3. 主绘制函数（双缓冲 + 清晰流程）
// ------------------------------------------------------------------------
void MainWindow::DrawLatencyChart(HDC hdc)
{
    // ---- 3.1 计算绘图区域 ----
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    RECT rcLine, rcList;
    GetWindowRect(m_ui.hLeftGroup, &rcLine);
    GetWindowRect(m_ui.hEventList, &rcList);
    MapWindowPoints(HWND_DESKTOP, m_hWnd, (LPPOINT)&rcLine, 2);
    MapWindowPoints(HWND_DESKTOP, m_hWnd, (LPPOINT)&rcList, 2);

    const int MARGIN = 10;
    RECT rcChart = {
        MARGIN ,
        rcLine.bottom,
        rcClient.right - MARGIN,
        rcList.top
    };

    if (rcChart.right <= rcChart.left || rcChart.bottom <= rcChart.top) {
        return;
    }

    // ---- 3.2 双缓冲 ----
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdc,
        rcChart.right - rcChart.left,
        rcChart.bottom - rcChart.top);
    HGDIOBJ hOldBmp = SelectObject(hdcMem, hbmMem);

    // 相对坐标（0,0 是图表左上角）
    RECT rcRelative = {
        0, 0,
        rcChart.right - rcChart.left,
        rcChart.bottom - rcChart.top
    };

    // ---- 3.3 绘制背景（✅ 用相对坐标） ----
    HBRUSH hBgBrush = CreateSolidBrush(ChartTheme::BACKGROUND);
    FillRect(hdcMem, &rcRelative, hBgBrush);  // ← 改这里！
    DeleteObject(hBgBrush);

    // ---- 3.4 获取数据 ----
    const auto& history = m_statistics->LatencyChartData();
    if (history.size() < 2) {
        DrawEmptyState(hdcMem, rcRelative);
        BitBlt(hdc,
            rcChart.left, rcChart.top,
            rcChart.right - rcChart.left,
            rcChart.bottom - rcChart.top,
            hdcMem,
            0, 0,
            SRCCOPY);
        SelectObject(hdcMem, hOldBmp);
        DeleteObject(hbmMem);
        DeleteDC(hdcMem);
        return;
    }
    const auto& s = m_statistics->GetSnapshot();

    // ---- 3.5 绘制信息卡片（✅ 用相对坐标） ----
    DrawInfoCard(hdcMem, rcRelative, s, history.size());  // ← 改这里！

    // ---- 3.6 准备绘图区（✅ 用相对坐标） ----
    const int left = rcRelative.left + 10;      // ← 改这里！
    const int right = rcRelative.right - 10;    // ← 改这里！
    const int top = rcRelative.top + ChartLayout().marginTop;   // ← 改这里！
    const int bottom = rcRelative.bottom - ChartLayout().marginBottom; // ← 改这里！

    // ---- 3.7 计算 Y 轴范围 ----
    uint64_t maxLatency = 1;
    for (auto value : history) {
        if (value > maxLatency) maxLatency = value;
    }
    double displayMax = static_cast<double>(maxLatency) * 1.2;
    if (s.avgLatencyUs * 1.5 > displayMax) {
        displayMax = s.avgLatencyUs * 1.5;
    }
    if (displayMax < 1000.0) {
        displayMax = 1000.0;
    }

    // ---- 3.8 绘制网格 + Y 轴标签 ----
    DrawGridAndLabels(hdcMem, rcRelative, left, right, top + 50, bottom, displayMax);

    // ---- 3.9 绘制数据曲线 ----
    DrawCurve(hdcMem, left + 60, right, top + 50, bottom, history, displayMax);

    // ---- 3.10 绘制参考线 ----
    DrawReferenceLines(hdcMem, left + 60, right, top + 50, bottom, s, displayMax);


    // ---- 绘制悬停浮窗 ----
    if (m_bShowTooltip && m_hoverIndex >= 0) {
        const auto& history = m_statistics->LatencyChartData();
        if (m_hoverIndex < (int)history.size()) {
            // 鼠标坐标 → 相对坐标（相对于 rcChart 左上角）
            int relX = m_ptHover.x - rcChart.left;
            int relY = m_ptHover.y - rcChart.top;

            RECT rcTooltip = {
                relX + 12,
                relY + 12,
                relX + 175,
                relY + 38
            };

            // 防越界（用 rcRelative 来判断，因为 hdcMem 的边界是 rcRelative）
            if (rcTooltip.right > rcRelative.right) {
                rcTooltip.left = relX - 175;
                rcTooltip.right = relX - 12;
            }
            if (rcTooltip.bottom > rcRelative.bottom) {
                rcTooltip.top = relY - 38;
                rcTooltip.bottom = relY - 12;
            }

            // 红色调试块（确认浮窗位置）
            HBRUSH hRed = CreateSolidBrush(RGB(255, 0, 0));
            FillRect(hdcMem, &rcTooltip, hRed);
            DeleteObject(hRed);

            // 浮窗背景（白色）
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(hdcMem, &rcTooltip, hBrush);
            DeleteObject(hBrush);

            // 浮窗边框
            HPEN hPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 210));
            SelectObject(hdcMem, hPen);
            SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
            Rectangle(hdcMem, rcTooltip.left, rcTooltip.top, rcTooltip.right, rcTooltip.bottom);
            DeleteObject(hPen);

            // 浮窗文字
            SetTextColor(hdcMem, RGB(60, 70, 80));
            SetBkMode(hdcMem, TRANSPARENT);
            char text[64];
            sprintf_s(text, "#%d  %.2fms", m_hoverIndex, m_hoverValue / 1000.0);
            TextOutA(hdcMem, rcTooltip.left + 6, rcTooltip.top + 4, text, (int)strlen(text));
        }
    }

    // ---- 3.11 贴回窗口 ----
    BitBlt(hdc, rcChart.left, rcChart.top,
        rcChart.right - rcChart.left,
        rcChart.bottom - rcChart.top,
        hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hOldBmp);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
}

// ------------------------------------------------------------------------
// 4. 信息卡片（圆角背景 + 三行指标）
// ------------------------------------------------------------------------
void MainWindow::DrawInfoCard(HDC hdc, const RECT& rcChart, const StatisticsSnapshot& s, size_t displayCount)
{
    // ---- 卡片背景 ----
    RECT card = {
        rcChart.left + 1,
        rcChart.top + 4,
        rcChart.right - 1,
        rcChart.top + 105     
    };
    HPEN hPen = CreatePen(PS_SOLID, 1, ChartTheme::CARD_SHADOW);
    HBRUSH hBrush = CreateSolidBrush(ChartTheme::CARD_BG);
    SelectObject(hdc, hPen);
    SelectObject(hdc, hBrush);
    RoundRect(hdc, card.left, card.top, card.right, card.bottom, 8, 8);
    DeleteObject(hPen);
    DeleteObject(hBrush);

    SetTextColor(hdc, ChartTheme::TEXT_PRIMARY);
    SetBkMode(hdc, TRANSPARENT);

    // ---- 三列布局 ----
    int col1 = card.left + 16;
    int col2 = card.left + 190;
    int col3 = card.left + 360;
    int col4 = card.left + 520;

    int row1 = card.top + 5;
    int row2 = card.top + 26;
    int row3 = card.top + 44;
    int row4 = card.top + 62;   
    int row5 = card.top + 80;  

    //先画标题
    HFONT hTitleFont = (HFONT)SelectObject(hdc, m_hLargeFont);
    TextOutA(hdc, col1, row1, "Latency", 8);
    TextOutA(hdc, col2, row1, "Win Rate", 9);
    TextOutA(hdc, col3, row1, "Advantage", 10);
    TextOutA(hdc, col4, row1, "Samples", 7);
    SelectObject(hdc, hTitleFont);

    //再画信息
    HFONT hOldFont = (HFONT)SelectObject(hdc, m_hSmallFont);
    // 列1: Latency
    char buf[64];
    sprintf_s(buf, "AVG  %.2fms", s.avgLatencyUs / 1000.0);
    TextOutA(hdc, col1, row2, buf, (int)strlen(buf));
    sprintf_s(buf, "P50  %.2fms", s.p50LatencyUs / 1000.0);
    TextOutA(hdc, col1, row3, buf, (int)strlen(buf));
    sprintf_s(buf, "P95  %.2fms", s.p95LatencyUs / 1000.0);
    TextOutA(hdc, col1, row4, buf, (int)strlen(buf));  // ← 加一行 P95
    sprintf_s(buf, "MAX  %.2fms", s.maxLatencyUs / 1000.0);
    TextOutA(hdc, col1, row5, buf, (int)strlen(buf));  // ← 加一行 MAX

    // 列2: Win Rate
    sprintf_s(buf, "LEFT  %.1f%%", s.leftWinRate * 100);
    TextOutA(hdc, col2, row2, buf, (int)strlen(buf));
    sprintf_s(buf, "RIGHT %.1f%%", s.rightWinRate * 100);
    TextOutA(hdc, col2, row3, buf, (int)strlen(buf));
    sprintf_s(buf, "DRAW  %.1f%%", s.drawRate * 100);
    TextOutA(hdc, col2, row4, buf, (int)strlen(buf));

    // 列3: Advantage
    sprintf_s(buf, sizeof(buf), "AVG %.2fms", s.avgAdvantageUs / 1000.0);
    TextOutA(hdc, col3, row2, buf, (int)strlen(buf));
    sprintf_s(buf, sizeof(buf), "MAX %.2fms", s.maxAdvantageUs / 1000.0);
    TextOutA(hdc, col3, row3, buf, (int)strlen(buf));

    // 列4: Samples（样本信息）
    sprintf_s(buf, sizeof(buf), "Total  %llu", s.matchedCount);
    TextOutA(hdc, col4, row2, buf, (int)strlen(buf));
    sprintf_s(buf, sizeof(buf), "Display %zu", displayCount);
    TextOutA(hdc, col4, row3, buf, (int)strlen(buf));

    // ---- 恢复字体 ----
    SelectObject(hdc, hOldFont);
}
// ------------------------------------------------------------------------
// 5. 网格和 Y 轴标签（浅灰虚线 + 右侧刻度）
// ------------------------------------------------------------------------
void MainWindow::DrawGridAndLabels(HDC hdc, const RECT& rcChart,
    int left, int right, int top, int bottom, double displayMax)
{
    HPEN hGridPen = CreatePen(PS_DOT, 1, ChartTheme::GRID_LINE);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hGridPen);
    SetTextColor(hdc, ChartTheme::TEXT_SECONDARY);
    SetBkMode(hdc, TRANSPARENT);

    const int gridCount = ChartLayout().gridCount;
    for (int i = 0; i <= gridCount; ++i) {
        double ratio = static_cast<double>(i) / gridCount;
        int y = bottom - static_cast<int>(ratio * (bottom - top));

        // 网格线
        MoveToEx(hdc, left, y, nullptr);
        LineTo(hdc, right, y);

        // Y 轴数值（靠左显示）
        char text[32];
        sprintf_s(text, "%.1fms", ratio * displayMax / 1000.0);

        HFONT hOldFont = (HFONT)SelectObject(hdc, m_hSmallFont);
        TextOutA(hdc, rcChart.left +2, y - 8, text, (int)strlen(text));
        SelectObject(hdc, hOldFont);
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hGridPen);
}

// ------------------------------------------------------------------------
// 6. 数据曲线（绿 → 黄 → 红 动态变色）
// ------------------------------------------------------------------------
void MainWindow::DrawCurve(HDC hdc, int left, int right, int top, int bottom,
    const std::deque<uint64_t>& history, double displayMax)
{
    const double stepX = static_cast<double>(right - left) / (history.size() - 1);
    const double height = static_cast<double>(bottom - top);

    // 预计算所有点
    std::vector<POINT> pts;
    pts.reserve(history.size());
    for (size_t i = 0; i < history.size(); ++i) {
        double ratio = static_cast<double>(history[i]) / displayMax;
        pts.push_back({
            left + static_cast<int>(i * stepX),
            bottom - static_cast<int>(ratio * height)
            });
    }

    // 用单色笔
    HPEN hPen = CreatePen(PS_SOLID, 1, ChartTheme::CURVE_DEFAULT);
    SelectObject(hdc, hPen);

    for (size_t i = 0; i < pts.size() - 1; ++i) {
        MoveToEx(hdc, pts[i].x, pts[i].y, nullptr);
        LineTo(hdc, pts[i + 1].x, pts[i + 1].y);
    }

    DeleteObject(hPen);
}

// ------------------------------------------------------------------------
// 7. 参考线（平均值橙色实线 + P95粉红虚线 + P99紫色虚线）
// ------------------------------------------------------------------------
void MainWindow::DrawReferenceLines(HDC hdc, int left, int right, int top, int bottom,
    const StatisticsSnapshot& s, double displayMax)
{
    const double height = static_cast<double>(bottom - top);

    // ---- 7.1 平均值（橙色实线） ----
    int avgY = bottom - static_cast<int>((s.avgLatencyUs / displayMax) * height);
    HPEN hAvgPen = CreatePen(PS_SOLID, 2, ChartTheme::LINE_AVG);
    SelectObject(hdc, hAvgPen);
    MoveToEx(hdc, left, avgY, nullptr);
    LineTo(hdc, right, avgY);
    SetTextColor(hdc, ChartTheme::LINE_AVG);
    SetBkMode(hdc, TRANSPARENT);
    //TextOutA(hdc, right - 70, avgY - 10, "AVG", 3);
    DeleteObject(hAvgPen);

    //// ---- 7.2 P95（粉红虚线） ----
    //int p95Y = bottom - static_cast<int>((s.p95LatencyUs / displayMax) * height);
    //HPEN hP95Pen = CreatePen(PS_DASH, 1, ChartTheme::LINE_P95);
    //SelectObject(hdc, hP95Pen);
    //MoveToEx(hdc, left, p95Y, nullptr);
    //LineTo(hdc, right, p95Y);
    //SetTextColor(hdc, ChartTheme::LINE_P95);
    //char bufP95[32];
    //sprintf_s(bufP95, "P95 %.2fms", s.p95LatencyUs / 1000.0);
    //TextOutA(hdc, right - 100, p95Y - 10, bufP95, (int)strlen(bufP95));
    //DeleteObject(hP95Pen);

    //// ---- 7.3 P99（紫色虚线） ----
    //int p99Y = bottom - static_cast<int>((s.p99LatencyUs / displayMax) * height);
    //HPEN hP99Pen = CreatePen(PS_DASH, 1, ChartTheme::LINE_P99);
    //SelectObject(hdc, hP99Pen);
    //MoveToEx(hdc, left, p99Y, nullptr);
    //LineTo(hdc, right, p99Y);
    //SetTextColor(hdc, ChartTheme::LINE_P99);
    //char bufP99[32];
    //sprintf_s(bufP99, "P99 %.2fms", s.p99LatencyUs / 1000.0);
    //TextOutA(hdc, right - 100, p99Y - 10, bufP99, (int)strlen(bufP99));
    //DeleteObject(hP99Pen);
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

void MainWindow::CreateFonts()
{
    // 创建小号字体（9pt，Segoe UI 或 Tahoma）
    m_hSmallFont = CreateFont(
        -12,                       // 高度（负数表示像素，12px ≈ 9pt）
        0,                         // 宽度（0 自动匹配）
        0,                         // 旋转角度
        0,                         // 旋转角度
        FW_NORMAL,                 // 字重（FW_NORMAL = 正常，FW_BOLD = 粗体）
        FALSE,                     // 斜体
        FALSE,                     // 下划线
        FALSE,                     // 删除线
        DEFAULT_CHARSET,           // 字符集
        OUT_DEFAULT_PRECIS,        // 输出精度
        CLIP_DEFAULT_PRECIS,       // 裁剪精度
        DEFAULT_QUALITY,           // 质量
        DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI"                 // 字体名（Win10+ 自带，优雅）
        // 如果兼容 XP，用 "Tahoma"
    );

    // 创建大号字体（用于图表标题，可选）
    m_hLargeFont = CreateFont(
        -16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"Segoe UI"
    );
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
            CreateFonts();

            CreateControls();

            SetTimer(
                m_hWnd,
                IDT_REFRESH,
                500,
                nullptr);

            return 0;
        }

        case WM_MOUSEMOVE: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            UpdateHoverState(x, y);
            break;
        }

        case WM_MOUSELEAVE: {
            m_bShowTooltip = false;
            m_hoverIndex = -1;
            InvalidateRect(m_hWnd, NULL, FALSE);
            break;
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

            if (m_hSmallFont) {
                DeleteObject(m_hSmallFont);
                m_hSmallFont = NULL;
            }
            if (m_hLargeFont) {
                DeleteObject(m_hLargeFont);
                m_hLargeFont = NULL;
            }

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
    TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
    tme.dwFlags = TME_LEAVE;
    tme.hwndTrack = m_hWnd;
    TrackMouseEvent(&tme);

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
    constexpr int chartheight = 250;

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
        margin + topHeight + gap + chartheight,
        clientWidth - margin * 2,
        clientHeight - topHeight - gap - margin - chartheight,
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
        L"Avg:%.2fms",
        snap.avgLatencyUs / 1000.0);

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

RECT MainWindow::GetChartRect() const
{
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    RECT rcLine, rcList;
    GetWindowRect(m_ui.hLeftGroup, &rcLine);
    GetWindowRect(m_ui.hEventList, &rcList);
    MapWindowPoints(HWND_DESKTOP, m_hWnd, (LPPOINT)&rcLine, 2);
    MapWindowPoints(HWND_DESKTOP, m_hWnd, (LPPOINT)&rcList, 2);

    const int MARGIN = 10;
    RECT rcChart = {
        MARGIN + 50,
        rcLine.bottom + MARGIN,
        rcClient.right - MARGIN,
        rcList.top - MARGIN
    };
    return rcChart;
}

int MainWindow::GetHoverIndex(int mouseX, int mouseY) const
{
    const auto& history = m_statistics->LatencyChartData();
    if (history.size() < 2) return -1;

    RECT rcChart = GetChartRect();
    int left = rcChart.left + 10;
    int right = rcChart.right - 10;
    int width = right - left;

    if (mouseX < left || mouseX > right) return -1;
    if (mouseY < rcChart.top + 65 || mouseY > rcChart.bottom - 15) return -1;

    double ratio = (double)(mouseX - left) / width;
    int index = (int)(ratio * (history.size() - 1));

    if (index < 0) index = 0;
    if (index >= (int)history.size()) index = (int)history.size() - 1;

    return index;
}

void MainWindow::UpdateHoverState(int mouseX, int mouseY)
{
    RECT rcChart = GetChartRect();
    const auto& history = m_statistics->LatencyChartData();

    bool bPrevState = m_bShowTooltip;

    if (PtInRect(&rcChart, { mouseX, mouseY }) && history.size() >= 2) {
        int index = GetHoverIndex(mouseX, mouseY);
        if (index >= 0 && index < (int)history.size()) {
            m_hoverIndex = index;
            m_hoverValue = history[index];
            m_ptHover = { mouseX, mouseY };
            m_bShowTooltip = true;
        }
        else {
            m_bShowTooltip = false;
        }
    }
    else {
        m_bShowTooltip = false;
    }

    // 只有状态变化时才重绘（减少无效刷新）
    if (bPrevState != m_bShowTooltip || m_bShowTooltip) {
        RECT rcChartArea = GetChartRect();
        InvalidateRect(m_hWnd, &rcChartArea, FALSE);
    }
}