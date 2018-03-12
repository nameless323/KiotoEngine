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
    Mesh2(Renderer::VertexLayout layout);
    Mesh2(const std::string& path);
    Mesh2(const Mesh2& other);
    Mesh2(Mesh2&& other);
    ~Mesh2();

    Mesh2& operator=(Mesh2 other);

    friend void swap(Mesh2& l, Mesh2& r)
    {
        std::swap(l.m_data, r.m_data);
        std::swap(l.m_vertexCount, r.m_vertexCount);
        std::swap(l.m_indexCount, r.m_indexCount);
        swap(l.m_layout, r.m_layout);
        std::swap(l.m_vertexStride, r.m_vertexStride);
    }

private:
    byte* m_data = nullptr;
    uint32 m_vertexCount = 0;
    uint32 m_indexCount = 0;
    Renderer::VertexLayout m_layout;
    uint32 m_vertexStride = 0;
};
}