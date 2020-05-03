//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include <sstream>

#include "AssetsSystem/AssetsSystem.h"
#include "Core/FPSCounter.h"
#include "Core/Input/Input.h"
#include "Core/KiotoEngine.h"
#include "Core/Scene.h"
#include "Core/Timer/GlobalTimer.h"
#include "Core/WindowsApplication.h"

#include "Render/Buffers/EngineBuffers.h"
#include "Render/Geometry/GeometryGenerator.h"
#include "Render/Geometry/MeshLoader.h"
#include "Render/Geometry/ParserFBX.h"
#include "Render/Material.h"
#include "Render/Renderer.h"
#include "Render/RenderOptions.h"

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace Kioto
{
Scene* m_scene = nullptr;
std::function<void()> InitEngineCallback = nullptr;
std::function<void()> ShutdownEngineCallback = nullptr;

namespace KiotoCore
{
ApplicationInfoData ApplicationInfo;
Kioto::RenderOptions RenderSettings;
}

void KiotoMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nCmdShow, std::wstring capture, std::function<void()> initEngineCallback, std::function<void()> shutdownEngineCallback)
{
    KiotoCore::ApplicationInfo.HInstance = hInstance;
    KiotoCore::ApplicationInfo.PrevInstance = prevInstance;
    KiotoCore::ApplicationInfo.CmdLine = cmdLine;
    KiotoCore::ApplicationInfo.NCmdShow = nCmdShow;
    KiotoCore::ApplicationInfo.WindowCapture = capture;

    InitEngineCallback = initEngineCallback;
    ShutdownEngineCallback = shutdownEngineCallback;

    KiotoCore::Init();
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

void SaveScene(std::string path)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Version" << YAML::Value << 0.09f;
    out << YAML::Key << "Scene";
    out << YAML::Value << YAML::BeginMap;
    m_scene->Serialize(out);

    out << YAML::EndMap;
    out << YAML::EndMap;
    std::fstream fstream;
    fstream.open(path, std::fstream::out | std::fstream::trunc);
    fstream << out.c_str();
    fstream.close();
}

void LoadScene(std::string path)
{
    YAML::Node config = YAML::LoadFile(path);
    float32 version = -1.0f;

    if (config["Version"])
        version = config["Version"].as<float32>();

    if (config["Scene"])
    {
        YAML::Node sceneNode = config["Scene"];
        if (sceneNode["SceneName"])
        {
            Scene* scene = new Scene(sceneNode["SceneName"].as<std::string>());
            SetScene(scene);
            scene->Deserialize(sceneNode);
        }
    }
}

Scene* GetScene()
{
    return m_scene;
}

namespace KiotoCore
{
void Init()
{
    GlobalTimer::Init();
    AssetsSystem::Init();
    MeshLoader::Init();
    Renderer::GeometryGenerator::Init();
    WindowsApplication::Init(ApplicationInfo.HInstance, ApplicationInfo.NCmdShow, ApplicationInfo.WindowCapture);
    Renderer::EngineBuffers::Init();
    Renderer::Init(Renderer::eRenderApi::DirectX12, RenderSettings.Resolution.x, RenderSettings.Resolution.y);

    Renderer::GeometryGenerator::RegisterGeometry();

    if (InitEngineCallback != nullptr)
        InitEngineCallback();

    WindowsApplication::Run();
}

void Update()
{
    Input::Update();
    GlobalTimer::Tick();
    FPSCounter::Tick(GlobalTimer::GetDeltaTime());
    Renderer::StartFrame();
    if (m_scene != nullptr)
        m_scene->Update(GlobalTimer::GetDeltaTime());
    Renderer::Update(GlobalTimer::GetDeltaTime());
    Renderer::Present();
}

void Shutdown()
{
    if (ShutdownEngineCallback != nullptr)
        ShutdownEngineCallback();

    Renderer::Shutdown();
    SafeDelete(m_scene);
    Renderer::GeometryGenerator::Shutdown();
    MeshLoader::Shutdown();
    AssetsSystem::Shutdown();
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

Kioto::RenderOptions& GetRenderSettings()
{
    return RenderSettings;
}
}
}