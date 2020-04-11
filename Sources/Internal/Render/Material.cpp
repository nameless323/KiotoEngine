#include "stdafx.h"

#include "Render/Material.h"

#include "yaml-cpp/yaml.h"

#include "AssetsSystem/AssetsSystem.h"
#include "AssetsSystem/RenderStateParamsConverter.h"
#include "Render/Renderer.h"
#include "Render/RenderPass/RenderPass.h"
#include "Render/Shader.h"

namespace Kioto::Renderer
{
static const float32 CurrentVersion = 0.01f;

Material::Material(const std::string& path)
    : Asset(path)
{
    m_buildedPassesHandles.reserve(32);

    using namespace RenderParamsConverter;
    if (!AssetsSystem::CheckIfFileExist(path))
        throw "Material not exist";

    YAML::Node config = YAML::LoadFile(path);
    float32 version = -1.0f;

    if (config["version"])
        version = config["version"].as<float32>();
    if (config["shader"])
    {
        m_shaderPath = config["shader"].as<std::string>();
        std::string shaderPath = AssetsSystem::GetAssetFullPath(m_shaderPath);
        m_shader = AssetsSystem::GetRenderAssetsManager()->GetOrLoadAsset<Shader>(shaderPath);
    }
    else
    {
        throw "Wtf";
    }
    m_shaderData = m_shader->m_data; // [a_vorontcov] TODO:: Think if copy of shader data is necesary here
    if (m_shaderData.textureSet.GetTexturesCount() > 0)
        Renderer::RegisterTextureSet(m_shaderData.textureSet);

    PipelineState::Append(config, m_shaderData.pipelineState);
    if (config["textures"])
    {
        YAML::Node texNodes = config["textures"];
        auto it = texNodes.begin();
        for (; it != texNodes.end(); ++it)
        {
            std::string name = it->first.as<std::string>();
            std::string path = it->second.as<std::string>();
            std::string fullPath = AssetsSystem::GetAssetFullPath(path);
            Texture* tex = AssetsSystem::GetRenderAssetsManager()->GetOrLoadAsset<Texture>(fullPath);
            m_shaderData.textureSet.SetTexture(name, tex);
        }
    }
}

Material::~Material()
{
}

void Material::BuildMaterialForPass(const RenderPass* pass)
{
    auto it = std::find(m_buildedPassesHandles.cbegin(), m_buildedPassesHandles.cend(), pass->GetHandle());
    if (it != m_buildedPassesHandles.cend())
        return;
    Renderer::BuildMaterialForPass(*this, pass);
    m_buildedPassesHandles.push_back(pass->GetHandle());
}
}