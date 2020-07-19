#pragma once

#include "Render/ShaderData.h"

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

    /// <summary>
    ///  Internally sets all constant buffers for rendering
    /// </summary>
    virtual void PrepareConstantBuffers(const std::string& passName);

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
    const std::unordered_map<PassName, RenderObjectBufferLayout>& GetBuffersLayouts() const;
    std::unordered_map<PassName, RenderObjectBufferLayout>& GetBuffersLayouts();
    void SetTexture(const std::string& name, Texture* texture, const std::string& passName);

    template<typename T>
    bool SetBuffer(const std::string& name, T&& val, const PassName& passName, uint32 elemOffset = 0)
    {
        assert(m_renderObjectBuffers.count(passName) == 1);
        for (auto& cb : m_renderObjectBuffers[passName].constantBuffers)
        {
            if (cb.GetName() == name)
            {
                cb.Set(val, elemOffset);
                return true;
            }
        }
        return false;
    }

private:
    Material* m_material = nullptr;
    Mesh* m_mesh = nullptr;
    std::unordered_map<PassName, RenderObjectBufferLayout> m_renderObjectBuffers;
    std::unordered_map<PassName, TextureSet> m_textureSets; // [a_vorontcov] Buffers are unique for ro, but texture set is more a material thing. but does it matter for bindless textures and for this engine at all?

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

inline const std::unordered_map<PassName, RenderObjectBufferLayout>& RenderObject::GetBuffersLayouts() const
{
    return m_renderObjectBuffers;
}

inline std::unordered_map<PassName, RenderObjectBufferLayout>& RenderObject::GetBuffersLayouts()
{
    return m_renderObjectBuffers;
}

}
