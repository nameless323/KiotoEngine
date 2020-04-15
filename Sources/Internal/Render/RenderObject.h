#pragma once

#include "Render/ShaderData.h"

// [a_vorontcov] TODO: Split to h/cpp
namespace Kioto::Renderer
{
using PassName = std::string;

class Material;
class Mesh;

class RenderObject
{
public:
    virtual ~RenderObject()
    {}

    void SetMaterial(Material* material, bool composeBuffersAndTextures = true);
    Material* GetMaterial() const;

    void SetMesh(Mesh* mesh);
    Mesh* GetMesh() const;

    void SetToWorld(const Matrix4& mat);
    const Matrix4* GetToWorld() const;

    void SetToModel(const Matrix4& mat);
    const Matrix4* GetToModel() const;

    void ComposeAllConstantBuffers();
    void RegisterAllTextureSets();

    const RenderObjectBufferLayout& GetBufferLayout(const PassName& passName);
    const TextureSet& GetTextureSet(const PassName& passName);

    template<typename T>
    ConstantBuffer::eReturnCode SetValueToBuffer(const std::string& name, T&& val, const PassName& passName)
    {
        assert(m_renderObjectBuffers.count(passName) == 1);
        ConstantBuffer::eReturnCode retCode = ConstantBuffer::eReturnCode::NotFound;
        for (auto& cb : m_renderObjectBuffers[passName].constantBuffers)
        {
            auto code = cb.Set(name, std::forward<T>(val));
            if (code == ConstantBuffer::eReturnCode::Ok)
                retCode = ConstantBuffer::eReturnCode::Ok;
        }
        return retCode;
    }

private:
    Material* m_material = nullptr;
    Mesh* m_mesh = nullptr;
    std::unordered_map<PassName, RenderObjectBufferLayout> m_renderObjectBuffers;
    std::unordered_map<PassName, TextureSet> m_textureSets;

    const Matrix4* m_toWorld = nullptr;
    const Matrix4* m_toModel = nullptr;
};

inline void RenderObject::SetMaterial(Material* material, bool composeBuffersAndTextures /* = true */)
{
    m_material = material;
    if (composeBuffersAndTextures)
    {
        ComposeAllConstantBuffers();
        RegisterAllTextureSets();
    }
}

inline Material* RenderObject::GetMaterial() const
{
    return m_material;
}

inline void RenderObject::SetMesh(Mesh* mesh)
{
    m_mesh = mesh;
}

inline Mesh* RenderObject::GetMesh() const
{
    return m_mesh;
}

inline void RenderObject::SetToWorld(const Matrix4& mat)
{
    m_toWorld = &mat;
}

inline const Matrix4* RenderObject::GetToWorld() const
{
    return m_toWorld;
}

inline void RenderObject::SetToModel(const Matrix4& mat)
{
    m_toModel = &mat;
}

inline const Matrix4* RenderObject::GetToModel() const
{
    return m_toModel;
}

inline void RenderObject::ComposeAllConstantBuffers()
{
    for (auto& pipelines : m_material->GetPipelineStates())
    {
        Shader* shader = pipelines.second.Shader;
        RenderObjectBufferLayout bufferLayout;

        bufferLayout.constantBuffers = shader->CreateLayoutTemplateShalowCopy();

        for (auto& cb : bufferLayout.constantBuffers)
            cb.ComposeBufferData();

        assert(m_renderObjectBuffers.count(pipelines.first) == 0);
        m_renderObjectBuffers[pipelines.first] = std::move(bufferLayout);
    }
}

inline void RenderObject::RegisterAllTextureSets()
{
    for (auto& textureAssetDescriptionsForPasses : m_material->GetTextureAssetDescriptions())
    {
        const PassName& passName = textureAssetDescriptionsForPasses.first;
        const PipelineState& state = m_material->GetPipelineState(passName);
        TextureSet set;
        for (auto& texDescr : textureAssetDescriptionsForPasses.second)
        {
            std::string fullPath = AssetsSystem::GetAssetFullPath(texDescr.Path);
            Texture* tex = AssetsSystem::GetRenderAssetsManager()->GetOrLoadAsset<Texture>(fullPath);
            set.SetTexture(texDescr.Name, tex);
        }
        if (set.GetTexturesCount() > 0)
            Renderer::RegisterTextureSet(set);
        assert(m_textureSets.count(passName) == 0);
        m_textureSets[passName] = std::move(set);
    }
}

inline const RenderObjectBufferLayout& RenderObject::GetBufferLayout(const PassName& passName)
{
    assert(m_renderObjectBuffers.count(passName) == 1);
    return m_renderObjectBuffers.at(passName);
}

inline const TextureSet& RenderObject::GetTextureSet(const PassName& passName)
{
    assert(m_textureSets.count(passName) == 1);
    return m_textureSets.at(passName);
}

}
