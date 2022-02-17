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
    const RenderObjectConstants& GetRenderObjectConstants() const;
    void SetRenderObjectConstants(const RenderObjectConstants& roConstants);

    RenderObjectBufferLayout CreateLayoutTemplateShalowCopy() const;

private:
    ShaderProgramHandle mVsHandle;
    ShaderProgramHandle mPsHandle;
    VertexLayoutHandle mVertexLayout;
    ShaderData mData;
    RenderObjectBufferLayout mBufferLayoutTemplate;
    RenderObjectConstants mRootConstants;

    ShaderHandle mHandle; // [a_vorontcov] Separate handles for each define set?

    friend class Material;
};

inline void Shader::SetHandle(ShaderHandle handle)
{
    mHandle = handle;
}

inline ShaderHandle Shader::GetHandle() const
{
    return mHandle;
}

inline void Shader::SetShaderData(const ShaderData& data)
{
    mData = data;
}

inline const ShaderData& Shader::GetShaderData() const
{
    return mData;
}

inline const RenderObjectBufferLayout& Shader::GetBufferLayoutTemplate() const
{
    return mBufferLayoutTemplate;
}

inline void Shader::SetBufferLayoutTemplate(const RenderObjectBufferLayout& layoutTemplate)
{
    mBufferLayoutTemplate = layoutTemplate;
}

inline RenderObjectBufferLayout Shader::CreateLayoutTemplateShalowCopy() const
{
    RenderObjectBufferLayout res;
    res.resize(mBufferLayoutTemplate.size());
    for (size_t i = 0; i < mBufferLayoutTemplate.size(); ++i)
        mBufferLayoutTemplate[i].MakeShallowCopy(res[i]);
    return res;
}

inline const RenderObjectConstants& Shader::GetRenderObjectConstants() const
{
    return mRootConstants;
}

inline void Shader::SetRenderObjectConstants(const RenderObjectConstants& roConstants)
{
    mRootConstants = roConstants;
}

}