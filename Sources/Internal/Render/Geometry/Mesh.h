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

class Mesh
{
public:
    Mesh() = default;
    ///
    /// Mesh will steal data pointer and will handle it lifetime by himself.
    ///
    Mesh(byte* data, uint32 dataSize, uint32 dataStride);
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

private:
    byte* m_data;
    uint32 m_dataSize;
    uint32 m_dataStride;
};
}