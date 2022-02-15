#pragma once

#include "AssetsSystem/Asset.h"
#include "Core/CoreTypes.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
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
        std::swap(l.mVertexData, r.mVertexData);
        std::swap(l.mVertexDataSize, r.mVertexDataSize);

        std::swap(l.mIndexData, r.mIndexData);
        std::swap(l.mIndexDataSize, r.mIndexDataSize);

        std::swap(l.mVertexCount, r.mVertexCount);
        std::swap(l.mIndexCount, r.mIndexCount);
        swap(l.mLayout, r.mLayout);
    }

    inline static constexpr uint32 MaxTexcoordCount = 8;
    inline static constexpr uint32 MaxColorCount = 8;

private:
    void LayoutFromIntermediateMesh(const IntermediateMesh& iMesh);

    byte* mVertexData = nullptr;
    uint32 mVertexDataSize = 0;

    byte* mIndexData = nullptr;
    uint32 mIndexDataSize = 0;

    uint32 mVertexCount = 0;
    uint32 mIndexCount = 0;
    VertexLayout mLayout;

    MeshHandle mHandle;
};

inline uint32* Mesh::GetIndexPtr(uint32 i)
{
    assert(i < mIndexCount);
    return reinterpret_cast<uint32*>(mIndexData + sizeof(uint32) * i);
}

template <typename T>
inline T* Mesh::GetVertexElementPtr(uint32 i, eVertexSemantic semantic, uint8 semanticIndex)
{
    assert(i < mVertexCount);
    const SemanticDesc* e = mLayout.FindElement(semantic, semanticIndex);
    if (e == nullptr)
        return nullptr;
    return reinterpret_cast<T*>(mVertexData + static_cast<uint64>(mLayout.GetVertexStride()) * static_cast<uint64>(i) + e->Offset);
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
    const SemanticDesc* e = mLayout.FindElement(semantic, semanticIndex);
    if (e == nullptr)
        return eDataFormat::UNKNOWN;
    return e->Format;
}

inline const byte* Mesh::GetVertexData() const
{
    return mVertexData;
}

inline uint32 Mesh::GetVertexDataSize() const
{
    return mVertexDataSize;
}

inline uint32 Mesh::GetVertexDataStride() const
{
    return mLayout.GetVertexStride();
}

inline uint32 Mesh::GetVertexCount() const
{
    return mVertexCount;
}

inline uint32 Mesh::GetIndexCount() const
{
    return mIndexCount;
}

inline const byte* Mesh::GetIndexData() const
{
    return mIndexData;
}

inline uint32 Mesh::GetIndexDataSize() const
{
    return mIndexDataSize;
}

inline MeshHandle Mesh::GetHandle() const
{
    return mHandle;
}

inline void Mesh::SetHandle(MeshHandle handle)
{
    mHandle = handle;
}
}