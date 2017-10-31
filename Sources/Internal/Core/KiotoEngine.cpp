//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include <sstream>

#include "AssetsSystem/AssetsSystem.h"
#include "Core/FPSCounter.h"
#include "Core/KiotoEngine.h"
#include "Core/Scene.h"
#include "Core/Timer/GlobalTimer.h"
#include "Core/WindowsApplication.h"
#include "Render/Renderer.h"

namespace Kioto
{
Scene* m_scene = nullptr;

void KiotoMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nCmdShow, std::wstring capture, std::function<void()> initEngineCallback)
{
    KiotoCore::ApplicationInfo.HInstance = hInstance;
    KiotoCore::ApplicationInfo.PrevInstance = prevInstance;
    KiotoCore::ApplicationInfo.CmdLine = cmdLine;
    KiotoCore::ApplicationInfo.NCmdShow = nCmdShow;
    KiotoCore::ApplicationInfo.WindowCapture = capture;

    KiotoCore::Init(initEngineCallback);
}

void SetScene(Scene* scene)
{
    if (m_scene != nullptr)
    {
        m_scene->Shutdown();
        SafeDelete(m_scene);
    }
    m_scene = scene;
    m_scene->Init();
}

namespace KiotoCore
{
void Init(std::function<void()> initEngineCallback)
{
    AssetsSystem::Init();
    GlobalTimer::Init();
    WindowsApplication::Init(ApplicationInfo.HInstance, ApplicationInfo.NCmdShow, ApplicationInfo.WindowCapture);
    Renderer::Init(Renderer::eRenderApi::DirectX12, 1024, 768);

    if (initEngineCallback != nullptr)
        initEngineCallback();

    WindowsApplication::Run();
}

void Update()
{
    GlobalTimer::Tick();
    FPSCounter::Tick(GlobalTimer::GetDeltaTime());
    if (m_scene != nullptr)
        m_scene->Update(GlobalTimer::GetDeltaTime());
    Renderer::Update(GlobalTimer::GetDeltaTime());
    Renderer::Present();
}

void Shutdown()
{
    Renderer::Shutdown();
}

void ChangeFullscreenMode(bool fullScreen)
{
    WindowsApplication::ChangeFullscreenMode(fullScreen);
    Renderer::ChangeFullScreenMode(fullScreen);
}

void Resize(uint16 width, uint16 height, bool minimized)
{
    Renderer::Resize(width, height, minimized);
}

}
}