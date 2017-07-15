//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Core/WindowsApplication.h"

#include <functional>
#include <Strsafe.h>

#include "Core/CoreTypes.h"

namespace Kioto
{

namespace KiotoCore
{
void Init();
void Update();
void Shutdown();
void ChangeFullscreenMode(bool fullScreen);
void Resize(uint16 width, uint16 height, bool minimized);
}

namespace WindowsApplication
{

namespace
{
const UINT m_windowStyle = WS_OVERLAPPEDWINDOW;
RECT m_windowRect = {};
std::wstring m_windowCaption;
bool m_isFullscreen = false;
HWND m_hwnd;
}

void MessageError(LPTSTR lpszFunction);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

bool Init(HINSTANCE hInstance, int nCmdShowm, std::wstring caption)
{
    // TODO: create mutex to check if another instance exist.
    m_isFullscreen = false;
    m_windowCaption = caption;

    WNDCLASSEX windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpszClassName = caption.c_str();
    RegisterClassEx(&windowClass);

    RECT windowRect = { 0, 0, 1024, 768 };
    if (!AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE))
        return false;

    m_hwnd = CreateWindow(windowClass.lpszClassName,
        m_windowCaption.c_str(),
        m_windowStyle,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (m_hwnd == nullptr)
    {
        MessageError(TEXT("Create window"));
        return false;
    }

    ShowWindow(m_hwnd, nCmdShowm);

    return true;
}

int Run()
{
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            KiotoCore::Update();
        }
    }
    Shutdown();

    return static_cast<int>(msg.wParam);
}

void Shutdown()
{
    KiotoCore::Shutdown();
}

void ChangeFullscreenMode(bool fullScreen)
{
    if (fullScreen == m_isFullscreen)
        return;

    m_isFullscreen = fullScreen;
    if (!m_isFullscreen)
    {
        SetWindowLong(m_hwnd, GWL_STYLE, m_windowStyle);

        SetWindowPos(
            m_hwnd,
            HWND_NOTOPMOST,
            m_windowRect.left,
            m_windowRect.top,
            m_windowRect.right - m_windowRect.left,
            m_windowRect.bottom - m_windowRect.top,
            SWP_FRAMECHANGED | SWP_NOACTIVATE);

        ShowWindow(m_hwnd, SW_NORMAL);
        return;
    }
    GetWindowRect(m_hwnd, &m_windowRect);

    UINT fullScreenWindowStyle = m_windowStyle & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME);
    SetWindowLong(m_hwnd, GWL_STYLE, fullScreenWindowStyle);

    DEVMODE devMode = {};
    devMode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);

    SetWindowPos(
        m_hwnd,
        HWND_TOPMOST,
        devMode.dmPosition.x,
        devMode.dmPosition.y,
        devMode.dmPosition.x + devMode.dmPelsWidth,
        devMode.dmPosition.y + devMode.dmPelsHeight,
        SWP_FRAMECHANGED | SWP_NOACTIVATE);

    ShowWindow(m_hwnd, SW_MAXIMIZE);
}

HWND GetHWND()
{
    return m_hwnd;
}

LRESULT WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
    }
    return 0;

    case WM_KEYDOWN:
        return 0;

    case WM_KEYUP:
        return 0;

    case WM_SYSKEYDOWN:
        if ((wParam == VK_RETURN) && (lParam & (1 << 29))) // [a_vorontsov] Handle Alt+Enter.
        {
            KiotoCore::ChangeFullscreenMode(!m_isFullscreen);
        }
        break;


    case WM_PAINT:
        return 0;

    case WM_EXITSIZEMOVE:
    {
        RECT clientRect = {};
        GetClientRect(m_hwnd, &clientRect);
        KiotoCore::Resize(static_cast<uint16>(clientRect.right - clientRect.left), static_cast<uint16>(clientRect.bottom - clientRect.top), wParam == SIZE_MINIMIZED);
    }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

void MessageError(LPTSTR lpszFunction)
{
    LPVOID messageBuffer;
    LPVOID displayBuffer;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&messageBuffer,
        0, nullptr);

    displayBuffer = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)messageBuffer) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)displayBuffer,
        LocalSize(displayBuffer) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, messageBuffer);
    MessageBox(nullptr, (LPCTSTR)displayBuffer, TEXT("Error"), MB_OK);

    LocalFree(messageBuffer);
    LocalFree(displayBuffer);
}

}
}