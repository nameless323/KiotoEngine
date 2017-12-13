//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/Geometry/Mesh.h"

#include "Render/Renderer.h"

namespace Kioto
{

Mesh::Mesh(byte* data, uint32 dataSize, uint32 dataStride, uint32 vertexCount, byte* indexData, uint32 indexDataSize, uint32 indexCount, eIndexFormat indexFormat, Renderer::VertexLayout vertexLayout, bool dynamic)
    : m_data(data)
    , m_dataSize(dataSize)
    , m_vertexCount(vertexCount)
    , m_dataStride(dataStride)
    , m_indexData(indexData)
    , m_indexDataSize(indexDataSize)
    , m_indexCount(indexCount)
    , m_indexFormat(indexFormat)
    , m_vertexLayout(std::move(vertexLayout))
    , m_isDynamic(dynamic)
{
}

Mesh::Mesh(const Mesh& other)
    : m_dataSize(other.m_dataSize)
    , m_dataStride(other.m_dataStride)
    , m_indexDataSize(other.m_indexDataSize)
    , m_vertexCount(other.m_vertexCount)
    , m_indexCount(other.m_indexCount)
    , m_indexFormat(other.m_indexFormat)
    , m_vertexLayout(other.m_vertexLayout)
    , m_isDynamic(other.m_isDynamic)
    , m_handle(other.m_handle)
    , m_isDirty(other.m_isDirty)
{
    m_data = new byte[m_dataSize];
    memcpy(m_data, other.m_data, m_dataSize);

    m_indexData = new byte[m_indexDataSize];
    memcpy(m_indexData, other.m_indexData, m_indexDataSize);

    Position = other.Position;
    Normal = other.Normal;
    Color = other.Color;
    UV0 = other.UV0;
    Triangles = other.Triangles;
}

Mesh::Mesh(Mesh&& other)
    : m_data(other.m_data)
    , m_dataSize(other.m_dataSize)
    , m_dataStride(other.m_dataStride)
    , m_indexData(other.m_indexData)
    , m_indexDataSize(other.m_indexDataSize)
    , m_vertexCount(other.m_vertexCount)
    , m_indexCount(other.m_indexCount)
    , m_indexFormat(other.m_indexFormat)
    , m_vertexLayout(other.m_vertexLayout)
    , m_isDynamic(other.m_isDynamic)
    , m_handle(other.m_handle)
    , m_isDirty(other.m_isDirty)
{
    other.m_data = nullptr;
    other.m_indexData = nullptr;
    Position = std::move(other.Position);
    Normal = std::move(other.Normal);
    Color = std::move(other.Color);
    UV0 = std::move(other.UV0);
    Triangles = std::move(other.Triangles);
}

Mesh::~Mesh()
{
    SafeDelete(m_data);
    SafeDelete(m_indexData);
}

void Mesh::SetData(byte* data, uint32 dataSize, uint32 dataStride, uint32 vertexCount, byte* indexData, uint32 indexDataSize, uint32 indexCount, eIndexFormat indexFormat, Renderer::VertexLayout layout, bool dynamic)
{
    SafeDelete(m_data);
    SafeDelete(m_indexData);

    m_data = data;
    m_dataSize = dataSize;
    m_dataStride = dataStride;
    m_vertexCount = vertexCount;

    m_indexData = indexData;
    m_indexDataSize = indexDataSize;
    m_indexCount = indexCount;
    m_indexFormat = indexFormat;

    m_vertexLayout = layout;

    m_handle = Renderer::GenerateVertexLayout(m_vertexLayout);

    m_isDynamic = dynamic;
    m_isDirty = false;
}

void Mesh::PrepareForUpload()
{
    uint32 pSize = static_cast<uint32>(Position.size());
    uint32 nSize = static_cast<uint32>(Normal.size());
    uint32 cSize = static_cast<uint32>(Color.size());
    uint32 uSize = static_cast<uint32>(UV0.size());
    m_dataSize = pSize * sizeof(Vector3) + nSize * sizeof(Vector3) + cSize * sizeof(Vector4) + uSize * sizeof(Vector2);
    SafeDelete(m_data);
    m_data = new byte[m_dataSize];
    SafeDelete(m_indexData);
    m_indexData = new byte[Triangles.size() * sizeof(uint32)];

    m_indexFormat = eIndexFormat::Format32Bit;
    m_indexCount = static_cast<uint32>(Triangles.size());
    m_vertexCount = pSize;

    m_vertexLayout.CleanElements();

    if (!Position.empty())
    {
        m_dataStride += sizeof(Vector3);
        m_vertexLayout.AddElement(Renderer::eVertexSemantic::Position, 0, Renderer::eVertexDataFormat::R32_G32_B32);
    }
    if (!Normal.empty())
    {
        m_dataStride += sizeof(Vector3);
        m_vertexLayout.AddElement(Renderer::eVertexSemantic::Normal, 0, Renderer::eVertexDataFormat::R32_G32_B32);
    }
    if (!Color.empty())
    {
        m_dataStride += sizeof(Vector4);
        m_vertexLayout.AddElement(Renderer::eVertexSemantic::Color, 0, Renderer::eVertexDataFormat::R32_G32_B32_A32);
    }
    if (!UV0.empty())
    {
        m_dataStride += sizeof(Vector2);
        m_vertexLayout.AddElement(Renderer::eVertexSemantic::Texcoord, 0, Renderer::eVertexDataFormat::R32_G32);
    }
    byte* currDataPtr = m_data;
    for (uint32 i = 0; i < pSize; ++i)
    {
        if (!Position.empty())
        {
            Vector3* pptr = reinterpret_cast<Vector3*>(currDataPtr);
            *pptr = Position[i];
            currDataPtr += sizeof(Vector3);
        }
        if (!Normal.empty())
        {
            Vector3* nptr = reinterpret_cast<Vector3*>(currDataPtr);
            *nptr = Normal[i];
            currDataPtr += sizeof(Vector3);
        }
        if (!Color.empty())
        {
            Vector4* cptr = reinterpret_cast<Vector4*>(currDataPtr);
            *cptr = Color[i];
            currDataPtr += sizeof(Vector4);
        }
        if (!UV0.empty())
        {
            Vector2* uptr = reinterpret_cast<Vector2*>(currDataPtr);
            *uptr = UV0[i];
            currDataPtr += sizeof(Vector2);
        }
    }

    uint32* currIndexPtr = reinterpret_cast<uint32*>(m_indexData);
    for (uint32 index : Triangles)
    {
        *currIndexPtr = index;
        ++currIndexPtr;
    }

    m_handle = Renderer::GenerateVertexLayout(m_vertexLayout); // [a_vorontsov] And here we will generate new layout every time. TODO: Make lookup!
    m_isDirty = false;
}

Mesh& Mesh::operator=(Mesh&& other)
{
    if (this == &other)
        return *this;
    m_dataSize = other.m_dataSize;
    m_dataStride = other.m_dataStride;
    m_indexDataSize = other.m_indexDataSize;

    m_vertexCount = other.m_vertexCount;
    m_indexCount = other.m_indexCount;
    m_indexFormat = other.m_indexFormat;

    SafeDelete(m_data);
    m_data = other.m_data;
    other.m_data = nullptr;

    SafeDelete(m_indexData);
    m_indexData = other.m_indexData;
    other.m_indexData = nullptr;

    Position = std::move(other.Position);
    Normal = std::move(other.Normal);
    Color = std::move(other.Color);
    UV0 = std::move(other.UV0);
    Triangles = std::move(other.Triangles);

    m_vertexLayout = std::move(other.m_vertexLayout);
    m_handle = other.m_handle;
    m_isDirty = other.m_isDirty;
    m_isDynamic = other.m_isDynamic;

    return *this;
}

Mesh& Mesh::operator=(const Mesh& other)
{
    if (this == &other)
        return *this;
    m_dataSize = other.m_dataSize;
    m_dataStride = other.m_dataStride;
    m_indexDataSize = other.m_indexDataSize;

    SafeDelete(m_data);
    m_data = new byte[m_dataSize];
    memcpy(m_data, other.m_data, m_dataSize);

    SafeDelete(m_indexData);
    m_indexData = new byte[m_indexDataSize];
    memcpy(m_indexData, other.m_indexData, m_indexDataSize);

    m_vertexCount = other.m_vertexCount;
    m_indexCount = other.m_indexCount;
    m_indexFormat = other.m_indexFormat;

    Position = other.Position;
    Normal = other.Normal;
    Color = other.Color;
    UV0 = other.UV0;
    Triangles = other.Triangles;

    m_vertexLayout = other.m_vertexLayout;

    m_handle = other.m_handle;
    m_isDirty = other.m_isDirty;
    m_isDynamic = other.m_isDynamic;

    return *this;
}
}