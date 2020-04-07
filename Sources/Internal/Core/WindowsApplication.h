#pragma once

#include <string>
#include <windows.h>

#include "Core/CoreTypes.h"

namespace Kioto::WindowsApplication
{
    bool Init(HINSTANCE hInstance, int32 nCmdShow, std::wstring caption);
    int64 Run();
    void Shutdown();
    void ChangeFullscreenMode(bool fullScreen);

    HWND GetHWND();
}