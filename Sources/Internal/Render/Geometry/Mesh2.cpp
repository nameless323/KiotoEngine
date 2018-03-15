#include "stdafx.h"

#include "Render/Geometry/Mesh2.h"

#include "Render/Geometry/IntermediateMesh.h"

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

void Mesh2::FromIntermediateMesh(const IntermediateMesh& iMesh)
{
    SafeDelete(m_vertexData);
    SafeDelete(m_indexData);

    m_layout.Clear();

    m_indexCount = static_cast<uint32>(iMesh.Indices.size());
    m_vertexCount = static_cast<uint32>(iMesh.Indices.size());

    LayoutFromIntermediateMesh(iMesh);
    m_vertexDataSize = m_layout.GetVertexStride() * m_vertexCount;
    m_indexDataSize = m_indexCount * sizeof(uint32);

    for (uint32 i = 0; i < m_indexCount; ++i)
        *GetIndexPtr(i) = iMesh.Indices[i];

    uint32 elemCount = m_layout.GetElementsCount();
    for (uint32 vElem = 0; vElem < elemCount; vElem++)
    {
        if (m_layout.GetElement(vElem).Semantic == Renderer::eVertexSemantic::Position)
        {
            for (uint32 i = 0; i < m_vertexCount; ++i)
                *GetPositionPtr(i) = iMesh.Vertices[i].Pos.GetVec3();
        }
        else if (m_layout.GetElement(vElem).Semantic == Renderer::eVertexSemantic::Normal)
        {
            for (uint32 i = 0; i < m_vertexCount; ++i)
                *GetNormalPtr(i) = iMesh.Vertices[i].Norm.GetVec3();
        }
        else if (m_layout.GetElement(vElem).Semantic == Renderer::eVertexSemantic::Tangent)
        {
            for (uint32 i = 0; i < m_vertexCount; ++i)
                *GetVertexElementPtr<Vector3>(i, Renderer::eVertexSemantic::Tangent, 0) = iMesh.Vertices[i].Tangent.GetVec3();
        }
        else if (m_layout.GetElement(vElem).Semantic == Renderer::eVertexSemantic::Bitangent)
        {
            for (uint32 i = 0; i < m_vertexCount; ++i)
                *GetVertexElementPtr<Vector3>(i, Renderer::eVertexSemantic::Bitangent, 0) = iMesh.Vertices[i].Bitangent.GetVec3();
        }
        else if (const Renderer::VertexDesc& desc = m_layout.GetElement(vElem); desc.Semantic == Renderer::eVertexSemantic::Texcoord)
        {
            uint32 texSemIndex = desc.SemanticIndex;
            for (uint32 i = 0; i < m_vertexCount; ++i)
                *GetVertexElementPtr<Vector2>(i, Renderer::eVertexSemantic::Texcoord, texSemIndex) = iMesh.Vertices[i].Uv[texSemIndex];
        }
        else if (const Renderer::VertexDesc& desc = m_layout.GetElement(vElem); desc.Semantic == Renderer::eVertexSemantic::Color)
        {
            uint32 texSemIndex = desc.SemanticIndex;
            for (uint32 i = 0; i < m_vertexCount; ++i)
                *GetVertexElementPtr<Vector4>(i, Renderer::eVertexSemantic::Color, texSemIndex) = iMesh.Vertices[i].Color[texSemIndex];
        }
    }
}

void Mesh2::LayoutFromIntermediateMesh(const IntermediateMesh& iMesh)
{
    m_layout.AddElement(Renderer::eVertexSemantic::Position, 0, Renderer::eDataFormat::R8_G8_B8);
    if ((iMesh.LayoutMask & IntermediateMesh::Normal) != 0)
        m_layout.AddElement(Renderer::eVertexSemantic::Normal, 0, Renderer::eDataFormat::R8_G8_B8);
    if ((iMesh.LayoutMask & IntermediateMesh::Tanget) != 0)
        m_layout.AddElement(Renderer::eVertexSemantic::Tangent, 0, Renderer::eDataFormat::R8_G8_B8);
    if ((iMesh.LayoutMask & IntermediateMesh::Bitangent) != 0)
        m_layout.AddElement(Renderer::eVertexSemantic::Bitangent, 0, Renderer::eDataFormat::R8_G8_B8);

    for (uint32 uvSemIndex = 0; uvSemIndex < MaxTexcoordCount; ++uvSemIndex)
    {
        if ((iMesh.LayoutMask & (IntermediateMesh::UV0 << uvSemIndex)) != 0)
            m_layout.AddElement(Renderer::eVertexSemantic::Texcoord, uvSemIndex, Renderer::eDataFormat::R8_G8);
    }
    for (uint32 colSemIndex = 0; colSemIndex < MaxTexcoordCount; ++colSemIndex)
    {
        if ((iMesh.LayoutMask & (IntermediateMesh::Color0 << colSemIndex)) != 0)
            m_layout.AddElement(Renderer::eVertexSemantic::Color, colSemIndex, Renderer::eDataFormat::R8_G8_B8_A8);
    }
}

Mesh2& Mesh2::operator=(Mesh2 other)
{
    swap(*this, other);
    return *this;
}

}