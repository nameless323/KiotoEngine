//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "AssetsSystem/Asset.h"
#include "Core/CoreTypes.h"
#include "Render/VertexLayout.h"

namespace Kioto
{
struct IntermediateMesh;

class Mesh2 : public Asset
{
public:
    Mesh2(Renderer::VertexLayout layout, uint32 vertexCount, uint32 indexCount);
    Mesh2(const std::string& path);
    Mesh2(const Mesh2& other);
    Mesh2(Mesh2&& other);
    ~Mesh2();

    Mesh2& operator=(Mesh2 other);

    void InitFromLayout(Renderer::VertexLayout layout, uint32 vertexCount, uint32 indexCount);

    uint32* GetIndexPtr(uint32 i);
    Renderer::eDataFormat GetVertexElementFormat(Renderer::eVertexSemantic semantic, uint8 semanticIndex) const;

    template <typename T>
    T* GetVertexElementPtr(uint32 i, Renderer::eVertexSemantic semantic, uint8 semanticIndex);

    // [a_vorontsov] Some helpers. If you need other types then return type or other semantic/index use GetVertexElementPtr.
    // If you set in layout other type and use this methods you don't get error/warning message, you'll just trash data, so be careful
    // or you angry pinokkio for yourself.

    ///
    /// Get position. Treating as Vector3 ptr.
    ///
    Vector3* GetPositionPtr(uint32 i); // [a_vorontsov] TODO: remember offsets and return direct.
    ///
    /// Get normal. Treating as Vector3 ptr.
    ///
    Vector3* GetNormalPtr(uint32 i);
    ///
    /// Get uv0. Treating as Vector2 ptr.
    ///
    Vector2* GetUv0Ptr(uint32 i);
    ///
    /// Get color. Treating as Vector4 ptr.
    ///
    Vector4* GetColorPtr(uint32 i);

    friend void swap(Mesh2& l, Mesh2& r)
    {
        std::swap(l.m_vertData, r.m_vertData);
        std::swap(l.m_vertDataSize, r.m_vertDataSize);

        std::swap(l.m_indData, r.m_indData);
        std::swap(l.m_indDataSize, r.m_indDataSize);

        std::swap(l.m_vertexCount, r.m_vertexCount);
        std::swap(l.m_indexCount, r.m_indexCount);
        swap(l.m_layout, r.m_layout);
        std::swap(l.m_vertexStride, r.m_vertexStride);
    }

private:
    byte* m_vertData = nullptr;
    uint32 m_vertDataSize = 0;

    byte* m_indData = nullptr;
    uint32 m_indDataSize = 0;

    uint32 m_vertexCount = 0;
    uint32 m_indexCount = 0;
    Renderer::VertexLayout m_layout;
};

inline uint32* Mesh2::GetIndexPtr(uint32 i)
{
    return reinterpret_cast<uint32*>(m_indData + sizeof(uint32) * i);
}

template <typename T>
inline T* Mesh2::GetVertexElementPtr(uint32 i, Renderer::eVertexSemantic semantic, uint8 semanticIndex)
{
    const Renderer::VertexDesc* e = m_layout.FindElement(semantic, semanticIndex);
    if (e == nullptr)
        return nullptr;
    return reinterpret_cast<T*>(m_vertData + m_layout.GetVertexStride() * i + e->Offset);
}

inline Vector3* Mesh2::GetPositionPtr(uint32 i)
{
    return GetVertexElementPtr<Vector3>(i, Renderer::eVertexSemantic::Position, 0);
}

inline Vector3* Mesh2::GetNormalPtr(uint32 i)
{
    return GetVertexElementPtr<Vector3>(i, Renderer::eVertexSemantic::Normal, 0);
}

inline Vector2* Mesh2::GetUv0Ptr(uint32 i)
{
    return GetVertexElementPtr<Vector2>(i, Renderer::eVertexSemantic::Texcoord, 0);
}

inline Vector4* Mesh2::GetColorPtr(uint32 i)
{
    return GetVertexElementPtr<Vector4>(i, Renderer::eVertexSemantic::Color, 0);
}

inline Renderer::eDataFormat Mesh2::GetVertexElementFormat(Renderer::eVertexSemantic semantic, uint8 semanticIndex) const
{
    const Renderer::VertexDesc* e = m_layout.FindElement(semantic, semanticIndex);
    if (e == nullptr)
        return Renderer::eDataFormat::UNKNOWN;
    return e->Format;
}
}