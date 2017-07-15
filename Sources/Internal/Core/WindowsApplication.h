//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>
#include <windows.h>

namespace Kioto
{
namespace WindowsApplication
{

    bool Init(HINSTANCE hInstance, int nCmdShowm, std::wstring caption);
    int Run();
    void Shutdown();
    void ChangeFullscreenMode(bool fullScreen);

    HWND GetHWND();

}
}