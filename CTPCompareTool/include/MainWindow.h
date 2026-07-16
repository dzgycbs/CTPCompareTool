#pragma once

#include "Common.h"

class Statistics;
//=============================================================================
// ¿Ø¼þID
//=============================================================================

#define IDT_REFRESH        1

#define IDC_STATUSBAR      1001
#define IDC_EVENTLIST      1002

#define IDC_LEFT_GROUP     1100
#define IDC_RIGHT_GROUP    1200

//=============================================================================
// UI¿Ø¼þ
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

    void DrawLatencyChart(HDC hdc);

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


};