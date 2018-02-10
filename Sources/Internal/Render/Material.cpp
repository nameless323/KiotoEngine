//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/Material.h"

#include "yaml-cpp/yaml.h"

#include "AssetsSystem/AssetsSystem.h"
#include "AssetsSystem/RenderStateParamsConverter.h"
#include "Render/Texture/RenderAssetsManager.h"

namespace Kioto::Renderer
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
        std::string shaderPath = AssetsSystem::GetAssetFullPath(m_shaderPath);
        m_shader = AssetsSystem::LoadAsset<Shader>(shaderPath);
    }
    m_pipelineState = m_shader->m_data.pipelineState;
    PipelineState::Append(config, m_pipelineState);
    if (config["textures"] != nullptr)
    {
        YAML::Node texNodes = config["textures"];
        auto it = texNodes.begin();
        for (; it != texNodes.end(); ++it)
        {
            std::string name = it->first.as<std::string>();
            std::string path = it->second.as<std::string>();
            std::string fullPath = AssetsSystem::GetAssetFullPath(path);
            Texture* tex = AssetsSystem::GetRenderAssetsManager<Texture>()->GetOrLoadAsset(fullPath);
            m_shader->m_data.textureSet.SetTexture(name, tex);
        }
    }
}

Material::~Material()
{
}

}