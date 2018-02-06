//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/Material.h"

#include "yaml-cpp/yaml.h"

#include "AssetsSystem/AssetsSystem.h"
#include "AssetsSystem/RenderStateParamsConverter.h"

namespace Kioto
{
static const float32 CurrentVersion = 0.01f;

Material::Material(const std::string& path)
    : Asset(path)
{
    using namespace RenderParamsConverter;
    if (!AssetsSystem::CheckIfFileExist(path))
        throw "Material not exist";

    YAML::Node config = YAML::LoadFile(path);
    float32 version = -1.0f;
    if (config["version"] != nullptr)
        version = config["version"].as<float32>();
    if (config["shader"] != nullptr)
    {
        m_shaderPath = config["shader"].as<std::string>();
        m_shader = AssetsSystem::LoadAsset<Renderer::Shader>(m_shaderPath);
    }
    m_pipelineState = Renderer::PipelineState::FromYaml(config);
}
}