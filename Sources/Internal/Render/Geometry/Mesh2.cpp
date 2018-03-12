#include "stdafx.h"

#include "Render/Geometry/Mesh2.h"

namespace Kioto
{

Mesh2::Mesh2(Renderer::VertexLayout layout)
    : Asset("")
{

}

Mesh2::Mesh2(const std::string& path)
    : Asset(path)
{

}

Mesh2::Mesh2(const Mesh2& other)
    : Asset(other.GetAssetPath())
    , m_data(other.m_data)
    , m_vertexCount(other.m_vertexCount)
    , m_indexCount(other.m_indexCount)
    , m_layout(other.m_layout)
    , m_vertexStride(other.m_vertexStride)
{
}

Mesh2::Mesh2(Mesh2&& other)
    : Asset(other.GetAssetPath())
{
    swap(*this, other);
}

Mesh2::~Mesh2()
{
    SafeDelete(m_data);
}

Mesh2& Mesh2::operator=(Mesh2 other)
{
    swap(*this, other);
    return *this;
}

}