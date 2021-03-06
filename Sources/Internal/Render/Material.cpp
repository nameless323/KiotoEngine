#include "stdafx.h"

#include "Render/Material.h"

#include "yaml-cpp/yaml.h"

#include "AssetsSystem/AssetsSystem.h"
#include "AssetsSystem/FilesystemHelpers.h"
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
    if (!FilesystemHelpers::CheckIfFileExist(path))
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

    const TextureSet* texSet = &state.Shader->GetShaderData().textureSet;
    for (uint32 i = 0; i < texSet->GetTexturesCount(); ++i)
    {
        texDescriptions.emplace_back(TextureAssetDescription{ *state.Shader->GetShaderData().textureSet.GetTextureName(i), "", state.Shader->GetShaderData().textureSet.GetTextureOffset(i) });
    }

    if (pass["textures"])
    {
        YAML::Node texNodes = pass["textures"];
        auto it = texNodes.begin();
        for (; it != texNodes.end(); ++it)
        {
            std::string texName = it->first.as<std::string>();
            auto texIter = std::find_if(texDescriptions.begin(), texDescriptions.end(), [&texName](const TextureAssetDescription& d)
                {
                    return d.Name == texName;
                });
            assert(texIter != texDescriptions.end());
            std::string texPath = it->second.as<std::string>();
            texIter->Path = std::move(texPath);
        }
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