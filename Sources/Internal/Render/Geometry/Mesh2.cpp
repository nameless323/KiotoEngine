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
    , m_vertData(other.m_vertData)
    , m_vertDataSize(other.m_vertDataSize)
    , m_indData(other.m_indData)
    , m_indDataSize(other.m_indDataSize)
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
    SafeDelete(m_vertData);
}

void Mesh2::InitFromLayout(Renderer::VertexLayout layout, uint32 vertexCount, uint32 indexCount)
{
    SafeDelete(m_vertData);
    m_vertDataSize = m_layout.GetVertexStride() * vertexCount;

    SafeDelete(m_indData);
    m_indDataSize = indexCount * sizeof(uint32);

    m_vertexCount = vertexCount;
    m_indexCount = indexCount;
    m_layout = layout;

    m_vertData = new byte[m_vertDataSize];
    m_indData = new byte[m_indDataSize];
}

Mesh2& Mesh2::operator=(Mesh2 other)
{
    swap(*this, other);
    return *this;
}

}