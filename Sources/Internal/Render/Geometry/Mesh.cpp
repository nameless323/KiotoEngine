//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/Geometry/Mesh.h"

#include "Render/Renderer.h"
#include "Render/Geometry/MeshLoader.h"
#include "Render/Geometry/IntermediateMesh.h"

namespace Kioto
{

namespace
{
template <typename T>
void SetValueInPtr(byte** dataPtr, const std::vector<Mesh::VertexDataElementType>& data, uint32 index)
{
    T* p = reinterpret_cast<T*>(*dataPtr);
    *p = std::get<T>(data[index]);
    *dataPtr += sizeof(T);
}

Vector3 Vec4ToVec3(Vector4 src)
{
    return { src.x, src.y, src.z };
}
}

Mesh::Mesh(byte* data, uint32 dataSize, uint32 dataStride, uint32 vertexCount, byte* indexData, uint32 indexDataSize, uint32 indexCount, eIndexFormat indexFormat, Renderer::VertexLayout vertexLayout, bool dynamic)
    : Asset("")
    , m_data(data)
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
    : Asset("")
    , m_dataSize(other.m_dataSize)
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
    Tangent = other.Tangent;
    Bitangent = other.Bitangent;
    m_color = other.m_color;
    m_uv = other.m_uv;
    Indices = other.Indices;
}

Mesh::Mesh(Mesh&& other)
    : Asset("")
    , m_data(other.m_data)
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
    Tangent = std::move(other.Tangent);
    Bitangent = std::move(other.Bitangent);
    m_color = std::move(other.m_color);
    m_uv = std::move(other.m_uv);
    Indices = std::move(other.Indices);
}

Mesh::Mesh() : Asset("")
{
}

