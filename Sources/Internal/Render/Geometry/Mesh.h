//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "AssetsSystem/Asset.h"
#include "Core/CoreTypes.h"
#include "Render/VertexLayout.h"
#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
struct IntermediateMesh;

enum class eTopology
{
    TriangleList,
    TriangleStrip,
    Point,
    Line
};

enum class eIndexFormat
{
    Format8Bit,
    Format16Bit,
    Format32Bit
};

enum class eVertexDataType
{
    Type_Unknown,
    Type_V1,
    Type_V2,
    Type_V3,
    Type_V4
};

class Mesh : public Asset
{
public:
    Mesh(VertexLayout layout, uint32 vertexCount, uint32 indexCount);
    Mesh(const std::string& path);
    Mesh(const Mesh& other);
    Mesh(Mesh&& other);
    ~Mesh();

    Mesh& operator=(Mesh other);

    void InitFromLayout(VertexLayout layout, uint32 vertexCount, uint32 indexCount);
    void FromIntermediateMesh(const IntermediateMesh& iMesh);

    uint32* GetIndexPtr(uint32 i);
    eDataFormat GetVertexElementFormat(eVertexSemantic semantic, uint8 semanticIndex) const;

    template <typename T>
    T* GetVertexElementPtr(uint32 i, eVertexSemantic semantic, uint8 semanticIndex);

    // [a_vorontcov] Some helpers. If you need other types then return type or other semantic/index use GetVertexElementPtr.
    // If you set in layout other type and use this methods you don't get error/warning message, you'll just trash data, so be careful
    // or you angry pinokkio for yourself.

    ///
    /// Get position. Treating as Vector3 ptr.
    ///
    Vector3* GetPositionPtr(uint32 i); // [a_vorontcov] TODO: remember offsets and return direct.

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

    const byte* GetVertexData() const;
    uint32 GetVertexDataSize() const;
    uint32 GetVertexDataStride() const;
    uint32 GetVertexCount() const;
    uint32 GetIndexCount() const;
    const byte* GetIndexData() const;
    uint32 GetIndexDataSize() const;

    MeshHandle GetHandle() const;
    void SetHandle(MeshHandle handle);

    friend void swap(Mesh& l, Mesh& r)
    {
        std::swap(l.m_vertexData, r.m_vertexData);
        std::swap(l.m_vertexDataSize, r.m_vertexDataSize);

        std::swap(l.m_indexData, r.m_indexData);
        std::swap(l.m_indexDataSize, r.m_indexDataSize);

        std::swap(l.m_vertexCount, r.m_vertexCount);
        std::swap(l.m_indexCount, r.m_indexCount);
        swap(l.m_layout, r.m_layout);
    }

    inline static constexpr uint32 MaxTexcoordCount = 8;
    inline static constexpr uint32 MaxColorCount = 8;

private:
    void LayoutFromIntermediateMesh(const IntermediateMesh& iMesh);

    byte* m_vertexData = nullptr;
    uint32 m_vertexDataSize = 0;

    byte* m_indexData = nullptr;
    uint32 m_indexDataSize = 0;

    uint32 m_vertexCount = 0;
    uint32 m_indexCount = 0;
    VertexLayout m_layout;

    MeshHandle m_handle;
};

inline uint32* Mesh::GetIndexPtr(uint32 i)
{
    assert(i < m_indexCount);
    return reinterpret_cast<uint32*>(m_indexData + sizeof(uint32) * i);
}

template <typename T>
inline T* Mesh::GetVertexElementPtr(uint32 i, eVertexSemantic semantic, uint8 semanticIndex)
{
    assert(i < m_vertexCount);
    const SemanticDesc* e = m_layout.FindElement(semantic, semanticIndex);
    if (e == nullptr)
        return nullptr;
    return reinterpret_cast<T*>(m_vertexData + static_cast<uint64>(m_layout.GetVertexStride()) * static_cast<uint64>(i) + e->Offset);
}

inline Vector3* Mesh::GetPositionPtr(uint32 i)
{
    return GetVertexElementPtr<Vector3>(i, eVertexSemantic::Position, 0);
}

inline Vector3* Mesh::GetNormalPtr(uint32 i)
{
    return GetVertexElementPtr<Vector3>(i, eVertexSemantic::Normal, 0);
}

inline Vector2* Mesh::GetUv0Ptr(uint32 i)
{
    return GetVertexElementPtr<Vector2>(i, eVertexSemantic::Texcoord, 0);
}

inline Vector4* Mesh::GetColorPtr(uint32 i)
{
    return GetVertexElementPtr<Vector4>(i, eVertexSemantic::Color, 0);
}

inline eDataFormat Mesh::GetVertexElementFormat(eVertexSemantic semantic, uint8 semanticIndex) const
{
    const SemanticDesc* e = m_layout.FindElement(semantic, semanticIndex);
    if (e == nullptr)
        return eDataFormat::UNKNOWN;
    return e->Format;
}

inline const byte* Mesh::GetVertexData() const
{
    return m_vertexData;
}

inline uint32 Mesh::GetVertexDataSize() const
{
    return m_vertexDataSize;
}

inline uint32 Mesh::GetVertexDataStride() const
{
    return m_layout.GetVertexStride();
}

inline uint32 Mesh::GetVertexCount() const
{
    return m_vertexCount;
}

inline uint32 Mesh::GetIndexCount() const
{
    return m_indexCount;
}

inline const byte* Mesh::GetIndexData() const
{
    return m_indexData;
}

inline uint32 Mesh::GetIndexDataSize() const
{
    return m_indexDataSize;
}

inline MeshHandle Mesh::GetHandle() const
{
    return m_handle;
}

inline void Mesh::SetHandle(MeshHandle handle)
{
    m_handle = handle;
}
}