//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <vector>

#include "AssetsSystem/Asset.h"
#include "Core/CoreTypes.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Render/VertexLayout.h"
#include "Render/RendererPublic.h"

namespace Kioto
{
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

///
/// Represent model's mesh. Use overloads with *data for best performance.
/// If you use *data overloads - mesh owns data and handle it lifetime.
///
class Mesh : public Asset // [a_vorontsov] TODO:: template for index type.
{
public:
    Mesh() = default;
    ///
    /// Mesh will steal data pointer and will handle it lifetime by himself.
    ///
    Mesh(byte* data, uint32 dataSize, uint32 dataStride, uint32 vertexCount, byte* indexData, uint32 indexDataSize, uint32 indexCount, eIndexFormat indexFormat, Renderer::VertexLayout vertexLayout, bool dynamic = false);
    Mesh(const Mesh& other);
    Mesh(Mesh&& other); // [a_vorontsov] TODO: copy swap.
    ~Mesh();
    Mesh& operator=(const Mesh& other);
    Mesh& operator=(Mesh&& other);

    // [a_vorontsov] Winding order is CW.
    std::vector<Vector3> Position; // [a_vorontsov] Maybe just byte* position and so on? How to manage memory in that case?
    std::vector<Vector3> Normal;
    std::vector<Vector4> Color;
    std::vector<Vector2> UV0;
    std::vector<uint32> Triangles; // [a_vorontsov] Hmmm... 32bits enough for almost everything but we definitely will need 64 later.

    void SetData(byte* data, uint32 dataSize, uint32 dataStride, uint32 vertexCount, byte* indexData, uint32 indexDataSize, uint32 indexCount, eIndexFormat indexFormat, Renderer::VertexLayout vertexLayout, bool dynamic = false);
    ///
    /// Prepares mesh to upload to api's vertex buffer. Call this method if you modified mesh via Position, Normal etc.
    /// If you modify Position, Normal etc after you call this method, you should call it again.
    ///
    void PrepareForUpload();

    void SetDynamic(bool isDynamic);

    const byte* GetVertexData() const;
    uint32 GetVertexDataSize() const;
    uint32 GetVertexDataStride() const;
    uint32 GetVertexCount() const;
    uint32 GetIndexCount() const;
    eIndexFormat GetIndexFormat() const;
    const byte* GetIndexData() const;
    uint32 GetIndexDataSize() const;
    Renderer::VertexLayout GetVertexLayout() const;
    Renderer::Handle GetVertexLayoutHandle() const;
    bool GetIsDirty() const;
    bool GetIsDynamic() const;

private:
    byte* m_data = nullptr;
    uint32 m_dataSize = 0;
    uint32 m_dataStride = 0;
    uint32 m_vertexCount = 0;

    byte* m_indexData = nullptr;
    uint32 m_indexDataSize = 0;
    uint32 m_indexCount = 0;
    eIndexFormat m_indexFormat = eIndexFormat::Format32Bit;
    Renderer::VertexLayout m_vertexLayout;
    Renderer::Handle m_handle = Renderer::InvalidHandle;

    bool m_isDirty = true;
    bool m_isDynamic = false;
};

inline void Mesh::SetDynamic(bool isDynamic)
{
    m_isDynamic = isDynamic;
}

inline const byte* Mesh::GetVertexData() const
{
    return m_data;
}

inline uint32 Mesh::GetVertexDataSize() const
{
    return m_dataSize;
}

inline uint32 Mesh::GetVertexDataStride() const
{
    return m_dataStride;
}

inline uint32 Mesh::GetVertexCount() const
{
    return m_vertexCount;
}

inline uint32 Mesh::GetIndexCount() const
{
    return m_indexCount;
}

inline eIndexFormat Mesh::GetIndexFormat() const
{
    return m_indexFormat;
}

inline const byte* Mesh::GetIndexData() const
{
    return m_indexData;
}

inline uint32 Mesh::GetIndexDataSize() const
{
    return m_dataSize;
}

inline Renderer::VertexLayout Mesh::GetVertexLayout() const
{
    return m_vertexLayout;
}

inline Renderer::Handle Mesh::GetVertexLayoutHandle() const
{
    return m_handle;
}

inline bool Mesh::GetIsDynamic() const
{
    return m_isDynamic;
}

inline bool Mesh::GetIsDirty() const
{
    return m_isDirty;
}
}