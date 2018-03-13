#include "stdafx.h"

#include "Render/Geometry/Mesh2.h"

namespace Kioto
{

Mesh2::Mesh2(Renderer::VertexLayout layout, uint32 vertexCount, uint32 indexCount)
    : Asset("")
{
    InitFromLayout(std::move(layout), vertexCount, indexCount);
}

Mesh2::Mesh2(const std::string& path)
    : Asset(path)
{

}

Mesh2::Mesh2(const Mesh2& other)
    : Asset(other.GetAssetPath())
    , m_vertexData(other.m_vertexData)
    , m_vertexDataSize(other.m_vertexDataSize)
    , m_indexData(other.m_indexData)
    , m_indexDataSize(other.m_indexDataSize)
    , m_vertexCount(other.m_vertexCount)
    , m_indexCount(other.m_indexCount)
    , m_layout(other.m_layout)
{
}

Mesh2::Mesh2(Mesh2&& other)
    : Asset(other.GetAssetPath())
{
    swap(*this, other);
}

Mesh2::~Mesh2()
{
    SafeDelete(m_vertexData);
    SafeDelete(m_indexData);
}

void Mesh2::InitFromLayout(Renderer::VertexLayout layout, uint32 vertexCount, uint32 indexCount)
{
    m_vertexCount = vertexCount;
    m_indexCount = indexCount;
    swap(m_layout, layout);

    SafeDelete(m_vertexData);
    m_vertexDataSize = m_layout.GetVertexStride() * vertexCount;

    SafeDelete(m_indexData);
    m_indexDataSize = indexCount * sizeof(uint32);

    m_vertexData = new byte[m_vertexDataSize];
    m_indexData = new byte[m_indexDataSize];
}

Mesh2& Mesh2::operator=(Mesh2 other)
{
    swap(*this, other);
    return *this;
}

}