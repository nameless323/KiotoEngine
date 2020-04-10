#pragma once

namespace Kioto::Renderer
{
class Material;
class Mesh;

class RenderObject
{
public:
    void SetMaterial(Material* material);
    Material* GetMaterial() const;

    void SetMesh(Mesh* mesh);
    Mesh* GetMesh() const;

    void SetToWorld(const Matrix4& mat);
    const Matrix4* GetToWorld() const;

    void SetToModel(const Matrix4& mat);
    const Matrix4* GetToModel() const;

private:
    Material* m_material = nullptr;
    Mesh* m_mesh = nullptr;

    const Matrix4* m_toWorld = nullptr;
    const Matrix4* m_toModel = nullptr;
};

inline void RenderObject::SetMaterial(Material* material)
{
    m_material = material;
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
}
