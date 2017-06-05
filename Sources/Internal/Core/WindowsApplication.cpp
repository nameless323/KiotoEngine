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
        //            if (pSample)
        //            {
        //                pSample->OnKeyDown(static_cast<UINT8>(wParam));
        //            }
        return 0;

    case WM_KEYUP:
        //            if (pSample)
        //            {
        //                pSample->OnKeyUp(static_cast<UINT8>(wParam));
        //            }
        return 0;

    case WM_SYSKEYDOWN:
        // Handle ALT+ENTER:
        //            if ((wParam == VK_RETURN) && (lParam & (1 << 29)))
        //            {
        //                if (pSample && pSample->GetTearingSupport())
        //                {
        //                    ToggleFullscreen();
        //                    return 0;
        //                }
        //            }
        // Send all other WM_SYSKEYDOWN messages to the default WndProc.
        break;

    case WM_PAINT:
        //            if (pSample)
        //            {
        //                pSample->OnUpdate();
        //                pSample->OnRender();
        //            }
        return 0;

    case WM_SIZE:
        //            if (pSample)
        //            {
        //                RECT clientRect = {};
        //                GetClientRect(hWnd, &clientRect);
        //                pSample->OnSizeChanged(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, wParam == SIZE_MINIMIZED);
        //            }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    // Handle any messages the switch statement didn't.
    return DefWindowProc(hwnd, message, wParam, lParam);
}

}