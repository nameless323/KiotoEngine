//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>

#include "AssetsSystem/Asset.h"
#include "AssetsSystem/AssetsSystem.h"
#include "Render/RendererPublic.h"
#include "Render/DX12/Shader/ShaderParser.h"

namespace Kioto::Renderer
{
class Shader : public Asset
{
public:
    Shader() : Asset("") {}
    Shader(const std::string& path) : Asset(path) 
    {
        m_shader = AssetsSystem::ReadFileAsString(path);
    }

    void SetHandle(ShaderHandle handle);
    ShaderHandle GetHandle() const;
    const ShaderData& GetShaderData() const;
    void SetShaderData(const ShaderData& data);
    const std::string& GetShaderString() const;

private:
    std::string m_shader;
    ShaderProgramHandle m_vsHandle;
    ShaderProgramHandle m_psHandle;
    VertexLayoutHandle m_vertexLayout;
    ShaderData m_data;
    ShaderHandle m_handle; // [a_vorontsov] Separate handles for each define set?

    friend class Material;
};

inline void Shader::SetHandle(ShaderHandle handle)
{
    m_handle = handle;
}

inline ShaderHandle Shader::GetHandle() const
{
    return m_handle;
}

inline void Shader::SetShaderData(const ShaderData& data)
{
    m_data = data;
}

inline const ShaderData& Shader::GetShaderData() const
{
    return m_data;
}

inline const std::string& Shader::GetShaderString() const
{
    return m_shader;
}
}