Mesh::Mesh(const std::string& path)
    : Asset(path)
{
    MeshLoader::LoadMesh(this);
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
    uint32 tSize = static_cast<uint32>(Tangent.size());
    uint32 btSize = static_cast<uint32>(Bitangent.size());

#if _DEBUG
    if (nSize > 0)
    {
        assert(pSize == nSize);
    }
    if (tSize > 0)
    {
        assert(pSize == tSize);
    }
    if (btSize > 0)
    {
        assert(pSize == btSize);
    }
#endif

    uint32 cSize = 0;
    for (uint8 i = 0; i < MaxColorCount; ++i)
    {
        if (m_color[i].Data.size() > 0)
        {
            assert(m_color[i].Type != eVertexDataType::Type_Unknown);
            uint32 size = static_cast<uint32>(m_color[i].Data.size());
            assert(pSize == size);
            cSize += GetDataElementSize(m_color[i]) * size;
        }
    }

    uint32 uSize = 0;
    for (uint8 i = 0; i < MaxUVCount; ++i)
    {
        if (m_uv[i].Data.size() > 0)
        {
            assert(m_uv[i].Type != eVertexDataType::Type_Unknown);
            uint32 size = static_cast<uint32>(m_uv[i].Data.size());
            assert(pSize == size);
            uSize += GetDataElementSize(m_uv[i]) * size;
        }
    }
    m_dataSize = pSize * sizeof(Vector3) + nSize * sizeof(Vector3) + cSize + uSize + tSize * sizeof(Vector3) + btSize * sizeof(Vector3);
    SafeDelete(m_data);
    m_data = new byte[m_dataSize];
    SafeDelete(m_indexData);
    m_indexData = new byte[Indices.size() * sizeof(uint32)];

    m_indexFormat = eIndexFormat::Format32Bit;
    m_indexCount = static_cast<uint32>(Indices.size());
    m_vertexCount = pSize;

    m_vertexLayout.CleanElements();

    if (!Position.empty())
    {
        m_dataStride += sizeof(Vector3);
        m_vertexLayout.AddElement(Renderer::eVertexSemantic::Position, 0, Renderer::eDataFormat::R32_G32_B32);
    }
    if (!Normal.empty())
    {
        m_dataStride += sizeof(Vector3);
        m_vertexLayout.AddElement(Renderer::eVertexSemantic::Normal, 0, Renderer::eDataFormat::R32_G32_B32);
    }
    if (!Tangent.empty())
    {
        m_dataStride += sizeof(Vector3);
        m_vertexLayout.AddElement(Renderer::eVertexSemantic::Tangent, 0, Renderer::eDataFormat::R32_G32_B32);
    }
    if (!Bitangent.empty())
    {
        m_dataStride += sizeof(Vector3);
        m_vertexLayout.AddElement(Renderer::eVertexSemantic::Bitangent, 0, Renderer::eDataFormat::R32_G32_B32);
    }
    for (uint8 i = 0; i < MaxColorCount; ++i)
    {
        if (m_color[i].Data.size() > 0)
        {
            assert(m_color[i].Type != eVertexDataType::Type_Unknown);
            m_dataStride += GetDataElementSize(m_color[i]);
            m_vertexLayout.AddElement(Renderer::eVertexSemantic::Color, i, GetVertexDataFormat(m_color[i]));
        }
    }
    for (uint8 i = 0; i < MaxUVCount; ++i)
    {
        if (m_uv[i].Data.size() > 0)
        {
            assert(m_uv[i].Type != eVertexDataType::Type_Unknown);
            m_dataStride += GetDataElementSize(m_uv[i]);
            m_vertexLayout.AddElement(Renderer::eVertexSemantic::Texcoord, i, GetVertexDataFormat(m_uv[i]));
        }
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
        for (uint8 setNum = 0; setNum < MaxColorCount; ++setNum)
        {
            if (m_color[setNum].Data.size() > 0)
            {
                if (m_color[setNum].Type == eVertexDataType::Type_V1)
                    SetValueInPtr<float32>(&currDataPtr, m_color[setNum].Data, i);
                else if (m_color[setNum].Type == eVertexDataType::Type_V2)
                    SetValueInPtr<Vector2>(&currDataPtr, m_color[setNum].Data, i);
                else if (m_color[setNum].Type == eVertexDataType::Type_V3)
                    SetValueInPtr<Vector3>(&currDataPtr, m_color[setNum].Data, i);
                else if (m_color[setNum].Type == eVertexDataType::Type_V4)
                    SetValueInPtr<Vector4>(&currDataPtr, m_color[setNum].Data, i);
            }
        }
        for (uint8 setNum = 0; setNum < MaxUVCount; ++setNum)
        {
            if (m_uv[setNum].Data.size() > 0)
            {
                if (m_uv[setNum].Type == eVertexDataType::Type_V1)
                    SetValueInPtr<float32>(&currDataPtr, m_uv[setNum].Data, i);
                else if (m_uv[setNum].Type == eVertexDataType::Type_V2)
                    SetValueInPtr<Vector2>(&currDataPtr, m_uv[setNum].Data, i);
                else if (m_uv[setNum].Type == eVertexDataType::Type_V3)
                    SetValueInPtr<Vector3>(&currDataPtr, m_uv[setNum].Data, i);
                else if (m_uv[setNum].Type == eVertexDataType::Type_V4)
                    SetValueInPtr<Vector4>(&currDataPtr, m_uv[setNum].Data, i);
            }
        }
    }

    uint32* currIndexPtr = reinterpret_cast<uint32*>(m_indexData);
    assert((Indices.size() % 3) == 0);
    for (uint32 index : Indices)
    {
        *currIndexPtr = index;
        ++currIndexPtr;
    }

    m_handle = Renderer::GenerateVertexLayout(m_vertexLayout); // [a_vorontsov] And here we will generate new layout every time. TODO: Make lookup!
    m_isDirty = false;
}

void Mesh::Cleanup()
{
    SafeDelete(m_data);
    m_dataSize = 0;
    m_dataStride = 0;
    m_vertexCount = 0;

    SafeDelete(m_indexData);
    m_indexDataSize = 0;
    m_indexCount = 0;

    for (uint32 colCnt = 0; colCnt < MaxColorCount; ++colCnt)
    {
        m_color[colCnt].Data.clear();
        m_color[colCnt].Type = eVertexDataType::Type_Unknown;
    }

    for (uint32 uvCnt = 0; uvCnt < MaxColorCount; ++uvCnt)
    {
        m_uv[uvCnt].Data.clear();
        m_uv[uvCnt].Type = eVertexDataType::Type_Unknown;
    }
}

