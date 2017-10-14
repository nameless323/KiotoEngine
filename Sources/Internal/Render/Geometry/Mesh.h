//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <vector>

#include "Core/CoreTypes.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Kioto
{
enum class eTopology
{
    TriangleList,
    TriangleStrip,
    Point,
    Line
};

///
/// Represent model's mesh. Use overloads with *data for best performance.
/// If you use *data overloads - mesh owns data and handle it lifetime.
///
class Mesh // [a_vorontsov] TODO:: template for index type.
{
public:
    Mesh() = default;
    ///
    /// Mesh will steal data pointer and will handle it lifetime by himself.
    ///
    Mesh(byte* data, uint32 dataSize, uint32 dataStride, byte* indexData, uint32 indexDataSize);
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

    void SetData(byte* data, uint32 dataSize, uint32 dataStride, byte* indexData, uint32 indexDataSize);
    ///
    /// Prepares mesh to upload to api's vertex buffer. Call this method if you modified mesh via Position, Normal etc.
    /// If you modify Position, Normal etc after you call this method, you should call it again.
    ///
    void PrepareForUpload();

    const byte* GetVertexData() const;
    uint32 GetVertexDataSize() const;
    uint32 GetVertexDataStride() const;
    const byte* GetIndexData() const;
    uint32 GetIndexDataSize() const;

private:
    byte* m_data = nullptr;
    uint32 m_dataSize = 0;
    uint32 m_dataStride = 0;

    byte* m_indexData = nullptr;
    uint32 m_indexDataSize = 0;
};

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

inline const byte* Mesh::GetIndexData() const
{
    return m_indexData;
}

inline uint32 Mesh::GetIndexDataSize() const
{
    return m_dataSize;
}
}