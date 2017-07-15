//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include <sstream>

#include "Core/KiotoEngine.h"
#include "Core/WindowsApplication.h"
#include "Render/Renderer.h"

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
    Renderer::Init(Renderer::eRenderApi::DirectX12);

    WindowsApplication::Run();
    OutputDebugStringA(ss.str().c_str());
}

void Update()
{
}

void Shutdown()
{
    Renderer::Shutdown();
}

void ChangeFullscreenMode(bool fullScreen)
{
    WindowsApplication::ChangeFullscreenMode(fullScreen);
    Renderer::ChangeFullscreenMode(fullScreen);
}

void Resize(uint16 width, uint16 height, bool minimized)
{
    Renderer::Resize(width, height, minimized);
}

}
}