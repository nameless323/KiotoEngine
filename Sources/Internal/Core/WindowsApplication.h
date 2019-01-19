//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

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