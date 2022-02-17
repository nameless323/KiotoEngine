#include "Render/Geometry/Mesh.h"

#include "Core/CoreHelpers.h"
#include "Render/Geometry/IntermediateMesh.h"
#include "Render/Geometry/MeshLoader.h"

namespace Kioto::Renderer
{

Mesh::Mesh(VertexLayout layout, uint32 vertexCount, uint32 indexCount)
    : Asset("")
{
    InitFromLayout(std::move(layout), vertexCount, indexCount);
}

Mesh::Mesh(const std::string& path)
    : Asset(path)
{
    MeshLoader::LoadMesh(this);
}

Mesh::Mesh(const Mesh& other)
    : Asset(other.GetAssetPath())
    , mVertexData(other.mVertexData)
    , mVertexDataSize(other.mVertexDataSize)
    , mIndexData(other.mIndexData)
    , mIndexDataSize(other.mIndexDataSize)
    , mVertexCount(other.mVertexCount)
    , mIndexCount(other.mIndexCount)
    , mLayout(other.mLayout)
{
}

Mesh::Mesh(Mesh&& other)
    : Asset(other.GetAssetPath())
{
    swap(*this, other);
}

Mesh::~Mesh()
{
    SafeDelete(mVertexData);
    SafeDelete(mIndexData);
}

void Mesh::InitFromLayout(Renderer::VertexLayout layout, uint32 vertexCount, uint32 indexCount)
{
    mVertexCount = vertexCount;
    mIndexCount = indexCount;
    swap(mLayout, layout);

    SafeDelete(mVertexData);
    mVertexDataSize = mLayout.GetVertexStride() * vertexCount;

    SafeDelete(mIndexData);
    mIndexDataSize = indexCount * sizeof(uint32);

    mVertexData = new byte[mVertexDataSize];
    mIndexData = new byte[mIndexDataSize];
}

void Mesh::FromIntermediateMesh(const IntermediateMesh& iMesh)
{
    SafeDelete(mVertexData);
    SafeDelete(mIndexData);

    mLayout.Clear();

    mIndexCount = static_cast<uint32>(iMesh.Indices.size());
    mVertexCount = static_cast<uint32>(iMesh.Vertices.size());

    LayoutFromIntermediateMesh(iMesh);
    mVertexDataSize = mLayout.GetVertexStride() * mVertexCount;
    mIndexDataSize = mIndexCount * sizeof(uint32);

    mVertexData = new byte[mVertexDataSize];
    mIndexData = new byte[mIndexDataSize];

    for (uint32 i = 0; i < mIndexCount; ++i)
        *GetIndexPtr(i) = iMesh.Indices[i];

    uint32 elemCount = mLayout.GetElementsCount();
    for (uint32 vElem = 0; vElem < elemCount; vElem++)
    {
        if (mLayout.GetElement(vElem).Semantic == Renderer::eVertexSemantic::Position)
        {
            for (uint32 i = 0; i < mVertexCount; ++i)
                *GetPositionPtr(i) = iMesh.Vertices[i].Pos.GetVec3();
        }
        else if (mLayout.GetElement(vElem).Semantic == Renderer::eVertexSemantic::Normal)
        {
            for (uint32 i = 0; i < mVertexCount; ++i)
                *GetNormalPtr(i) = iMesh.Vertices[i].Norm.GetVec3();
        }
        else if (mLayout.GetElement(vElem).Semantic == Renderer::eVertexSemantic::Tangent)
        {
            for (uint32 i = 0; i < mVertexCount; ++i)
                *GetVertexElementPtr<Vector3>(i, Renderer::eVertexSemantic::Tangent, 0) = iMesh.Vertices[i].Tangent.GetVec3();
        }
        else if (mLayout.GetElement(vElem).Semantic == Renderer::eVertexSemantic::Bitangent)
        {
            for (uint32 i = 0; i < mVertexCount; ++i)
                *GetVertexElementPtr<Vector3>(i, Renderer::eVertexSemantic::Bitangent, 0) = iMesh.Vertices[i].Bitangent.GetVec3();
        }
        else if (const Renderer::SemanticDesc& desc = mLayout.GetElement(vElem); desc.Semantic == Renderer::eVertexSemantic::Texcoord)
        {
            uint32 texSemIndex = desc.SemanticIndex;
            for (uint32 i = 0; i < mVertexCount; ++i)
                *GetVertexElementPtr<Vector2>(i, Renderer::eVertexSemantic::Texcoord, texSemIndex) = iMesh.Vertices[i].Uv[texSemIndex];
        }
        else if (const Renderer::SemanticDesc& desc = mLayout.GetElement(vElem); desc.Semantic == Renderer::eVertexSemantic::Color)
        {
            uint32 texSemIndex = desc.SemanticIndex;
            for (uint32 i = 0; i < mVertexCount; ++i)
                *GetVertexElementPtr<Vector4>(i, Renderer::eVertexSemantic::Color, texSemIndex) = iMesh.Vertices[i].Color[texSemIndex];
        }
    }
}

void Mesh::LayoutFromIntermediateMesh(const IntermediateMesh& iMesh)
{
    mLayout.AddElement(Renderer::eVertexSemantic::Position, 0, Renderer::eDataFormat::R8_G8_B8);
    if ((iMesh.LayoutMask & IntermediateMesh::Normal) != 0)
        mLayout.AddElement(Renderer::eVertexSemantic::Normal, 0, Renderer::eDataFormat::R8_G8_B8);
    if ((iMesh.LayoutMask & IntermediateMesh::Tanget) != 0)
        mLayout.AddElement(Renderer::eVertexSemantic::Tangent, 0, Renderer::eDataFormat::R8_G8_B8);
    if ((iMesh.LayoutMask & IntermediateMesh::Bitangent) != 0)
        mLayout.AddElement(Renderer::eVertexSemantic::Bitangent, 0, Renderer::eDataFormat::R8_G8_B8);

    for (uint32 uvSemIndex = 0; uvSemIndex < MaxTexcoordCount; ++uvSemIndex)
    {
        if ((iMesh.LayoutMask & (IntermediateMesh::UV0 << uvSemIndex)) != 0)
            mLayout.AddElement(Renderer::eVertexSemantic::Texcoord, uvSemIndex, Renderer::eDataFormat::R8_G8);
    }
    for (uint32 colSemIndex = 0; colSemIndex < MaxTexcoordCount; ++colSemIndex)
    {
        if ((iMesh.LayoutMask & (IntermediateMesh::Color0 << colSemIndex)) != 0)
            mLayout.AddElement(Renderer::eVertexSemantic::Color, colSemIndex, Renderer::eDataFormat::R8_G8_B8_A8);
    }
}

Mesh& Mesh::operator=(Mesh other)
{
    swap(*this, other);
    return *this;
}

}