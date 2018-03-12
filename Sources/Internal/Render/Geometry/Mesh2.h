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

    template <typename T>
    T* GetVertexElementPtr(uint32 i, Renderer::eVertexSemantic semantic, uint8 semanticIndex);

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
T* Mesh2::GetVertexElementPtr(uint32 i, Renderer::eVertexSemantic semantic, uint8 semanticIndex)
{
    const Renderer::VertexDesc* e = m_layout.FindElement(semantic, semanticIndex);
    if (e == nullptr)
        return nullptr;
    return reinterpret_cast<T*>(m_vertData + m_layout.GetVertexStride() * i + e->Offset);
}
}