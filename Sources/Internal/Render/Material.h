//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>

#include "Render/Texture/TextureSet.h"
#include "Render/ConstantBuffer.h"
#include "AssetsSystem/Asset.h"
#include "Core/CoreTypes.h"
#include "Render/PipelineState.h"
#include "Render/Shader.h"
#include "Render/ShaderData.h"
#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
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

    template<typename T>
    ConstantBuffer::eReturnCode SetValueToBuffer(const std::string& name, T&& val)
    {
        ConstantBuffer::eReturnCode retCode = ConstantBuffer::eReturnCode::NotFound;
        for (auto& cb : m_shaderData.constantBuffers)
        {
            auto code = cb.Set(name, std::forward(val));
            if (code == ConstantBuffer::eReturnCode::Ok)
                retCode = ConstantBuffer::eReturnCode::Ok;
        }
        return retCode;
    }

private:
    std::string m_shaderPath;
    Shader* m_shader = nullptr;
    ShaderData m_shaderData;
    MaterialHandle m_handle;

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