//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include <sstream>

#include "Core/KiotoEngine.h"
#include "Core/WindowsApplication.h"

namespace Kioto
{

KIOTO_API void KiotoMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nCmdShow, std::wstring capture)
{
    KiotoCore::ApplicationInfo.HInstance = hInstance;
    KiotoCore::ApplicationInfo.PrevInstance = prevInstance;
    KiotoCore::ApplicationInfo.CmdLine = cmdLine;
    KiotoCore::ApplicationInfo.NCmdShow = nCmdShow;
    KiotoCore::ApplicationInfo.WindowCapture = capture;

    KiotoCore::Init();
}

namespace KiotoCore
{
void Init()
{
    std::stringstream ss;
    WindowsApplication::Init(ApplicationInfo.HInstance, ApplicationInfo.NCmdShow, ApplicationInfo.WindowCapture);
    WindowsApplication::Run();
    OutputDebugStringA(ss.str().c_str());
}

void Update()
{
    int i = 0;
    i++;
}

void Shutdown()
{
    int j = 0;
    j++;
}

void ChangeFullscreenMode(bool fullScreen)
{
    WindowsApplication::ChangeFullscreenMode(fullScreen);
}

}
}