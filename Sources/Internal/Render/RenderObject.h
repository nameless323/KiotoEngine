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
        assert(mRenderObjectBuffers.count(passName) == 1);
        for (auto& cb : mRenderObjectBuffers[passName])
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
    Material* mMaterial = nullptr;
    Mesh* mMesh = nullptr;
    bool mCastShadow = true; // [a_vorontcov] TODO: merge to one bitmask
    bool mIsVisible = true;
    std::unordered_map<PassName, RenderObjectBufferLayout> mRenderObjectBuffers;
    std::unordered_map<PassName, RenderObjectConstants> mRenderObjectConstants;
    std::unordered_map<PassName, TextureSet> mTextureSets; // [a_vorontcov] Buffers are unique for ro, but texture set is more a material thing. but does it matter for bindless textures and for this engine at all?

    const Matrix4* mToWorld = nullptr;
    const Matrix4* mToModel = nullptr;
};

inline void RenderObject::SetMaterial(Material* material, bool composeBuffersAndTextures /* = true */)
{
    mMaterial = material;
    if (composeBuffersAndTextures)
    {
        ComposeAllConstantBuffers();
        RegisterAllTextureSets();
    }
}

inline Material* RenderObject::GetMaterial() const
{
    return mMaterial;
}

inline void RenderObject::SetMesh(Mesh* mesh)
{
    mMesh = mesh;
}

inline Mesh* RenderObject::GetMesh() const
{
    return mMesh;
}

inline void RenderObject::SetToWorld(const Matrix4& mat)
{
    mToWorld = &mat;
}

inline const Matrix4* RenderObject::GetToWorld() const
{
    return mToWorld;
}

inline void RenderObject::SetToModel(const Matrix4& mat)
{
    mToModel = &mat;
}

inline const Matrix4* RenderObject::GetToModel() const
{
    return mToModel;
}

inline bool RenderObject::GetCastShadow() const
{
    return mCastShadow;
}

inline void RenderObject::SetCastShadow(bool castShadow)
{
    mCastShadow = castShadow;
}

inline bool RenderObject::GetIsVisible() const
{
    return mIsVisible;
}

inline void RenderObject::SetIsVisible(bool isVisible)
{
    mIsVisible = isVisible;
}

inline const RenderObjectBufferLayout& RenderObject::GetBufferLayout(const PassName& passName)
{
    assert(mRenderObjectBuffers.count(passName) == 1);
    return mRenderObjectBuffers.at(passName);
}

inline const TextureSet& RenderObject::GetTextureSet(const PassName& passName)
{
    assert(mTextureSets.count(passName) == 1);
    return mTextureSets.at(passName);
}

inline const std::unordered_map<PassName, RenderObjectBufferLayout>& RenderObject::GetBuffersLayouts() const
{
    return mRenderObjectBuffers;
}

inline std::unordered_map<PassName, RenderObjectBufferLayout>& RenderObject::GetBuffersLayouts()
{
    return mRenderObjectBuffers;
}


template <typename T>
inline void RenderObject::SetConstant(const std::string& passName, const std::string& cName, T constant)
{
    if (!mRenderObjectConstants.count(passName))
    {
        assert(false);
        return;
    }
    RenderObjectConstants& constants = mRenderObjectConstants[passName];
    auto c = std::find_if(constants.begin(), constants.end(), [&cName](const UniformConstant& c) { return c.GetName() == cName; });
    if (c == constants.end())
    {
        assert(false);
        return;
    }
    c->SetValue(constant);
}

}
