#pragma once

#include <string>

#include "AssetsSystem/Asset.h"
#include "AssetsSystem/FilesystemHelpers.h"
#include "Render/RendererPublic.h"
#include "Render/ShaderData.h"

namespace Kioto::Renderer
{
class Shader : public Asset
{
public:
    Shader() : Asset("") {}
    Shader(const std::string& path) : Asset(path) 
    {
    }

    void SetHandle(ShaderHandle handle);
    ShaderHandle GetHandle() const;

    const ShaderData& GetShaderData() const;
    void SetShaderData(const ShaderData& data);

    const RenderObjectBufferLayout& GetBufferLayoutTemplate() const;
    void SetBufferLayoutTemplate(const RenderObjectBufferLayout& layoutTemplate);
    RenderObjectBufferLayout CreateLayoutTemplateShalowCopy() const;

private:
    ShaderProgramHandle m_vsHandle;
    ShaderProgramHandle m_psHandle;
    VertexLayoutHandle m_vertexLayout;
    ShaderData m_data;
    RenderObjectBufferLayout m_bufferLayoutTemplate;
    ShaderHandle m_handle; // [a_vorontcov] Separate handles for each define set?

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

inline const RenderObjectBufferLayout& Shader::GetBufferLayoutTemplate() const
{
    return m_bufferLayoutTemplate;
}

inline void Shader::SetBufferLayoutTemplate(const RenderObjectBufferLayout& layoutTemplate)
{
    m_bufferLayoutTemplate = layoutTemplate;
}

inline RenderObjectBufferLayout Shader::CreateLayoutTemplateShalowCopy() const
{
    RenderObjectBufferLayout res;
    res.resize(m_bufferLayoutTemplate.size());
    for (size_t i = 0; i < m_bufferLayoutTemplate.size(); ++i)
        m_bufferLayoutTemplate[i].MakeShallowCopy(res[i]);
    return res;
}

}