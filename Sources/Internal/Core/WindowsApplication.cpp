//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Core/WindowsApplication.h"

#include <functional>
#include <Strsafe.h>

namespace Kioto
{
HWND WindowsApplication::m_hwnd = nullptr;
std::wstring WindowsApplication::m_windowCaption;

bool WindowsApplication::Init(HINSTANCE hInstance, int nCmdShowm, std::wstring caption)
{
    // TODO: create mutex to check if another instance exist.
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

int WindowsApplication::Run()
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

void WindowsApplication::Shutdown()
{
    KiotoCore::Shutdown();
}

LRESULT WindowsApplication::WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
        return 0;

    case WM_PAINT:
        return 0;

    case WM_SIZE:
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

void WindowsApplication::MessageError(LPTSTR lpszFunction)
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