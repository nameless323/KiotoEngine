//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>
#include <windows.h>

namespace Kioto
{
class WindowsApplication
{
public:
    WindowsApplication() = delete;
    WindowsApplication(const WindowsApplication&) = delete;
    WindowsApplication& operator= (const WindowsApplication&) = delete;

    static bool Init(HINSTANCE hInstance, int nCmdShowm, std::wstring caption);
    static int Run();
    static void Shutdown();
    static void ChangeFullscreenMode(bool fullScreen);

    static HWND GetHWND();

private:
    static void MessageError(LPTSTR lpszFunction);
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    static HWND m_hwnd;
};

inline HWND WindowsApplication::GetHWND()
{
    return m_hwnd;
}
}