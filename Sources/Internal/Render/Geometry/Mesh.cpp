//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/Geometry/Mesh.h"

namespace Kioto
{

Mesh::Mesh(byte* data, uint32 dataSize, uint32 dataStride)
    : m_data(data), m_dataSize(dataSize), m_dataStride(dataStride)
{
}

Mesh::Mesh(const Mesh& other)
    : m_dataSize(other.m_dataSize), m_dataStride(other.m_dataStride)
{
    m_data = new byte[m_dataSize];
    memcpy(m_data, other.m_data, m_dataSize);
    Position = other.Position;
    Normal = other.Normal;
    Color = other.Color;
    UV0 = other.UV0;
    Triangles = other.Triangles;
}

Mesh::Mesh(Mesh&& other)
    : m_data(other.m_data), m_dataSize(other.m_dataSize), m_dataStride(other.m_dataStride)
{
    other.m_data = nullptr;
    Position = std::move(other.Position);
    Normal = std::move(other.Normal);
    Color = std::move(other.Color);
    UV0 = std::move(other.UV0);
    Triangles = std::move(other.Triangles);
}

Mesh::~Mesh()
{
    SafeDelete(m_data);
}

Mesh& Mesh::operator=(Mesh&& other)
{
    if (this == &other)
        return *this;
    m_dataSize = other.m_dataSize;
    m_dataStride = other.m_dataStride;

    SafeDelete(m_data);
    m_data = other.m_data;
    other.m_data = nullptr;

    Position = std::move(other.Position);
    Normal = std::move(other.Normal);
    Color = std::move(other.Color);
    UV0 = std::move(other.UV0);
    Triangles = std::move(other.Triangles);
}

Mesh& Mesh::operator=(const Mesh& other)
{
    if (this == &other)
        return *this;
    m_dataSize = other.m_dataSize;
    m_dataStride = other.m_dataStride;

    SafeDelete(m_data);
    m_data = new byte[m_dataSize];
    memcpy(m_data, other.m_data, m_dataSize);
    Position = other.Position;
    Normal = other.Normal;
    Color = other.Color;
    UV0 = other.UV0;
    Triangles = other.Triangles;

    return *this;
}

}