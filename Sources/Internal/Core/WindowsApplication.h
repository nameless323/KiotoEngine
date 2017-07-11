//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>
#include <windows.h>

namespace Kioto
{
namespace KiotoCore
{
void Init();
void Update();
void Shutdown();
}

class WindowsApplication
{
public:
    WindowsApplication() = delete;
    WindowsApplication(const WindowsApplication&) = delete;
    WindowsApplication& operator= (const WindowsApplication&) = delete;

    static bool Init(HINSTANCE hInstance, int nCmdShowm, std::wstring caption);
    static int Run();
    static void Shutdown();
    static void MessageError(LPTSTR lpszFunction);

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    static HWND m_hwnd;
    static const UINT m_windowStyle = WS_OVERLAPPEDWINDOW;
    static RECT m_windowRect;
    static std::wstring m_windowCaption;
};
}