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

    if (config["passes"])
    {
        YAML::Node characterType = config["passes"];
        for (YAML::const_iterator it = characterType.begin(); it != characterType.end(); ++it)
        {
            DeserializeRenderPassConfig(it->second);
        }
    }
    else
        assert(false);
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

void Material::DeserializeRenderPassConfig(const YAML::Node& pass)
{
    assert(pass["name"]);
    std::string passName = pass["name"].as<std::string>();

    assert(pass["pipelineConfig"]);
    std::string pipelineConfigPath = AssetsSystem::GetAssetFullPath(pass["pipelineConfig"].as<std::string>());
    PipelineState state = PipelineState::FromYaml(pipelineConfigPath);

    assert(pass["shader"]);
    std::string shaderPath = AssetsSystem::GetAssetFullPath(pass["shader"].as<std::string>());
    state.Shader = AssetsSystem::GetRenderAssetsManager()->GetOrLoadAsset<Shader>(shaderPath);

    std::vector<TextureAssetDescription> texDescriptions;

    if (pass["textures"])
    {
        YAML::Node texNodes = pass["textures"];
        auto it = texNodes.begin();
        std::string texName = it->first.as<std::string>();
        uint16 texOffset = state.Shader->GetShaderData().textureSet.GetTextureOffset(texName);
        assert(texOffset != -1);
        for (; it != texNodes.end(); ++it)
            texDescriptions.emplace_back(TextureAssetDescription{ std::move(texName), it->second.as<std::string>(), texOffset });
    }
    m_materialPipelineStates[passName] = std::move(state);
    m_textures[passName] = std::move(texDescriptions);
}

const PipelineState& Material::GetPipelineState(const PassName& passName) const
{
    assert(m_materialPipelineStates.count(passName) == 1);
    return m_materialPipelineStates.at(passName);
}

PipelineState& Material::GetPipelineState(const PassName& passName)
{
    assert(m_materialPipelineStates.count(passName) == 1);
    return m_materialPipelineStates.at(passName);
}

const std::unordered_map<PassName, PipelineState>& Material::GetPipelineStates() const
{
    return m_materialPipelineStates;
}

const std::vector<TextureAssetDescription>& Material::GetTextureAssetDescriptions(const PassName& passName) const
{
    assert(m_textures.count(passName) == 1);
    return m_textures.at(passName);
}

const std::unordered_map<PassName, std::vector<TextureAssetDescription>>& Material::GetTextureAssetDescriptions() const
{
    return m_textures;
}

}