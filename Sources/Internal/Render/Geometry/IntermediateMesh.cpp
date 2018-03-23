//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/Geometry/IntermediateMesh.h"

#include <algorithm>

namespace Kioto::Renderer
{
bool IntermediateMesh::Vertex::operator==(const IntermediateMesh::Vertex& other) const
{
    bool p = Pos == other.Pos;
    for (size_t i = 0; i < Color.size(); ++i)
    {
        if (Color[i] != other.Color[i])
            return false;
    }
    bool n = Norm == other.Norm;
    for (size_t i = 0; i < Uv.size(); ++i)
    {
        if (Uv[i] != other.Uv[i])
            return false;
    }
    bool t = Tangent == other.Tangent;
    bool b = Bitangent == other.Bitangent;
    return p && n && t && b;
}

void IntermediateMesh::Indexate()
{
    Indices.reserve(Vertices.size());
    for (uint32 i = 0; i < Vertices.size(); ++i)
        Indices.push_back(i);

    uint32 markedBefore = 0;
    for (size_t i = 0; i < Vertices.size(); ++i)
    {
        if (Vertices[i].marked)
        {
            ++markedBefore;
            continue;
        }

        for (size_t j = i + 1; j < Vertices.size(); ++j)
        {
            if (Vertices[j].marked)
                continue;

            if (Vertices[i] == Vertices[j])
            {
                Indices[j] = static_cast<uint32>(i) - markedBefore;
                Vertices[j].marked = true;
            }
        }
        Indices[i] -= markedBefore;
    }
    Vertices.erase(std::remove_if(Vertices.begin(), Vertices.end(), [](const Vertex& v) { return v.marked; }), Vertices.end());
}
}