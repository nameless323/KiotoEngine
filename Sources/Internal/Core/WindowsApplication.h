#pragma once

#include <windows.h>
#include <string>

namespace Kioto
{

class WindowsApplication
{
public:
    WindowsApplication() = delete;
    WindowsApplication(const WindowsApplication&) = delete;
    WindowsApplication& operator= (const WindowsApplication&) = delete;

    static bool Init(HINSTANCE hInstance, int nCmdShow);

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    static HWND m_hwnd;
    static const UINT m_windowStyle = WS_OVERLAPPEDWINDOW;
    static RECT m_windowRect;
    static std::wstring m_windowCapture;
};

}