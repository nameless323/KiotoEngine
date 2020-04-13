#pragma once

#include <string>

#include "AssetsSystem/Asset.h"
#include "Core/CoreTypes.h"
#include "Render/ConstantBuffer.h"
#include "Render/PipelineState.h"
#include "Render/RendererPublic.h"
#include "Render/ShaderData.h"

namespace Kioto::Renderer
{
class Shader;

class Material : public Asset
{
public:
    Material() = default;
    Material(const std::string& path);
    ~Material();

    void SetHandle(MaterialHandle handle);
    MaterialHandle GetHandle() const;

    Shader* GetShader() const;
    ShaderData& GetShaderData();
    void BuildMaterialForPass(const RenderPass* pass);

private:
    void DeserializeRenderPassConfig(const YAML::Node& pass);

    ShaderData m_shaderData; <-- remove from here, move to render object
    MaterialHandle m_handle;

    // [a_vorontcov] For each pass contains appropriate pipeline state
    std::unordered_map<std::string, PipelineState> m_materialPipelineStates;
    std::vector<RenderPassHandle> m_buildedPassesHandles;
};

inline void Material::SetHandle(MaterialHandle handle)
{
    m_handle = handle;
}

inline MaterialHandle Material::GetHandle() const
{
    return m_handle;
}

inline Shader* Material::GetShader() const
{
    return m_shader;
}

inline ShaderData& Material::GetShaderData()
{
    return m_shaderData;
}
}