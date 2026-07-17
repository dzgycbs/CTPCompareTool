#pragma once

#include "Common.h"
#include "StatisticsSnapshot.h"

class Statistics;
//=============================================================================
// 控件ID
//=============================================================================

#define IDT_REFRESH        1

#define IDC_STATUSBAR      1001
#define IDC_EVENTLIST      1002

#define IDC_LEFT_GROUP     1100
#define IDC_RIGHT_GROUP    1200

//=============================================================================
// UI控件
//=============================================================================

struct UIControls
{
    HWND hStatusBar = nullptr;
    HWND hEventList = nullptr;

    HWND hLeftGroup = nullptr;
    HWND hRightGroup = nullptr;
};

//=============================================================================
// MainWindow
//=============================================================================

class MainWindow
{
public:

    MainWindow();
    ~MainWindow();

public:

    bool Create(HINSTANCE hInstance);

    void Show(int nCmdShow);

    HWND GetHwnd() const
    {
        return m_hWnd;
    }

    void DrawEmptyState(HDC hdc, RECT& rcChart);
    void DrawLatencyChart(HDC hdc);
    void DrawInfoCard(HDC hdc, const RECT& rcChart, const StatisticsSnapshot& s, size_t displayCount);
    void DrawGridAndLabels(HDC hdc, const RECT& rcChart,int left, int right,
        int top, int bottom, double displayMax);
    void DrawCurve(HDC hdc, int left, int right, int top, int bottom,
        const std::deque<uint64_t>& history, double displayMax);
    void DrawReferenceLines(HDC hdc, int left, int right, int top, int bottom,
            const StatisticsSnapshot& s, double displayMax);

    void SetStatistics(Statistics* statistics);

private:

    static LRESULT CALLBACK WndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam);

    LRESULT HandleMessage(
        UINT message,
        WPARAM wParam,
        LPARAM lParam);

private:

    bool RegisterWindowClass();

    void CreateFonts();

    void CreateControls();

    void CreateStatusBar();

    void CreateEventList();

    void LayoutControls();

    void UpdateControls();

private:

    HINSTANCE m_hInstance = nullptr;

    HWND m_hWnd = nullptr;

    UIControls m_ui;

    Statistics* m_statistics = nullptr;

    HFONT m_hSmallFont;      // 小号字体（信息卡、标签）

    HFONT m_hLargeFont;      // 大号字体（标题，可选）

private:

    POINT m_ptHover = { -1, -1 };        // 鼠标悬停位置（-1表示无悬停）

    int m_hoverIndex = -1;              // 悬停对应的数据点索引

    uint64_t m_hoverValue = 0;          // 悬停点的数值

    bool m_bShowTooltip = false;        // 是否显示浮窗

    // 辅助函数
    RECT GetChartRect() const;

    int GetHoverIndex(int mouseX, int mouseY) const;

    void UpdateHoverState(int mouseX, int mouseY);
};