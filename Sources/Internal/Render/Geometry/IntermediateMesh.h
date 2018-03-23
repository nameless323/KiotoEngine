//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <vector>

#include "Math/Vector4.h"
#include "Math/Vector2.h"

namespace Kioto::Renderer
{
///
/// Mesh representation for parsing from 3rd side formats.
///
struct IntermediateMesh
{
    enum eVertexFormatElement
    {
        Position = 1 << 0,
        Normal = 1 << 1,
        Tanget = 1 << 2,
        Bitangent = 1 << 3,
        Color0 = 1 << 4,
        Color1 = 1 << 5,
        Color2 = 1 << 6,
        Color3 = 1 << 7,
        Color4 = 1 << 8,
        Color5 = 1 << 9,
        Color6 = 1 << 10,
        Color7 = 1 << 11,
        Color8 = 1 << 12,
        UV0 = 1 << 13,
        UV1 = 1 << 14,
        UV2 = 1 << 15,
        UV3 = 1 << 16,
        UV4 = 1 << 17,
        UV5 = 1 << 18,
        UV6 = 1 << 19,
        UV7 = 1 << 20,
        UV8 = 1 << 21,
    };

    struct Vertex
    {
        Vector4 Pos;
        Vector4 Norm;
        Vector4 Tangent;
        Vector4 Bitangent;
        std::vector<Vector4> Color;
        std::vector<Vector2> Uv;

        bool marked = false;

        bool operator== (const Vertex& other) const;
    };

    uint32 LayoutMask = eVertexFormatElement::Position;
    IntermediateMesh() = default;
    IntermediateMesh(const IntermediateMesh&) = delete;
    IntermediateMesh& operator=(const IntermediateMesh&) = delete;

    std::vector<Vertex> Vertices;
    std::vector<uint32> Indices;

    void Indexate();
};
}