void Mesh::FromIntermediateMesh(const IntermediateMesh& iMesh)
{
    Cleanup();
    Indices = iMesh.Indices;
    for (uint32 i = 0; i < iMesh.Vertices.size(); ++i)
    {
        Position.push_back(Vec4ToVec3(iMesh.Vertices[i].Pos));
        if ((iMesh.LayoutMask & IntermediateMesh::Normal) != 0)
            Normal.push_back(Vec4ToVec3(iMesh.Vertices[i].Norm));
        if ((iMesh.LayoutMask & IntermediateMesh::Tanget) != 0)
            Tangent.push_back(Vec4ToVec3(iMesh.Vertices[i].Tangent));
        if ((iMesh.LayoutMask & IntermediateMesh::Bitangent) != 0)
            Bitangent.push_back(Vec4ToVec3(iMesh.Vertices[i].Bitangent));
        for (uint32 colI = 0; colI< MaxColorCount; ++colI)
        {
            if ((iMesh.LayoutMask & (IntermediateMesh::Color0 << colI)) != 0)
            {
                m_color[colI].Type = eVertexDataType::Type_V4;
                m_color[colI].Data.push_back(iMesh.Vertices[i].Color[colI]);
            }
        }
        for (uint32 uvI = 0; uvI < MaxUVCount; ++uvI)
        {
            if ((iMesh.LayoutMask & (IntermediateMesh::UV0 << uvI)) != 0)
            {
                m_uv[uvI].Type = eVertexDataType::Type_V2;
                m_uv[uvI].Data.push_back(iMesh.Vertices[i].Uv[uvI]);
            }
        }
    }
    PrepareForUpload();
}

uint32 Mesh::GetDataElementSize(const VertexDataElement& data) const
{
    if (data.Type == eVertexDataType::Type_V1)
        return sizeof(float32);
    if (data.Type == eVertexDataType::Type_V2)
        return sizeof(Vector2);
    if (data.Type == eVertexDataType::Type_V3)
        return sizeof(Vector3);
    if (data.Type == eVertexDataType::Type_V4)
        return sizeof(Vector4);
    assert(false);
    return -1;
}

Renderer::eDataFormat Mesh::GetVertexDataFormat(const VertexDataElement& data) const // [a_vorontsov] std::map?
{
    if (data.Type == eVertexDataType::Type_V1)
        return Renderer::eDataFormat::R32;
    if (data.Type == eVertexDataType::Type_V2)
        return Renderer::eDataFormat::R32_G32;
    if (data.Type == eVertexDataType::Type_V3)
        return Renderer::eDataFormat::R32_G32_B32;
    if (data.Type == eVertexDataType::Type_V4)
        return Renderer::eDataFormat::R32_G32_B32_A32;
    assert(false);
    return Renderer::eDataFormat::R32;
}

bool Mesh::CompareVertices(uint32 i, uint32 j) const
{
    bool p = Position[i] == Position[j];
    bool n = Normal[i] == Normal[j];
    bool t = Tangent[i] == Tangent[j];
    bool b = Bitangent[i] == Bitangent[j];
    for (uint32 colCnt = 0; colCnt < MaxColorCount; ++colCnt)
    {
        if (m_color[colCnt].Data.size() == 0)
            continue;
        if (m_color[colCnt].Data[i] != m_color[colCnt].Data[j])
            return false;
    }
    for (uint32 uvCnt = 0; uvCnt < MaxUVCount; ++uvCnt)
    {
        if (m_uv[uvCnt].Data.size() == 0)
            continue;
        if (m_uv[uvCnt].Data[i] != m_uv[uvCnt].Data[j])
            return false;
    }
    return p && n && t && b;
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
    m_color = std::move(other.m_color);
    m_uv = std::move(other.m_uv);
    Indices = std::move(other.Indices);

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
    m_color = other.m_color;
    m_uv = other.m_uv;
    Indices = other.Indices;

    m_vertexLayout = other.m_vertexLayout;

    m_handle = other.m_handle;
    m_isDirty = other.m_isDirty;
    m_isDynamic = other.m_isDynamic;

    return *this;
}
}