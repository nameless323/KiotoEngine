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

    bool GetCastShadow() const;
    void SetCastShadow(bool castShadow);

    bool GetIsVisible() const;
    void SetIsVisible(bool isVisible);

    void ComposeAllConstantBuffers();
    void RegisterAllTextureSets();

    /// <summary>
    /// Hijacks cb handle in the render object. This is necessary when you need to set cb as a common cb (time, light), or just set per pass buffer (camera)
    /// </summary>
    void SetExternalCB(const std::string& passName, const std::string& cbName, ConstantBufferHandle newHandle); // [a_vorontcov] TODO: really fishy. rethink

    template <typename T>
    void SetConstant(const std::string& passName, const std::string& cName, T constant);

    std::vector<ConstantBufferHandle> GetCBHandles(const std::string& passName) const;
    std::vector<uint32> GetConstants(const std::string& passName) const;

    const RenderObjectBufferLayout& GetBufferLayout(const PassName& passName);
    const TextureSet& GetTextureSet(const PassName& passName);
    const std::unordered_map<PassName, RenderObjectBufferLayout>& GetBuffersLayouts() const;
    std::unordered_map<PassName, RenderObjectBufferLayout>& GetBuffersLayouts();
    void SetTexture(const std::string& name, Texture* texture, const std::string& passName);

    template<typename T>
    bool SetBuffer(const std::string& name, T&& val, const PassName& passName, uint32 elemOffset = 0)
    {
        assert(m_renderObjectBuffers.count(passName) == 1);
        for (auto& cb : m_renderObjectBuffers[passName])
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
    bool m_castShadow = true; // [a_vorontcov] TODO: merge to one bitmask
    bool m_isVisible = true;
    std::unordered_map<PassName, RenderObjectBufferLayout> m_renderObjectBuffers;
    std::unordered_map<PassName, RenderObjectConstants> m_renderObjectConstants;
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

inline bool RenderObject::GetCastShadow() const
{
    return m_castShadow;
}

inline void RenderObject::SetCastShadow(bool castShadow)
{
    m_castShadow = castShadow;
}

inline bool RenderObject::GetIsVisible() const
{
    return m_isVisible;
}

inline void RenderObject::SetIsVisible(bool isVisible)
{
    m_isVisible = isVisible;
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


template <typename T>
inline void RenderObject::SetConstant(const std::string& passName, const std::string& cName, T constant)
{
    if (!m_renderObjectConstants.count(passName))
    {
        assert(false);
        return;
    }
    RenderObjectConstants& constants = m_renderObjectConstants[passName];
    auto& c = std::find_if(constants.begin(), constants.end(), [&cName](const UniformConstant& c) { return c.GetName() == cName; });
    if (c == constants.end())
    {
        assert(false);
        return;
    }
    c->SetValue(constant);
}

}
