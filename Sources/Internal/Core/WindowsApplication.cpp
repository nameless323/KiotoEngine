//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Core/WindowsApplication.h"

namespace Kioto
{
HWND WindowsApplication::m_hwnd = nullptr;
std::wstring WindowsApplication::m_windowCapture = L"Kioto Project";

bool WindowsApplication::Init(HINSTANCE hInstance, int nCmdShow)
{
    // TODO: create mutex to check if another instance exist.
    WNDCLASSEX windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpszClassName = L"KiotoEngine";
    RegisterClassEx(&windowClass);

    RECT windowRect = { 0, 0, 1024, 768 };
    if (!AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE))
        return false;

    m_hwnd = CreateWindow(windowClass.lpszClassName,
        m_windowCapture.c_str(),
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
        return false;

    ShowWindow(m_hwnd, nCmdShow);
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
    }
    Shutdown();

    return static_cast<int>(msg.wParam);
}

void WindowsApplication::Shutdown()
{

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
        break;

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
}